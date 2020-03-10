#pragma once

#ifndef RAZ_RENDERERVK_HPP
#define RAZ_RENDERERVK_HPP

#include "RaZ/Utils/EnumUtils.hpp"

#include <vulkan/vulkan.h>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace Raz {

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

enum class FaceOrientation : unsigned int {
  FRONT      = 1028, // GL_FRONT
  BACK       = 1029, // GL_BACK
  FRONT_BACK = 1032  // GL_FRONT_AND_BACK
};

enum class PolygonMode : unsigned int {
  LINE  = 6913, // GL_LINE
  FILL  = 6914  // GL_FILL
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

  // TODO: temporary GL functions to allow compilation

  static void enable(Capability) {}
  static void disable(Capability) {}
  static unsigned int getCurrentProgram() { return 0; }
  static void clearColor(float, float, float, float) {}
  static void clear(MaskType) {}
  static void clear(MaskType, MaskType) {}
  static void setDepthFunction(DepthFunction) {}
  static void setFaceCulling(FaceOrientation) {}
  static void setPolygonMode(FaceOrientation, PolygonMode) {}
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
  /// Destroys the renderer, deallocating Vulkan resources.
  static void destroy();

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) noexcept = delete;

  ~Renderer() = delete;

private:
  static inline bool s_isInitialized = false;

  static inline VkInstance m_instance {};
  static inline VkSurfaceKHR m_surface {};
  static inline VkPhysicalDevice m_physicalDevice {};
  static inline VkDevice m_logicalDevice {};
  static inline VkQueue m_graphicsQueue {};
  static inline VkQueue m_presentQueue {};
  static inline VkSwapchainKHR m_swapchain {};
  static inline std::vector<VkImage> m_swapchainImages {};
  static inline VkFormat m_swapchainImageFormat {};
  static inline VkExtent2D m_swapchainExtent {};
  static inline std::vector<VkImageView> m_swapchainImageViews {};
#if !defined(NDEBUG)
  static inline VkDebugUtilsMessengerEXT m_debugMessenger {};
#endif
};

} // namespace Raz

#endif // RAZ_RENDERERVK_HPP
