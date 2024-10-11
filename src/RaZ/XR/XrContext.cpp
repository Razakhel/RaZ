#include "RaZ/Utils/Logger.hpp"
#include "RaZ/XR/XrContext.hpp"

#if defined(XR_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Unknwn.h>
#elif defined(XR_OS_LINUX)
#include "GL/glx.h"
#endif

#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"
#include "tracy/Tracy.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>

namespace Raz {

namespace {

const char* getResultStr(XrInstance instance, XrResult result) {
  static std::array<char, XR_MAX_RESULT_STRING_SIZE> errorStr {};
  xrResultToString(instance, result, errorStr.data());
  return errorStr.data();
}

std::string getErrorStr(const std::string& errorMsg, XrResult result, XrInstance instance) {
  return "[XrContext] " + errorMsg + ": " + getResultStr(instance, result) + " (" + std::to_string(result) + ')';
}

void checkLog(XrResult result, const std::string& errorMsg, XrInstance instance) {
  if (XR_SUCCEEDED(result))
    return;

  Logger::error(getErrorStr(errorMsg, result, instance));
}

void checkThrow(XrResult result, const std::string& errorMsg, XrInstance instance) {
  if (XR_SUCCEEDED(result))
    return;

  throw std::runtime_error(getErrorStr(errorMsg, result, instance));
}

XrBool32 logCallback(XrDebugUtilsMessageSeverityFlagsEXT severity,
                     XrDebugUtilsMessageTypeFlagsEXT type,
                     const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
                     void* /* userData */) {
  std::string severityStr;
  LoggingLevel logLevel = LoggingLevel::INFO;

  if (severity & XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    severityStr += "INFO";

  if (severity & XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    if (!severityStr.empty())
      severityStr += ',';
    severityStr += "WARN";

    logLevel = LoggingLevel::WARNING;
  }

  if (severity & XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    if (!severityStr.empty())
      severityStr += ',';
    severityStr += "ERROR";

    logLevel = LoggingLevel::ERROR;
  }

  constexpr auto getTypeStr = [] (XrDebugUtilsMessageTypeFlagsEXT messageType) {
    std::string msgFlags;

    if (messageType & XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
      msgFlags += "GEN";

    if (messageType & XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
      if (!msgFlags.empty())
        msgFlags += ',';
      msgFlags += "SPEC";
    }

    if (messageType & XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
      if (!msgFlags.empty())
        msgFlags += ',';
      msgFlags += "PERF";
    }

    return msgFlags;
  };

  std::string logMsg = "[OpenXR] ";

  if (callbackData->functionName)
    logMsg += callbackData->functionName;

  logMsg += '(' + severityStr + '/' + getTypeStr(type) + "): ";

  if (callbackData->messageId)
    logMsg += "ID: " + std::string(callbackData->messageId);

  if (callbackData->message)
    logMsg += " - " + std::string(callbackData->message);

  switch (logLevel) {
    case LoggingLevel::INFO:
      Logger::info(logMsg);
      break;

    case LoggingLevel::WARNING:
      Logger::warn(logMsg);
      break;

    case LoggingLevel::ERROR:
      Logger::error(logMsg);
      break;

    default:
      break;
  }

  // Returning false to indicate that the call should not be aborted
  // See: https://registry.khronos.org/OpenXR/specs/1.1/html/xrspec.html#PFN_xrDebugUtilsMessengerCallbackEXT
  return false;
}

} // namespace

XrContext::XrContext(const std::string& appName) {
  ZoneScopedN("XrContext::XrContext");

  Logger::debug("[XrContext] Creating context...");

  recoverApiLayers();
  recoverExtensions();
  createInstance(appName);

  XrInstanceProperties instanceProperties {};
  instanceProperties.type = XR_TYPE_INSTANCE_PROPERTIES;
  checkLog(xrGetInstanceProperties(m_instance, &instanceProperties), "Failed to get instance properties", m_instance);

  Logger::info("[XrContext] OpenXR runtime: " + std::string(instanceProperties.runtimeName) + " - "
                                              + std::to_string(XR_VERSION_MAJOR(instanceProperties.runtimeVersion)) + '.'
                                              + std::to_string(XR_VERSION_MINOR(instanceProperties.runtimeVersion)) + '.'
                                              + std::to_string(XR_VERSION_PATCH(instanceProperties.runtimeVersion)));

  createDebugMessenger();

  XrSystemGetInfo systemGetInfo {};
  systemGetInfo.type       = XR_TYPE_SYSTEM_GET_INFO;
  systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY; // Always search for a VR headset for now
  checkThrow(xrGetSystem(m_instance, &systemGetInfo, &m_systemId), "Failed to get system ID", m_instance);

  XrSystemProperties m_systemProperties {};
  m_systemProperties.type = XR_TYPE_SYSTEM_PROPERTIES;
  checkLog(xrGetSystemProperties(m_instance, m_systemId, &m_systemProperties), "Failed to get system properties", m_instance);

  Logger::debug("[XrContext] System properties:"
                "\n    - System ID:             " + std::to_string(m_systemProperties.systemId)
              + "\n    - Vendor ID:             " + std::to_string(m_systemProperties.vendorId)
              + "\n    - Name:                  " + m_systemProperties.systemName
              + "\n    - Max. swapchain height: " + std::to_string(m_systemProperties.graphicsProperties.maxSwapchainImageHeight)
              + "\n    - Max. swapchain width:  " + std::to_string(m_systemProperties.graphicsProperties.maxSwapchainImageWidth)
              + "\n    - Max. layer count:      " + std::to_string(m_systemProperties.graphicsProperties.maxLayerCount)
              + "\n    - Orientation tracking:  " + (m_systemProperties.trackingProperties.orientationTracking == XR_TRUE ? "true" : "false")
              + "\n    - Position tracking:     " + (m_systemProperties.trackingProperties.positionTracking == XR_TRUE ? "true" : "false"));

  Logger::debug("[XrContext] Created context");
}

XrContext::~XrContext() {
  ZoneScopedN("XrContext::~XrContext");

  Logger::debug("[XrContext] Destroying context...");
  destroyDebugMessenger();
  destroyInstance();
  Logger::debug("[XrContext] Destroyed context");
}

void XrContext::recoverApiLayers() {
  ZoneScopedN("XrContext::recoverApiLayers");

  Logger::debug("[XrContext] Recovering API layers...");

  uint32_t apiLayerCount {};
  std::vector<XrApiLayerProperties> apiLayerProperties;
  checkLog(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to get API layer property count", m_instance);
  apiLayerProperties.resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
  checkLog(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate API layer properties", m_instance);

#if !defined(NDEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  {
    std::string availableLayersMsg = "[XrContext] Available layers:";
    for (const XrApiLayerProperties& layerProp : apiLayerProperties) {
      availableLayersMsg += "\n    " + std::string(layerProp.layerName) +
                            "\n     -> " + std::string(layerProp.description);
    }
    Logger::debug(availableLayersMsg);
  }
#endif

  for (const std::string& requestedLayer : m_apiLayers) {
    const auto layerPropIter = std::find_if(apiLayerProperties.cbegin(), apiLayerProperties.cend(),
                                            [&requestedLayer] (const XrApiLayerProperties& layerProp) { return requestedLayer == layerProp.layerName; });

    if (layerPropIter != apiLayerProperties.cend())
      m_activeApiLayers.emplace_back(requestedLayer.c_str());
    else
      Logger::error("[XrContext] Failed to find OpenXR API layer: " + requestedLayer);
  }

#if !defined(NDEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  if (!m_activeApiLayers.empty()) {
    std::string activeLayersMsg = "[XrContext] Active layers:";
    for (const char* activeLayer : m_activeApiLayers)
      activeLayersMsg += "\n    " + std::string(activeLayer);
    Logger::debug(activeLayersMsg);
  }
#endif

  Logger::debug("[XrContext] Recovered API layers");
}

void XrContext::recoverExtensions() {
  ZoneScopedN("XrContext::recoverExtensions");

  Logger::debug("[XrContext] Recovering extensions...");

  m_extensions.emplace_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
  m_extensions.emplace_back(XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);

  uint32_t extensionCount {};
  std::vector<XrExtensionProperties> extensionProperties;
  checkLog(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to get instance extension property count", m_instance);
  extensionProperties.resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
  checkLog(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()),
           "Failed to enumerate instance extension properties",
           m_instance);

#if !defined(NDEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  {
    std::string availableExtensionsMsg = "[XrContext] Available extensions:";
    for (const XrExtensionProperties& extProp : extensionProperties) {
      const std::string extVersionStr = std::to_string(XR_VERSION_MAJOR(extProp.extensionVersion)) + '.'
                                      + std::to_string(XR_VERSION_MINOR(extProp.extensionVersion)) + '.'
                                      + std::to_string(XR_VERSION_PATCH(extProp.extensionVersion));
      availableExtensionsMsg += "\n    " + std::string(extProp.extensionName) + " - " + extVersionStr;
    }
    Logger::debug(availableExtensionsMsg);
  }
#endif

  for (const std::string& requestedExtension : m_extensions) {
    const auto extIter = std::find_if(extensionProperties.cbegin(), extensionProperties.cend(), [&requestedExtension] (const XrExtensionProperties& extProp) {
      return requestedExtension == extProp.extensionName;
    });

    if (extIter != extensionProperties.cend())
      m_activeExtensions.emplace_back(requestedExtension.c_str());
    else
      Logger::error("[XrContext] Failed to find OpenXR instance extension: " + requestedExtension);
  }

#if !defined(NDEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  if (!m_activeExtensions.empty()) {
    std::string activeExtensionsMsg = "[XrContext] Active extensions:";
    for (const char* activeExt : m_activeExtensions)
      activeExtensionsMsg += "\n    " + std::string(activeExt);
    Logger::debug(activeExtensionsMsg);
  }
#endif

  Logger::debug("[XrContext] Recovered extensions");
}

void XrContext::createInstance(const std::string& appName) {
  ZoneScopedN("XrContext::createInstance");

  Logger::debug("[XrContext] Creating instance...");

  XrApplicationInfo appInfo {};
  appName.copy(appInfo.applicationName, XR_MAX_APPLICATION_NAME_SIZE - 1);
  appInfo.applicationVersion = 1;
  std::string("RaZ").copy(appInfo.engineName, XR_MAX_ENGINE_NAME_SIZE - 1);
  appInfo.engineVersion      = 1;
  appInfo.apiVersion         = XR_MAKE_VERSION(1, 0, 34); // XR_CURRENT_API_VERSION is too recent for the tested runtime

  XrInstanceCreateInfo instanceCreateInfo {};
  instanceCreateInfo.type                  = XR_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.createFlags           = 0;
  instanceCreateInfo.applicationInfo       = appInfo;
  instanceCreateInfo.enabledApiLayerCount  = static_cast<uint32_t>(m_activeApiLayers.size());
  instanceCreateInfo.enabledApiLayerNames  = m_activeApiLayers.data();
  instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_activeExtensions.size());
  instanceCreateInfo.enabledExtensionNames = m_activeExtensions.data();
  checkThrow(xrCreateInstance(&instanceCreateInfo, &m_instance), "Failed to create instance", m_instance);

  Logger::debug("[XrContext] Created instance");
}

void XrContext::destroyInstance() {
  ZoneScopedN("XrContext::destroyInstance");

  Logger::debug("[XrContext] Destroying instance...");
  checkLog(xrDestroyInstance(m_instance), "Failed to destroy instance", m_instance);
  Logger::debug("[XrContext] Destroyed instance");
}

void XrContext::createDebugMessenger() {
  ZoneScopedN("XrContext::createDebugMessenger");

  if (std::find(m_activeExtensions.cbegin(), m_activeExtensions.cend(), std::string_view(XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) == m_activeExtensions.cend())
    return; // Extension not found

  Logger::debug("[XrContext] Creating debug messenger...");

  XrDebugUtilsMessengerCreateInfoEXT debugMsgrCreateInfo {};
  debugMsgrCreateInfo.type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugMsgrCreateInfo.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                        | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                        | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; // Ignoring verbose severity
  debugMsgrCreateInfo.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                   | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                   | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                                   | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
  debugMsgrCreateInfo.userCallback = logCallback;
  debugMsgrCreateInfo.userData     = nullptr;

  PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT {};
  checkLog(xrGetInstanceProcAddr(m_instance,
                                 "xrCreateDebugUtilsMessengerEXT",
                                 reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateDebugUtilsMessengerEXT)),
           "Failed to get debug messenger create function",
           m_instance);

  checkLog(xrCreateDebugUtilsMessengerEXT(m_instance, &debugMsgrCreateInfo, &m_debugMsgr), "Failed to create debug messenger", m_instance);

  Logger::debug("[XrContext] Created debug messenger");
}

void XrContext::destroyDebugMessenger() {
  ZoneScopedN("XrContext::destroyDebugMessenger");

  if (m_debugMsgr == XR_NULL_HANDLE)
    return;

  if (std::find(m_activeExtensions.cbegin(), m_activeExtensions.cend(), std::string_view(XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) == m_activeExtensions.cend())
    return; // Extension not found

  Logger::debug("[XrContext] Destroying debug messenger...");

  PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT {};
  checkLog(xrGetInstanceProcAddr(m_instance,
                                 "xrDestroyDebugUtilsMessengerEXT",
                                 reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyDebugUtilsMessengerEXT)),
           "Failed to get debug messenger destroy function",
           m_instance);

  checkLog(xrDestroyDebugUtilsMessengerEXT(m_debugMsgr), "Failed to destroy debug messenger", m_instance);

  Logger::debug("[XrContext] Destroyed debug messenger");
}

} // namespace Raz
