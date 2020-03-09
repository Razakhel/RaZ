#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "RaZ/Render/RendererVk.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <optional>
#include <set>

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

///////////////
// Swapchain //
///////////////

constexpr std::array<const char*, 1> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

inline bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  std::size_t validExtensionCount = 0;

  for (const char* extensionName : deviceExtensions) {
    for (const VkExtensionProperties& extensionProperties : availableExtensions) {
      if (std::strcmp(extensionName, extensionProperties.extensionName) == 0)
        ++validExtensionCount;
    }
  }

  return (validExtensionCount == deviceExtensions.size());
}

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities {};
  std::vector<VkSurfaceFormatKHR> formats {};
  std::vector<VkPresentModeKHR> presentModes {};
};

inline VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return availableFormat;
  }

  return availableFormats.front();
}

inline VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
  for (const VkPresentModeKHR& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      return availablePresentMode;
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

inline VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* windowHandle) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    return capabilities.currentExtent;

  // Width & height may be user-defined
  int width {};
  int height {};
  glfwGetWindowSize(windowHandle, &width, &height);

  VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

  actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
  actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

  return actualExtent;
}

inline SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapchainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
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

  const QueueFamilyIndices indices  = findQueueFamilies(device, surface);
  const bool areExtensionsSupported = checkDeviceExtensionSupport(device);

  bool isSwapchainSupported = false;
  if (areExtensionsSupported) {
    const SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device, surface);
    isSwapchainSupported = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
  }

  return indices.isComplete() && areExtensionsSupported && isSwapchainSupported;
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
    throw std::runtime_error("Error: Failed to create the Vulkan instance.");

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
      throw std::runtime_error("Error: Failed to create the window surface.");
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
  deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
  deviceCreateInfo.pEnabledFeatures        = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
#if !defined(NDEBUG)
  deviceCreateInfo.enabledLayerCount       = static_cast<uint32_t>(validationLayers.size());
  deviceCreateInfo.ppEnabledLayerNames     = validationLayers.data();
#endif

  if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create the logical device.");

  // Recovering our queues
  vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);

  ///////////////
  // Swapchain //
  ///////////////

  SwapchainSupportDetails swapchainSupport = querySwapchainSupport(m_physicalDevice, m_surface);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
  VkPresentModeKHR presentMode     = chooseSwapPresentMode(swapchainSupport.presentModes);
  VkExtent2D extent                = chooseSwapExtent(swapchainSupport.capabilities, windowHandle);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

  if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
    imageCount = swapchainSupport.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR swapchainCreateInfo {};
  swapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface          = m_surface;
  swapchainCreateInfo.minImageCount    = imageCount;
  swapchainCreateInfo.imageFormat      = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace  = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent      = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const std::array<uint32_t, 2> queueFamilyIndices = { indices.graphicsFamily.value(), indices.presentFamily.value() };

  if (indices.graphicsFamily != indices.presentFamily) {
    swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices.data();
  } else {
    swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices   = nullptr;
  }

  swapchainCreateInfo.preTransform   = swapchainSupport.capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode    = presentMode;
  swapchainCreateInfo.clipped        = VK_TRUE;
  swapchainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(m_logicalDevice, &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create the swapchain.");

  vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchain, &imageCount, nullptr);

  m_swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(m_logicalDevice, m_swapchain, &imageCount, m_swapchainImages.data());

  m_swapchainImageFormat = surfaceFormat.format;
  m_swapchainExtent      = extent;

  s_isInitialized = true;
}

void Renderer::destroy() {
#if !defined(NDEBUG)
  destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif

  vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);
  vkDestroyDevice(m_logicalDevice, nullptr);
  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  vkDestroyInstance(m_instance, nullptr);

  s_isInitialized = false;
}

} // namespace Raz
