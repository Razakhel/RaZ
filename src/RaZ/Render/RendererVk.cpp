#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "RaZ/Render/RendererVk.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"

using namespace std::literals;

namespace Raz {

namespace {

struct Vertex {
  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription {};

    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

    // Position
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = 0; // offsetof(Vertex, position)

    // Color
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset   = sizeof(Vertex::position); // offsetof(Vertex, color)

    return attributeDescriptions;
  }

  Vec3f position;
  Vec3f color;
};

struct UniformMatrices {
  Mat4f model {};
  Mat4f view {};
  Mat4f projection {};
};

constexpr std::array<Vertex, 4> vertices = {
  Vertex { Vec3f(-0.5f, -0.5f, 0.f), Vec3f(1.f, 0.f, 0.f) },
  Vertex { Vec3f( 0.5f, -0.5f, 0.f), Vec3f(0.f, 1.f, 0.f) },
  Vertex { Vec3f( 0.5f,  0.5f, 0.f), Vec3f(0.f, 0.f, 1.f) },
  Vertex { Vec3f(-0.5f,  0.5f, 0.f), Vec3f(1.f, 1.f, 1.f) }
};

constexpr std::array<uint32_t, 6> indices = {
  0, 1, 2,
  2, 3, 0
};

///////////////////////
// Validation layers //
///////////////////////

constexpr std::array<const char*, 1> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};

inline bool checkValidationLayersSupport() {
  uint32_t layerCount {};
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
  const char** glfwExtensions {};
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
  QueueFamilyIndices queueIndices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  for (std::size_t queueIndex = 0; queueIndex < queueFamilies.size(); ++queueIndex) {
    if (queueFamilies[queueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueIndices.graphicsFamily = static_cast<uint32_t>(queueIndex);

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, static_cast<uint32_t>(queueIndex), surface, &presentSupport);

      if (presentSupport)
        queueIndices.presentFamily = static_cast<uint32_t>(queueIndex);

      if (queueIndices.isComplete())
        break;
    }
  }

  return queueIndices;
}

///////////////
// Swapchain //
///////////////

constexpr std::array<const char*, 1> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

inline bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount {};
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

  uint32_t formatCount {};
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount {};
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

inline void createSwapchain(VkPhysicalDevice physicalDevice,
                            VkSurfaceKHR surface,
                            GLFWwindow* windowHandle,
                            VkDevice logicalDevice,
                            VkSwapchainKHR& swapchain,
                            std::vector<VkImage>& swapchainImages,
                            VkFormat& swapchainImageFormat,
                            VkExtent2D& swapchainExtent) {
  const SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surface);

  const VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
  const VkPresentModeKHR presentMode     = chooseSwapPresentMode(swapchainSupport.presentModes);
  const VkExtent2D extent                = chooseSwapExtent(swapchainSupport.capabilities, windowHandle);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

  if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
    imageCount = swapchainSupport.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR swapchainCreateInfo {};
  swapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface          = surface;
  swapchainCreateInfo.minImageCount    = imageCount;
  swapchainCreateInfo.imageFormat      = surfaceFormat.format;
  swapchainCreateInfo.imageColorSpace  = surfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent      = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const QueueFamilyIndices queueIndices = findQueueFamilies(physicalDevice, surface);
  const std::array<uint32_t, 2> queueFamilyIndices = { queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value() };

  if (queueIndices.graphicsFamily != queueIndices.presentFamily) {
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
  swapchainCreateInfo.oldSwapchain   = nullptr;

  if (vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create the swapchain.");

  vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);

  swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapchainImages.data());

  swapchainImageFormat = surfaceFormat.format;
  swapchainExtent      = extent;
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

  const QueueFamilyIndices queueIndices = findQueueFamilies(device, surface);
  const bool areExtensionsSupported     = checkDeviceExtensionSupport(device);

  bool isSwapchainSupported = false;
  if (areExtensionsSupported) {
    const SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device, surface);
    isSwapchainSupported = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
  }

  return queueIndices.isComplete() && areExtensionsSupported && isSwapchainSupported;
}

/////////////
// Shaders //
/////////////

inline std::vector<char> readFile(const std::string& filePath) {
  std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);

  if (!file)
    throw std::runtime_error("Error: Couldn't open the file '" + filePath + "'.");

  const auto fileSize = static_cast<std::size_t>(file.tellg());
  file.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  file.read(bytes.data(), static_cast<std::streamsize>(fileSize));

  return bytes;
}

/////////////////
// Image views //
/////////////////

inline void createImageViews(std::vector<VkImageView>& swapchainImageViews,
                             const std::vector<VkImage>& swapchainImages,
                             VkFormat swapchainImageFormat,
                             VkDevice logicalDevice) {
  swapchainImageViews.resize(swapchainImages.size());

  for (std::size_t i = 0; i < swapchainImages.size(); ++i) {
    VkImageViewCreateInfo imageViewCreateInfo {};
    imageViewCreateInfo.sType        = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image        = swapchainImages[i];
    imageViewCreateInfo.viewType     = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format       = swapchainImageFormat;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
    imageViewCreateInfo.subresourceRange.levelCount     = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
      throw std::runtime_error("Error: Failed to create an image view.");
  }
}

/////////////////
// Render pass //
/////////////////

inline void createRenderPass(VkFormat swapchainImageFormat, VkDevice logicalDevice, VkRenderPass& renderPass) {
  VkAttachmentDescription colorAttachment {};
  colorAttachment.format         = swapchainImageFormat;
  colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass {};
  subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments    = &colorAttachmentRef;

  VkSubpassDependency dependency {};
  dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass    = 0;
  dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo {};
  renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments    = &colorAttachment;
  renderPassInfo.subpassCount    = 1;
  renderPassInfo.pSubpasses      = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies   = &dependency;

  if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create a render pass.");
}

//////////////
// Pipeline //
//////////////

inline void createGraphicsPipeline(VkDevice logicalDevice,
                                   VkExtent2D swapchainExtent,
                                   VkDescriptorSetLayout descriptorSetLayout,
                                   VkPipelineLayout& pipelineLayout,
                                   VkRenderPass renderPass,
                                   VkPipeline& graphicsPipeline) {
  const std::vector<char> vertShaderCode = readFile(RAZ_ROOT + "shaders/triangle_vk_vert.spv"s);
  const std::vector<char> fragShaderCode = readFile(RAZ_ROOT + "shaders/triangle_vk_frag.spv"s);

  VkShaderModule vertShaderModule {};
  Renderer::createShaderModule(vertShaderModule, vertShaderCode.size(), vertShaderCode.data(), logicalDevice);

  VkShaderModule fragShaderModule {};
  Renderer::createShaderModule(fragShaderModule, fragShaderCode.size(), fragShaderCode.data(), logicalDevice);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
  vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName  = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
  fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName  = "main";

  const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

  const VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
  const std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = Vertex::getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
  vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount   = 1;
  vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
  inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport {};
  viewport.x        = 0.f;
  viewport.y        = 0.f;
  viewport.width    = static_cast<float>(swapchainExtent.width);
  viewport.height   = static_cast<float>(swapchainExtent.height);
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;

  VkRect2D scissor {};
  scissor.offset = { 0, 0 };
  scissor.extent = swapchainExtent;

  VkPipelineViewportStateCreateInfo viewportState {};
  viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports    = &viewport;
  viewportState.scissorCount  = 1;
  viewportState.pScissors     = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer {};
  rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable        = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth               = 1.f;
  rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable         = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.f;
  rasterizer.depthBiasClamp          = 0.f;
  rasterizer.depthBiasSlopeFactor    = 0.f;

  VkPipelineMultisampleStateCreateInfo multisampling {};
  multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable   = VK_FALSE;
  multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading      = 1.f;
  multisampling.pSampleMask           = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable      = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment {};
  colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable         = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending {};
  colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable     = VK_FALSE;
  colorBlending.logicOp           = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount   = 1;
  colorBlending.pAttachments      = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.f;
  colorBlending.blendConstants[1] = 0.f;
  colorBlending.blendConstants[2] = 0.f;
  colorBlending.blendConstants[3] = 0.f;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
  pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount         = 1;
  pipelineLayoutInfo.pSetLayouts            = &descriptorSetLayout;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges    = nullptr;

  if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create the pipeline layout.");

  VkGraphicsPipelineCreateInfo pipelineInfo {};
  pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
  pipelineInfo.pStages             = shaderStages.data();
  pipelineInfo.pVertexInputState   = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState      = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState   = &multisampling;
  pipelineInfo.pDepthStencilState  = nullptr;
  pipelineInfo.pColorBlendState    = &colorBlending;
  pipelineInfo.pDynamicState       = nullptr;
  pipelineInfo.layout              = pipelineLayout;
  pipelineInfo.renderPass          = renderPass;
  pipelineInfo.subpass             = 0;
  pipelineInfo.basePipelineHandle  = nullptr;
  pipelineInfo.basePipelineIndex   = -1;

  if (vkCreateGraphicsPipelines(logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create the graphics pipeline.");

  vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
  vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
}

//////////////////
// Framebuffers //
//////////////////

inline void createFramebuffers(std::vector<VkFramebuffer>& swapchainFramebuffers,
                               const std::vector<VkImageView>& swapchainImageViews,
                               VkRenderPass renderPass,
                               VkExtent2D swapchainExtent,
                               VkDevice logicalDevice) {
  swapchainFramebuffers.resize(swapchainImageViews.size());

  for (std::size_t i = 0; i < swapchainImageViews.size(); ++i) {
    const std::array<VkImageView, 1> attachments = { swapchainImageViews[i] };

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments    = attachments.data();
    framebufferInfo.width           = swapchainExtent.width;
    framebufferInfo.height          = swapchainExtent.height;
    framebufferInfo.layers          = 1;

    if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
      throw std::runtime_error("Error: Failed to create a framebuffer.");
  }
}

/////////////
// Buffers //
/////////////

inline uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, MemoryProperty properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
    if ((typeFilter & (1u << i)) && ((memProperties.memoryTypes[i].propertyFlags & static_cast<uint32_t>(properties)) == static_cast<uint32_t>(properties)))
      return i;
  }

  throw std::runtime_error("Error: Failed to find a suitable memory type.");
}

/////////////////////
// Uniform buffers //
/////////////////////

inline void createUniformBuffers(std::vector<VkBuffer>& uniformBuffers,
                                 std::vector<VkDeviceMemory>& uniformBuffersMemory,
                                 std::size_t bufferCount,
                                 VkPhysicalDevice physicalDevice,
                                 VkDevice logicalDevice) {
  constexpr std::size_t bufferSize = sizeof(UniformMatrices);

  uniformBuffers.resize(bufferCount);
  uniformBuffersMemory.resize(bufferCount);

  for (std::size_t i = 0; i < bufferCount; ++i) {
    Renderer::createBuffer(uniformBuffers[i],
                           uniformBuffersMemory[i],
                           BufferUsage::UNIFORM_BUFFER,
                           MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
                           bufferSize,
                           physicalDevice,
                           logicalDevice);
  }
}

inline void createDescriptorSets(std::size_t descriptorCount,
                                 VkDescriptorSetLayout descriptorSetLayout,
                                 VkDescriptorPool descriptorPool,
                                 std::vector<VkDescriptorSet>& descriptorSets,
                                 VkDevice logicalDevice,
                                 const std::vector<VkBuffer>& uniformBuffers) {
  const std::vector<VkDescriptorSetLayout> layouts(descriptorCount, descriptorSetLayout);

  VkDescriptorSetAllocateInfo allocInfo {};
  allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool     = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorCount);
  allocInfo.pSetLayouts        = layouts.data();

  descriptorSets.resize(descriptorCount);

  if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to allocate descriptor sets.");

  for (std::size_t i = 0; i < descriptorCount; ++i) {
    VkDescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range  = sizeof(UniformMatrices);

    VkWriteDescriptorSet descriptorWrite {};
    descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet           = descriptorSets[i];
    descriptorWrite.dstBinding       = 0;
    descriptorWrite.dstArrayElement  = 0;
    descriptorWrite.descriptorCount  = 1;
    descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.pImageInfo       = nullptr;
    descriptorWrite.pBufferInfo      = &bufferInfo;
    descriptorWrite.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, nullptr);
  }
}

inline void updateUniformBuffer(VkExtent2D swapchainExtent,
                                uint32_t currentImageIndex,
                                VkDevice logicalDevice,
                                const std::vector<VkDeviceMemory>& uniformBuffersMemory) {
  static Transform transform {};
  static auto startTime = std::chrono::high_resolution_clock::now();

  const auto currentTime = std::chrono::high_resolution_clock::now();
  const float totalTime  = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  Camera camera(swapchainExtent.width, swapchainExtent.height, Degreesf(45.f), 0.1f, 100.f, ProjectionType::PERSPECTIVE);
  transform.setRotation(Quaternionf(Degreesf(90.f) * totalTime, Axis::Z).computeMatrix());

  UniformMatrices matrices {};
  matrices.model      = transform.computeTransformMatrix();
  matrices.view       = camera.computeLookAt(Vec3f(0.f, 2.f, 2.f));
  matrices.projection = camera.computePerspectiveMatrix();

  void* data {};
  vkMapMemory(logicalDevice, uniformBuffersMemory[currentImageIndex], 0, sizeof(UniformMatrices), 0, &data);
  std::memcpy(data, &matrices, sizeof(UniformMatrices));
  vkUnmapMemory(logicalDevice, uniformBuffersMemory[currentImageIndex]);
}

/////////////////////
// Command buffers //
/////////////////////

inline void createCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers,
                                 const std::vector<VkFramebuffer>& swapchainFramebuffers,
                                 VkCommandPool commandPool,
                                 VkDevice logicalDevice,
                                 VkRenderPass renderPass,
                                 VkExtent2D swapchainExtent,
                                 VkPipeline graphicsPipeline,
                                 VkBuffer vertexBuffer,
                                 VkBuffer indexBuffer,
                                 VkPipelineLayout pipelineLayout,
                                 const std::vector<VkDescriptorSet>& descriptorSets) {
  commandBuffers.resize(swapchainFramebuffers.size());

  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool        = commandPool;
  allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to allocate command buffers.");

  for (std::size_t i = 0; i < commandBuffers.size(); ++i) {
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
      throw std::runtime_error("Error: Failed to begin recording a command buffer.");

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderPass;
    renderPassInfo.framebuffer       = swapchainFramebuffers[i];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchainExtent;

    const VkClearValue clearColor  = { 0.15f, 0.15f, 0.15f, 1.f };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
      vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

      const std::array<VkBuffer, 1> vertexBuffers = { vertexBuffer };
      const std::array<VkDeviceSize, 1> offsets = { 0 };
      vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers.data(), offsets.data());
      vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
      vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

      vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    }
    vkCmdEndRenderPass(commandBuffers[i]);

    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
      throw std::runtime_error("Error: Failed to record a command buffer.");
  }
}

inline void destroySwapchain(VkDevice logicalDevice,
                             const std::vector<VkFramebuffer>& swapchainFramebuffers,
                             VkCommandPool commandPool,
                             const std::vector<VkCommandBuffer>& commandBuffers,
                             VkDescriptorPool descriptorPool,
                             const std::vector<VkBuffer>& uniformBuffers,
                             const std::vector<VkDeviceMemory>& uniformBuffersMemory,
                             VkPipeline graphicsPipeline,
                             VkPipelineLayout pipelineLayout,
                             VkRenderPass m_renderPass,
                             const std::vector<VkImageView>& swapchainImageViews,
                             VkSwapchainKHR swapchain) {
  for (VkFramebuffer framebuffer : swapchainFramebuffers)
    vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);

  vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

  vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);

  for (std::size_t i = 0; i < uniformBuffers.size(); ++i) {
    vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
    vkFreeMemory(logicalDevice, uniformBuffersMemory[i], nullptr);
  }

  vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
  vkDestroyRenderPass(logicalDevice, m_renderPass, nullptr);

  for (VkImageView imageView : swapchainImageViews)
    vkDestroyImageView(logicalDevice, imageView, nullptr);

  vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
}

} // namespace

void Renderer::initialize(GLFWwindow* windowHandle) {
  // If already initialized, do nothing
  if (s_isInitialized)
    return;

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
    m_windowHandle = windowHandle;

    if (glfwCreateWindowSurface(m_instance, m_windowHandle, nullptr, &m_surface) != VK_SUCCESS)
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

  const QueueFamilyIndices queueIndices = findQueueFamilies(m_physicalDevice, m_surface);

  // Recovering the necessary queues
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  const std::set<uint32_t> uniqueQueueFamilies = { queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value() };
  const float queuePriority = 1.f;

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
  vkGetDeviceQueue(m_logicalDevice, queueIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_logicalDevice, queueIndices.presentFamily.value(), 0, &m_presentQueue);

  ///////////////
  // Swapchain //
  ///////////////

  createSwapchain(m_physicalDevice, m_surface, m_windowHandle, m_logicalDevice, m_swapchain, m_swapchainImages, m_swapchainImageFormat, m_swapchainExtent);

  /////////////////
  // Image views //
  /////////////////

  createImageViews(m_swapchainImageViews, m_swapchainImages, m_swapchainImageFormat, m_logicalDevice);

  /////////////////
  // Render pass //
  /////////////////

  createRenderPass(m_swapchainImageFormat, m_logicalDevice, m_renderPass);

  ////////////////////
  // Descriptor set //
  ////////////////////

  Renderer::createDescriptorSetLayout(m_descriptorSetLayout, DescriptorType::UNIFORM_BUFFER, ShaderStage::VERTEX, m_logicalDevice);

  //////////////
  // Pipeline //
  //////////////

  createGraphicsPipeline(m_logicalDevice, m_swapchainExtent, m_descriptorSetLayout, m_pipelineLayout, m_renderPass, m_graphicsPipeline);

  //////////////////
  // Framebuffers //
  //////////////////

  createFramebuffers(m_swapchainFramebuffers, m_swapchainImageViews, m_renderPass, m_swapchainExtent, m_logicalDevice);

  //////////////////
  // Command pool //
  //////////////////

  Renderer::createCommandPool(m_commandPool, CommandPoolOption::TRANSIENT, queueIndices.graphicsFamily.value(), m_logicalDevice);

  ///////////////////
  // Vertex buffer //
  ///////////////////

  Renderer::createStagedBuffer(m_vertexBuffer,
                               m_vertexBufferMemory,
                               BufferUsage::VERTEX_BUFFER,
                               vertices.data(),
                               sizeof(vertices.front()) * vertices.size(),
                               m_physicalDevice,
                               m_logicalDevice,
                               m_graphicsQueue,
                               m_commandPool);

  //////////////////
  // Index buffer //
  //////////////////

  Renderer::createStagedBuffer(m_indexBuffer,
                               m_indexBufferMemory,
                               BufferUsage::INDEX_BUFFER,
                               indices.data(),
                               sizeof(indices.front()) * indices.size(),
                               m_physicalDevice,
                               m_logicalDevice,
                               m_graphicsQueue,
                               m_commandPool);

  /////////////////////
  // Uniform buffers //
  /////////////////////

  createUniformBuffers(m_uniformBuffers, m_uniformBuffersMemory, m_swapchainImages.size(), m_physicalDevice, m_logicalDevice);
  Renderer::createDescriptorPool(m_descriptorPool, DescriptorType::UNIFORM_BUFFER, static_cast<uint32_t>(m_swapchainImages.size()), m_logicalDevice);
  createDescriptorSets(m_swapchainImages.size(), m_descriptorSetLayout, m_descriptorPool, m_descriptorSets, m_logicalDevice, m_uniformBuffers);

  /////////////////////
  // Command buffers //
  /////////////////////

  createCommandBuffers(m_commandBuffers,
                       m_swapchainFramebuffers,
                       m_commandPool,
                       m_logicalDevice,
                       m_renderPass,
                       m_swapchainExtent,
                       m_graphicsPipeline,
                       m_vertexBuffer,
                       m_indexBuffer,
                       m_pipelineLayout,
                       m_descriptorSets);

  ////////////////
  // Semaphores //
  ////////////////

  m_imagesInFlight.resize(m_swapchainImages.size(), nullptr);

  VkSemaphoreCreateInfo semaphoreInfo {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (std::size_t i = 0; i < MaxFramesInFlight; ++i) {
    if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS
      || vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS
      || vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
      throw std::runtime_error("Error: Failed to create the synchronization objects.");
  }

  s_isInitialized = true;
}

void Renderer::createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout,
                                         DescriptorType descriptorType,
                                         ShaderStage shaderStageFlags,
                                         VkDevice logicalDevice) {
  VkDescriptorSetLayoutBinding layoutBinding {};
  layoutBinding.binding            = 0;
  layoutBinding.descriptorType     = static_cast<VkDescriptorType>(descriptorType);
  layoutBinding.descriptorCount    = 1;
  layoutBinding.stageFlags         = static_cast<VkShaderStageFlags>(shaderStageFlags);
  layoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo {};
  layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings    = &layoutBinding;

  if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create a descriptor set layout.");
}

void Renderer::createShaderModule(VkShaderModule& shaderModule, std::size_t shaderCodeSize, const char* shaderCodeStr, VkDevice logicalDevice) {
  VkShaderModuleCreateInfo createInfo {};
  createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shaderCodeSize;
  createInfo.pCode    = reinterpret_cast<const uint32_t*>(shaderCodeStr);

  if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create a shader module.");
}

void Renderer::createCommandPool(VkCommandPool& commandPool, CommandPoolOption options, uint32_t queueFamilyIndex, VkDevice logicalDevice) {
  VkCommandPoolCreateInfo commandPoolInfo {};
  commandPoolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolInfo.flags            = static_cast<VkCommandPoolCreateFlags>(options);
  commandPoolInfo.queueFamilyIndex = queueFamilyIndex;

  if (vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create a command pool.");
}

void Renderer::createBuffer(VkBuffer& buffer,
                            VkDeviceMemory& bufferMemory,
                            BufferUsage usageFlags,
                            MemoryProperty propertyFlags,
                            std::size_t bufferSize,
                            VkPhysicalDevice physicalDevice,
                            VkDevice logicalDevice) {
  VkBufferCreateInfo bufferInfo {};
  bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size        = bufferSize;
  bufferInfo.usage       = static_cast<uint32_t>(usageFlags);
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create a buffer.");

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo {};
  allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize  = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, propertyFlags);

  if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to allocate a buffer's memory.");

  vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

void Renderer::createStagedBuffer(VkBuffer& buffer,
                                  VkDeviceMemory& bufferMemory,
                                  BufferUsage bufferType,
                                  const void* bufferData,
                                  std::size_t bufferSize,
                                  VkPhysicalDevice physicalDevice,
                                  VkDevice logicalDevice,
                                  VkQueue queue,
                                  VkCommandPool commandPool) {
  VkBuffer stagingBuffer {};
  VkDeviceMemory stagingBufferMemory {};

  Renderer::createBuffer(stagingBuffer,
                         stagingBufferMemory,
                         BufferUsage::TRANSFER_SRC,
                         MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
                         bufferSize,
                         physicalDevice,
                         logicalDevice);

  void* data {};
  vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
  std::memcpy(data, bufferData, bufferSize);
  vkUnmapMemory(logicalDevice, stagingBufferMemory);

  Renderer::createBuffer(buffer,
                         bufferMemory,
                         BufferUsage::TRANSFER_DST | bufferType,
                         MemoryProperty::DEVICE_LOCAL,
                         bufferSize,
                         physicalDevice,
                         logicalDevice);

  Renderer::copyBuffer(stagingBuffer, buffer, bufferSize, logicalDevice, queue, commandPool);

  vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
  vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void Renderer::copyBuffer(VkBuffer srcBuffer,
                          VkBuffer dstBuffer,
                          VkDeviceSize bufferSize,
                          VkDevice logicalDevice,
                          VkQueue queue,
                          VkCommandPool commandPool) {
  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool        = commandPool;
  allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer {};
  vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copyRegion {};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size      = bufferSize;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo {};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &commandBuffer;

  vkQueueSubmit(queue, 1, &submitInfo, nullptr);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

void Renderer::createDescriptorPool(VkDescriptorPool& descriptorPool, DescriptorType descriptorType, uint32_t descriptorCount, VkDevice logicalDevice) {
  VkDescriptorPoolSize poolSize {};
  poolSize.type            = static_cast<VkDescriptorType>(descriptorType);
  poolSize.descriptorCount = descriptorCount;

  VkDescriptorPoolCreateInfo poolInfo {};
  poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.maxSets       = descriptorCount;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes    = &poolSize;

  if (vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to create a descriptor pool.");
}

void Renderer::recreateSwapchain() {
  // If the window is minimized, pausing the application to wait for the window to be on screen again
  int width {};
  int height {};
  glfwGetFramebufferSize(m_windowHandle, &width, &height);

  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(m_windowHandle, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(m_logicalDevice);

  createSwapchain(m_physicalDevice, m_surface, m_windowHandle, m_logicalDevice, m_swapchain, m_swapchainImages, m_swapchainImageFormat, m_swapchainExtent);
  createImageViews(m_swapchainImageViews, m_swapchainImages, m_swapchainImageFormat, m_logicalDevice);
  createRenderPass(m_swapchainImageFormat, m_logicalDevice, m_renderPass);
  createGraphicsPipeline(m_logicalDevice, m_swapchainExtent, m_descriptorSetLayout, m_pipelineLayout, m_renderPass, m_graphicsPipeline);
  createFramebuffers(m_swapchainFramebuffers, m_swapchainImageViews, m_renderPass, m_swapchainExtent, m_logicalDevice);
  createUniformBuffers(m_uniformBuffers, m_uniformBuffersMemory, m_swapchainImages.size(), m_physicalDevice, m_logicalDevice);
  Renderer::createDescriptorPool(m_descriptorPool, DescriptorType::UNIFORM_BUFFER, static_cast<uint32_t>(m_swapchainImages.size()), m_logicalDevice);
  createDescriptorSets(m_swapchainImages.size(), m_descriptorSetLayout, m_descriptorPool, m_descriptorSets, m_logicalDevice, m_uniformBuffers);
  createCommandBuffers(m_commandBuffers,
                       m_swapchainFramebuffers,
                       m_commandPool,
                       m_logicalDevice,
                       m_renderPass,
                       m_swapchainExtent,
                       m_graphicsPipeline,
                       m_vertexBuffer,
                       m_indexBuffer,
                       m_pipelineLayout,
                       m_descriptorSets);
}

void Renderer::drawFrame() {
  vkWaitForFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());

  uint32_t imageIndex {};
  const VkResult imageResult = vkAcquireNextImageKHR(m_logicalDevice,
                                                     m_swapchain,
                                                     std::numeric_limits<uint64_t>::max(),
                                                     m_imageAvailableSemaphores[m_currentFrameIndex],
                                                     nullptr,
                                                     &imageIndex);

  if (imageResult == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapchain();
    return;
  } else if (imageResult != VK_SUCCESS && imageResult != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Error: Failed to acquire a swapchain image.");
  }

  updateUniformBuffer(m_swapchainExtent, imageIndex, m_logicalDevice, m_uniformBuffersMemory);

  if (m_imagesInFlight[imageIndex] != nullptr)
    vkWaitForFences(m_logicalDevice, 1, &m_imagesInFlight[imageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());

  m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrameIndex];

  const std::array<VkSemaphore, 1> waitSemaphores      = { m_imageAvailableSemaphores[m_currentFrameIndex] };
  const std::array<VkPipelineStageFlags, 1> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  const std::array<VkSemaphore, 1> signalSemaphores    = { m_renderFinishedSemaphores[m_currentFrameIndex] };

  VkSubmitInfo submitInfo {};
  submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount   = static_cast<uint32_t>(waitSemaphores.size());
  submitInfo.pWaitSemaphores      = waitSemaphores.data();
  submitInfo.pWaitDstStageMask    = waitStages.data();
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &m_commandBuffers[imageIndex];
  submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
  submitInfo.pSignalSemaphores    = signalSemaphores.data();

  vkResetFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrameIndex]);

  if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrameIndex]) != VK_SUCCESS)
    throw std::runtime_error("Error: Failed to submit the draw command buffer.");

  VkPresentInfoKHR presentInfo {};
  presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
  presentInfo.pWaitSemaphores    = signalSemaphores.data();

  const std::array<VkSwapchainKHR, 1> swapchains = { m_swapchain };
  presentInfo.swapchainCount = static_cast<uint32_t>(swapchains.size());
  presentInfo.pSwapchains    = swapchains.data();
  presentInfo.pImageIndices  = &imageIndex;
  presentInfo.pResults       = nullptr;

  const VkResult presentResult = vkQueuePresentKHR(m_presentQueue, &presentInfo);

  if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
    m_framebufferResized = false;
    recreateSwapchain();
  } else if (presentResult != VK_SUCCESS) {
    throw std::runtime_error("Error: Failed to present a swapchain image.");
  }

  m_currentFrameIndex = (m_currentFrameIndex + 1) % MaxFramesInFlight;
}

void Renderer::destroy() {
  vkDeviceWaitIdle(m_logicalDevice);

#if !defined(NDEBUG)
  destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif

  destroySwapchain(m_logicalDevice,
                   m_swapchainFramebuffers,
                   m_commandPool,
                   m_commandBuffers,
                   m_descriptorPool,
                   m_uniformBuffers,
                   m_uniformBuffersMemory,
                   m_graphicsPipeline,
                   m_pipelineLayout,
                   m_renderPass,
                   m_swapchainImageViews,
                   m_swapchain);

  vkDestroyDescriptorSetLayout(m_logicalDevice, m_descriptorSetLayout, nullptr);

  for (std::size_t i = 0; i < MaxFramesInFlight; ++i) {
    vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(m_logicalDevice, m_inFlightFences[i], nullptr);
  }

  vkDestroyBuffer(m_logicalDevice, m_indexBuffer, nullptr);
  vkFreeMemory(m_logicalDevice, m_indexBufferMemory, nullptr);

  vkDestroyBuffer(m_logicalDevice, m_vertexBuffer, nullptr);
  vkFreeMemory(m_logicalDevice, m_vertexBufferMemory, nullptr);

  vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);

  vkDestroyDevice(m_logicalDevice, nullptr);
  vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  vkDestroyInstance(m_instance, nullptr);

  m_windowHandle = nullptr;
  s_isInitialized = false;
}

} // namespace Raz
