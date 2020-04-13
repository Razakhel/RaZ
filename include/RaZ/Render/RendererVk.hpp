#pragma once

#ifndef RAZ_RENDERERVK_HPP
#define RAZ_RENDERERVK_HPP

#include "RaZ/Utils/EnumUtils.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace Raz {

enum class CullingMode : uint32_t {
  NONE       = 0 /* VK_CULL_MODE_NONE           */, ///< No triangle culling (shows everything).
  FRONT      = 1 /* VK_CULL_MODE_FRONT_BIT      */, ///< Cull front-facing triangles.
  BACK       = 2 /* VK_CULL_MODE_BACK_BIT       */, ///< Cull back-facing triangles.
  FRONT_BACK = 3 /* VK_CULL_MODE_FRONT_AND_BACK */  ///< Cull both front- & back-facing triangles (discards everything).
};

enum class CommandPoolOption : uint32_t {
  TRANSIENT            = 1 /* VK_COMMAND_POOL_CREATE_TRANSIENT_BIT            */, ///<
  RESET_COMMAND_BUFFER = 2 /* VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT */, ///<
  PROTECTED            = 4 /* VK_COMMAND_POOL_CREATE_PROTECTED_BIT            */  ///<
};
MAKE_ENUM_FLAG(CommandPoolOption)

enum class BufferUsage : uint32_t {
  TRANSFER_SRC               = 1      /* VK_BUFFER_USAGE_TRANSFER_SRC_BIT                          */, ///< Memory transfer source buffer.
  TRANSFER_DST               = 2      /* VK_BUFFER_USAGE_TRANSFER_DST_BIT                          */, ///< Memory transfer destination buffer.
  UNIFORM_TEXEL_BUFFER       = 4      /* VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT                  */, ///< Uniform texel buffer.
  STORAGE_TEXEL_BUFFER       = 8      /* VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT                  */, ///< Storage texel buffer.
  UNIFORM_BUFFER             = 16     /* VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT                        */, ///< Uniform buffer.
  STORAGE_BUFFER             = 32     /* VK_BUFFER_USAGE_STORAGE_BUFFER_BIT                        */, ///< Storage buffer.
  INDEX_BUFFER               = 64     /* VK_BUFFER_USAGE_INDEX_BUFFER_BIT                          */, ///< Index buffer.
  VERTEX_BUFFER              = 128    /* VK_BUFFER_USAGE_VERTEX_BUFFER_BIT                         */, ///< Vertex buffer.
  INDIRECT_BUFFER            = 256    /* VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT                       */, ///< Indirect buffer.
  SHADER_DEVICE_ADDRESS      = 131072 /* VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT                 */, ///<
  TRANSFORM_FEEDBACK         = 2048   /* VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT         */, ///<
  TRANSFORM_FEEDBACK_COUNTER = 4096   /* VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT */, ///<
  CONDITIONAL_RENDERING      = 512    /* VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT             */, ///<
  RAY_TRACING                = 1024   /* VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR                       */  ///<
};
MAKE_ENUM_FLAG(BufferUsage)

enum class MemoryProperty : uint32_t {
  DEVICE_LOCAL        = 1   /* VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT        */, ///<
  HOST_VISIBLE        = 2   /* VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT        */, ///<
  HOST_COHERENT       = 4   /* VK_MEMORY_PROPERTY_HOST_COHERENT_BIT       */, ///<
  HOST_CACHED         = 8   /* VK_MEMORY_PROPERTY_HOST_CACHED_BIT         */, ///<
  LAZILY_ALLOCATED    = 16  /* VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT    */, ///<
  PROTECTED           = 32  /* VK_MEMORY_PROPERTY_PROTECTED_BIT           */, ///<
  DEVICE_COHERENT_AMD = 64  /* VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD */, ///<
  DEVICE_UNCACHED_AMD = 128 /* VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD */  ///<
};
MAKE_ENUM_FLAG(MemoryProperty)

// TODO: temporary GL enums to allow compilation

enum class Capability : unsigned int {
  CULL       = 2884, // GL_CULL_FACE
  DEPTH_TEST = 2929  // GL_DEPTH_TEST
};

enum class MaskType : unsigned int {
  COLOR = 16384, // GL_COLOR_BUFFER_BIT
  DEPTH = 256    // GL_DEPTH_BUFFER_BIT
};
MAKE_ENUM_FLAG(MaskType)

enum class DepthFunction : unsigned int {
  EQUAL      = 514, // GL_EQUAL
  LESS       = 513, // GL_LESS
  LESS_EQUAL = 515  // GL_LEQUAL
};

enum class PolygonMode : uint32_t {
  POINT = 2, // VK_POLYGON_MODE_POINT
  LINE  = 1, // VK_POLYGON_MODE_LINE
  FILL  = 0  // VK_POLYGON_MODE_FILL
};

enum class BufferType : unsigned int {
  ARRAY_BUFFER   = 34962, // GL_ARRAY_BUFFER
  ELEMENT_BUFFER = 34963, // GL_ELEMENT_ARRAY_BUFFER
  UNIFORM_BUFFER = 35345  // GL_UNIFORM_BUFFER
};

enum class BufferDataUsage : unsigned int {
  STATIC_DRAW = 35044 // GL_STATIC_DRAW
};

enum class ShaderType : unsigned int {
  VERTEX   = 35633, // GL_VERTEX_SHADER
  FRAGMENT = 35632, // GL_FRAGMENT_SHADER
  GEOMETRY = 36313  // GL_GEOMETRY_SHADER
};

enum class TextureType : unsigned int {
  TEXTURE_2D    = 3553,  // GL_TEXTURE_2D
  CUBEMAP       = 34067, // GL_TEXTURE_CUBE_MAP
  CUBEMAP_POS_X = 34069, // GL_TEXTURE_CUBE_MAP_POSITIVE_X
  CUBEMAP_NEG_X = 34070, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
  CUBEMAP_POS_Y = 34071, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
  CUBEMAP_NEG_Y = 34072, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
  CUBEMAP_POS_Z = 34073, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
  CUBEMAP_NEG_Z = 34074  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

enum class TextureParam : unsigned int {
  MINIFY_FILTER  = 10241, // GL_TEXTURE_MIN_FILTER
  MAGNIFY_FILTER = 10240, // GL_TEXTURE_MAG_FILTER
  WRAP_S         = 10242, // GL_TEXTURE_WRAP_S
  WRAP_T         = 10243, // GL_TEXTURE_WRAP_T
  WRAP_R         = 32882, // GL_TEXTURE_WRAP_R
  SWIZZLE_RGBA   = 36422  // GL_TEXTURE_SWIZZLE_RGBA
};

enum class TextureParamValue : unsigned int {
  NEAREST              = 9728,  // GL_NEAREST
  LINEAR               = 9729,  // GL_LINEAR
  LINEAR_MIPMAP_LINEAR = 9987,  // GL_LINEAR_MIPMAP_LINEAR
  REPEAT               = 10497, // GL_REPEAT
  CLAMP_TO_EDGE        = 33071  // GL_CLAMP_TO_EDGE
};

enum class TextureFormat : unsigned int {
  RED           = 6403,  // GL_RED
  RG            = 33319, // GL_RG
  RGB           = 6407,  // GL_RGB
  BGR           = 32992, // GL_BGR
  RGBA          = 6408,  // GL_RGBA
  BGRA          = 32993, // GL_BGRA
  SRGB          = 35904, // GL_SRGB
  DEPTH         = 6402,  // GL_DEPTH_COMPONENT
  DEPTH_STENCIL = 34041  // GL_DEPTH_STENCIL
};

enum class TextureInternalFormat : unsigned int {
  // Base formats
  RGB   = static_cast<unsigned int>(TextureFormat::RGB),
  RGBA  = static_cast<unsigned int>(TextureFormat::RGBA),
  SRGB  = static_cast<unsigned int>(TextureFormat::SRGB),
  DEPTH = static_cast<unsigned int>(TextureFormat::DEPTH),

  // Sized formats
  RED16F   = 33325, // GL_R16F
  RG16F    = 33327, // GL_RG16F
  RGB16F   = 34843, // GL_RGB16F
  RGBA16F  = 34842, // GL_RGBA16F
  DEPTH32F = 36012  // GL_DEPTH_COMPONENT32F
};

enum class TextureDataType : unsigned int {
  UBYTE = 5121, // GL_UNSIGNED_BYTE
  FLOAT = 5126  // GL_FLOAT
};

enum class FramebufferAttachment : unsigned int {
  DEPTH  = 36096, // GL_DEPTH_ATTACHMENT
  COLOR0 = 36064  // GL_COLOR_ATTACHMENT0
};

enum class FramebufferType : unsigned int {
  FRAMEBUFFER = 36160 // GL_FRAMEBUFFER
};

enum class DrawBuffer : unsigned int {
  COLOR_ATTACHMENT0 = static_cast<unsigned int>(FramebufferAttachment::COLOR0)
};

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void initialize(GLFWwindow* windowHandle = nullptr);
  static bool isInitialized() { return s_isInitialized; }

  static void createCommandPool(VkCommandPool& commandPool, CommandPoolOption options, uint32_t queueFamilyIndex, VkDevice logicalDevice);
  static void createBuffer(VkBuffer& buffer,
                           VkDeviceMemory& bufferMemory,
                           BufferUsage usageFlags,
                           MemoryProperty propertyFlags,
                           VkPhysicalDevice physicalDevice,
                           VkDevice logicalDevice,
                           std::size_t bufferSize);
  static void copyBuffer(VkBuffer srcBuffer,
                         VkBuffer dstBuffer,
                         VkDeviceSize bufferSize,
                         VkQueue queue,
                         VkDevice logicalDevice,
                         VkCommandPool commandPool);

  static void recreateSwapchain();
  static void drawFrame();
  /// Destroys the renderer, deallocating Vulkan resources.
  static void destroy();

  // TODO: temporary GL functions to allow compilation

  static void enable(Capability) {}
  static void disable(Capability) {}
  static unsigned int getCurrentProgram() { return 0; }
  static void clearColor(float, float, float, float) {}
  static void clear(MaskType) {}
  static void clear(MaskType, MaskType) {}
  static void setDepthFunction(DepthFunction) {}
  static void setFaceCulling(CullingMode) {}
  static void setPolygonMode(CullingMode, PolygonMode) {}
  static void recoverFrame(unsigned int, unsigned int, TextureFormat, TextureDataType, void*) {}
  static void generateBuffer(unsigned int&) {}
  static void bindBuffer(BufferType, unsigned int) {}
  static void unbindBuffer(BufferType) {}
  static void bindBufferBase(BufferType, unsigned int, unsigned int) {}
  static void bindBufferRange(BufferType, unsigned int, unsigned int, std::ptrdiff_t, std::ptrdiff_t) {}
  static void sendBufferData(BufferType, std::ptrdiff_t, const void*, BufferDataUsage) {}
  static void sendBufferSubData(BufferType, std::ptrdiff_t, std::ptrdiff_t, const void*) {}
  static void deleteBuffer(unsigned int&) {}
  static bool isTexture(unsigned int) { return true; }
  static void sendImageData2D(TextureType, unsigned int, TextureInternalFormat, unsigned int, unsigned int, TextureFormat, TextureDataType, const void*) {}
  static void generateMipmap(TextureType) {}
  static void generateTexture(unsigned int&) {}
  static void bindTexture(TextureType, unsigned int) {}
  static void unbindTexture(TextureType) {}
  static void activateTexture(unsigned int) {}
  static void setTextureParameter(TextureType, TextureParam, const int*) {}
  static void setTextureParameter(TextureType, TextureParam, TextureParamValue) {}
  static void deleteTexture(unsigned int&) {}
  static void resizeViewport(int, int, unsigned int, unsigned int) {}
  static unsigned int createProgram() { return 0; }
  static bool isProgramLinked(unsigned int) { return true; }
  static void linkProgram(unsigned int) {}
  static void useProgram(unsigned int) {}
  static void deleteProgram(unsigned int) {}
  static unsigned int createShader(ShaderType) { return 0; }
  static bool isShaderCompiled(unsigned int) { return true; }
  static void sendShaderSource(unsigned int, const char*, int) {}
  static void sendShaderSource(unsigned int, const std::string&) {}
  static void compileShader(unsigned int) {}
  static void attachShader(unsigned int, unsigned int) {}
  static void detachShader(unsigned int, unsigned int) {}
  static bool isShaderAttached(unsigned int, unsigned int) { return true; }
  static void deleteShader(unsigned int) {}
  static int recoverUniformLocation(unsigned int, const char*) { return 0; }
  static void sendUniform(int, int) {}
  static void sendUniform(int, unsigned int) {}
  static void sendUniform(int, float) {}
  static void sendUniformVector2(int, const float*, int = 1) {}
  static void sendUniformVector3(int, const float*, int = 1) {}
  static void sendUniformVector4(int, const float*, int = 1) {}
  static void sendUniformMatrix2x2(int, const float*, int = 1, bool = false) {}
  static void sendUniformMatrix3x3(int, const float*, int = 1, bool = false) {}
  static void sendUniformMatrix4x4(int, const float*, int = 1, bool = false) {}
  static void generateFramebuffer(unsigned int&) {}
  static bool isFramebufferComplete(FramebufferType = FramebufferType::FRAMEBUFFER) { return true; }
  static void bindFramebuffer(unsigned int, FramebufferType = FramebufferType::FRAMEBUFFER) {}
  static void unbindFramebuffer(FramebufferType = FramebufferType::FRAMEBUFFER) {}
  static void setFramebufferTexture2D(FramebufferAttachment, TextureType, unsigned int, int, FramebufferType = FramebufferType::FRAMEBUFFER) {}
  static void setDrawBuffers(unsigned int, const DrawBuffer*) {}
  static void deleteFramebuffer(unsigned int&) {}
  static void printErrors() {}

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) noexcept = delete;

  ~Renderer() = delete;

private:
  static constexpr int MaxFramesInFlight = 2;

  static inline bool s_isInitialized = false;

  static inline VkInstance m_instance {};
  static inline VkSurfaceKHR m_surface {};
  static inline GLFWwindow* m_windowHandle {};
  static inline VkPhysicalDevice m_physicalDevice {};
  static inline VkDevice m_logicalDevice {};
  static inline VkQueue m_graphicsQueue {};
  static inline VkQueue m_presentQueue {};

  static inline VkSwapchainKHR m_swapchain {};
  static inline std::vector<VkImage> m_swapchainImages {};
  static inline VkFormat m_swapchainImageFormat {};
  static inline VkExtent2D m_swapchainExtent {};
  static inline std::vector<VkImageView> m_swapchainImageViews {};

  static inline VkRenderPass m_renderPass {};
  static inline VkDescriptorSetLayout m_descriptorSetLayout {};
  static inline VkPipelineLayout m_pipelineLayout {};
  static inline VkPipeline m_graphicsPipeline {};
  static inline std::vector<VkFramebuffer> m_swapchainFramebuffers {};
  static inline VkCommandPool m_commandPool {};
  static inline VkBuffer m_vertexBuffer {};
  static inline VkDeviceMemory m_vertexBufferMemory {};
  static inline VkBuffer m_indexBuffer {};
  static inline VkDeviceMemory m_indexBufferMemory {};
  static inline std::vector<VkBuffer> m_uniformBuffers {};
  static inline std::vector<VkDeviceMemory> m_uniformBuffersMemory {};
  static inline VkDescriptorPool m_descriptorPool {};
  static inline std::vector<VkDescriptorSet> m_descriptorSets {};
  static inline std::vector<VkCommandBuffer> m_commandBuffers {};

  static inline std::array<VkSemaphore, MaxFramesInFlight> m_imageAvailableSemaphores {};
  static inline std::array<VkSemaphore, MaxFramesInFlight> m_renderFinishedSemaphores {};
  static inline std::array<VkFence, MaxFramesInFlight> m_inFlightFences {};
  static inline std::vector<VkFence> m_imagesInFlight {};
  static inline std::size_t m_currentFrameIndex = 0;

  static inline bool m_framebufferResized = false;

#if !defined(NDEBUG)
  static inline VkDebugUtilsMessengerEXT m_debugMessenger {};
#endif
};

} // namespace Raz

#endif // RAZ_RENDERERVK_HPP
