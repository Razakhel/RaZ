#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "RaZ/Render/RendererVk.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

namespace Raz {

namespace {

///////////////////////
// Validation layers //
///////////////////////

constexpr std::array<const char*, 1> validationLayers = {
  "VK_LAYER_KHRONOS_validation" // VK_LAYER_LUNARG_standard_validation
};

inline bool checkValidationLayersSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layerName : validationLayers) {
    bool layerFound = false;

    for (const VkLayerProperties& layerProperties : availableLayers) {
      if (std::strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound)
      return false;
  }

  return true;
}

////////////////////
// Debug Callback //
////////////////////

inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /* messageSeverity */,
                                                    VkDebugUtilsMessageTypeFlagsEXT /* messageType */,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                    void* /* userData */) {
  std::cerr << "[Validation layer] Error: " << callbackData->pMessage << std::endl;

  return VK_FALSE;
}

inline VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
                                             const VkAllocationCallbacks* allocator,
                                             VkDebugUtilsMessengerEXT& debugMessenger) {
  const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

  if (func != nullptr)
    return func(instance, createInfo, allocator, &debugMessenger);

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

inline void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                          VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks* allocator) {
  const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

  if (func != nullptr)
    func(instance, debugMessenger, allocator);
}

constexpr VkDebugUtilsMessengerCreateInfoEXT createDebugMessengerCreateInfo(void* userData = nullptr) noexcept {
  VkDebugUtilsMessengerCreateInfoEXT createInfo {};

  createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                             //| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData       = userData;

  return createInfo;
}

inline void setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT& debugMessenger, void* userData = nullptr) {
  const VkDebugUtilsMessengerCreateInfoEXT createInfo = createDebugMessengerCreateInfo(userData);

  if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, debugMessenger) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to set up debug messenger.");
}

////////////////
// Extensions //
////////////////

inline std::vector<const char*> getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if !defined(NDEBUG)
  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

  return extensions;
}

////////////////////
// Queue families //
////////////////////

struct QueueFamilyIndices {
  constexpr bool isComplete() const noexcept { return graphicsFamily.has_value() && presentFamily.has_value(); }

  std::optional<uint32_t> graphicsFamily {};
  std::optional<uint32_t> presentFamily {};
};

inline QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  for (std::size_t queueIndex = 0; queueIndex < queueFamilies.size(); ++queueIndex) {
    if (queueFamilies[queueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = queueIndex;

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, static_cast<uint32_t>(queueIndex), surface, &presentSupport);

      if (presentSupport)
        indices.presentFamily = queueIndex;

      if (indices.isComplete())
        break;
    }
  }

  return indices;
}

/////////////////////
// Physical device //
/////////////////////

inline bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  // TODO: pick the best device

  const QueueFamilyIndices indices = findQueueFamilies(device, surface);

  return indices.isComplete();
}

} // namespace

void Renderer::initialize(GLFWwindow* windowHandle) {
  ///////////////////////
  // Validation layers //
  ///////////////////////

#if !defined(NDEBUG)
  if (!checkValidationLayersSupport())
    throw std::runtime_error("Error: Validation layers are not supported.");
#endif

  //////////////
  // Instance //
  //////////////

  VkApplicationInfo appInfo {};
  appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName   = "RaZ";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  appInfo.pEngineName        = "RaZ";
  appInfo.engineVersion      = VK_MAKE_VERSION(0, 0, 0);
  appInfo.apiVersion         = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceCreateInfo {};
  instanceCreateInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pApplicationInfo = &appInfo;

  const std::vector<const char*> requiredExtensions = getRequiredExtensions();
  instanceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

#if !defined(NDEBUG)
  instanceCreateInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
  instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

  const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = createDebugMessengerCreateInfo();
  instanceCreateInfo.pNext = &debugCreateInfo;
#endif

  if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create a Vulkan instance.");

  ////////////////////
  // Debug Callback //
  ////////////////////

#if !defined(NDEBUG)
  setupDebugMessenger(m_instance, m_debugMessenger);
#endif

  ////////////////////
  // Window surface //
  ////////////////////

  if (windowHandle != nullptr) {
    if (glfwCreateWindowSurface(m_instance, windowHandle, nullptr, &m_surface) != VK_SUCCESS)
      throw std::runtime_error("Error: Failed to create window surface.");
  }

  ////////////////
  // Extensions //
  ////////////////

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  std::cout << "Available Vulkan extensions:\n";

  for (const VkExtensionProperties& extension : extensions)
    std::cout << "\t" << extension.extensionName << '\n';

  std::cout << std::flush;

  /////////////////////
  // Physical device //
  /////////////////////

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

  if (deviceCount == 0)
    throw std::runtime_error("Error: No GPU available with Vulkan support.");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

  for (const VkPhysicalDevice& device : devices) {
    if (isDeviceSuitable(device, m_surface)) {
      m_physicalDevice = device;
      break;
    }
  }

  if (m_physicalDevice == nullptr)
    throw std::runtime_error("Error: No suitable GPU available.");

  ////////////////////
  // Logical device //
  ////////////////////

  const QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice, m_surface);

  // Recovering the necessary queues
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  const std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
  float queuePriority = 1.0f;

  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo {};
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.emplace_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures {};

  VkDeviceCreateInfo deviceCreateInfo {};
  deviceCreateInfo.sType                 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount  = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos     = queueCreateInfos.data();
  deviceCreateInfo.pEnabledFeatures      = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount = 0;
#if !defined(NDEBUG)
  deviceCreateInfo.enabledLayerCount     = static_cast<uint32_t>(validationLayers.size());
  deviceCreateInfo.ppEnabledLayerNames   = validationLayers.data();
#endif

  if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create logical device.");

  // Recovering our queues
  vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentationQueue);

  s_isInitialized = true;
}

void Renderer::destroy() {
#if !defined(NDEBUG)
  destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif

  vkDestroyDevice(m_logicalDevice, nullptr);
  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  vkDestroyInstance(m_instance, nullptr);

  s_isInitialized = false;
}

} // namespace Raz
