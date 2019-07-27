#pragma once

#ifndef RAZ_RENDERER_HPP
#define RAZ_RENDERER_HPP

namespace Raz {

enum class MaskType : unsigned int {
  COLOR   = 16384, // GL_COLOR_BUFFER_BIT
  DEPTH   = 256,   // GL_DEPTH_BUFFER_BIT
  STENCIL = 1024   // GL_STENCIL_BUFFER_BIT
};

enum class BufferType : unsigned int {
  ARRAY_BUFFER   = 34962, // GL_ARRAY_BUFFER
  ELEMENT_BUFFER = 34963, // GL_ELEMENT_ARRAY_BUFFER
  UNIFORM_BUFFER = 35345  // GL_UNIFORM_BUFFER
};

enum class TextureType : unsigned int {
  TEXTURE_2D = 3553, // GL_TEXTURE_2D
  CUBEMAP    = 34067 // GL_TEXTURE_CUBE_MAP
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
  RED           = static_cast<unsigned int>(TextureFormat::RED),
  RG            = static_cast<unsigned int>(TextureFormat::RG),
  RGB           = static_cast<unsigned int>(TextureFormat::RGB),
  BGR           = static_cast<unsigned int>(TextureFormat::BGR),
  RGBA          = static_cast<unsigned int>(TextureFormat::RGBA),
  BGRA          = static_cast<unsigned int>(TextureFormat::BGRA),
  SRGB          = static_cast<unsigned int>(TextureFormat::SRGB),
  DEPTH         = static_cast<unsigned int>(TextureFormat::DEPTH),
  DEPTH_STENCIL = static_cast<unsigned int>(TextureFormat::DEPTH_STENCIL),

  // Sized formats
  RED16F   = 33325, // GL_R16F
  RG16F    = 33327, // GL_RG16F
  RGB16F   = 34843, // GL_RGB16F
  RGBA16F  = 34842, // GL_RGBA16F
  DEPTH32F = 36012, // GL_DEPTH_COMPONENT32F
};

enum class TextureDataType : unsigned int {
  UBYTE = 5121, // GL_UNSIGNED_BYTE
  FLOAT = 5126  // GL_FLOAT
};

enum class ShaderType : unsigned int {
  VERTEX   = 35633, // GL_VERTEX_SHADER
  FRAGMENT = 35632, // GL_FRAGMENT_SHADER
  GEOMETRY = 36313  // GL_GEOMETRY_SHADER
};

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void initialize();
  static bool isInitialized() { return s_isInitialized; }
  static void enable(unsigned int code);
  static void disable(unsigned int code);
  static void clear(MaskType type) { clear(static_cast<unsigned int>(type)); }
  static void clear(MaskType type1, MaskType type2) { clear(static_cast<unsigned int>(type1) | static_cast<unsigned int>(type2)); }
  static void clear(MaskType type1, MaskType type2, MaskType type3) { clear(static_cast<unsigned int>(type1)
                                                                          | static_cast<unsigned int>(type2)
                                                                          | static_cast<unsigned int>(type3)); }
  static void generateBuffers(unsigned int count, unsigned int* indices);
  static void bindBuffer(BufferType type, unsigned int index);
  static void unbindBuffer(BufferType type) { bindBuffer(type, 0); }
  static void bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex);
  static void sendBufferSubData(BufferType type, ptrdiff_t offset, ptrdiff_t dataSize, const void* data);
  template <typename T> static void sendBufferSubData(BufferType type, ptrdiff_t offset, const T& data) { sendBufferSubData(type, offset, sizeof(T), &data); }
  static void deleteBuffers(unsigned int count, unsigned int* indices);
  static void generateTextures(unsigned int count, unsigned int* indices);
  static void activateTexture(unsigned int index);
  static void sendImageData2D(TextureType type,
                              int mipmapLevel,
                              TextureInternalFormat internalFormat,
                              int width, int height,
                              TextureFormat format,
                              TextureDataType dataType, const void* data);
  static void bindTexture(TextureType type, unsigned int index);
  static void unbindTexture(TextureType type) { bindTexture(type, 0); }
  static void deleteTextures(unsigned int count, unsigned int* indices);
  static void resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height);
  static unsigned int createProgram();
  static int getProgramInfo(unsigned int index, unsigned int infoType);
  static bool isProgramLinked(unsigned int index);
  static void linkProgram(unsigned int index);
  static void useProgram(unsigned int index);
  static void deleteProgram(unsigned int index);
  static unsigned int createShader(ShaderType type);
  static int getShaderInfo(unsigned int index, unsigned int infoType);
  static bool isShaderCompiled(unsigned int index);
  static void compileShader(unsigned int index);
  static void attachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void detachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void deleteShader(unsigned int index);
  static int recoverUniformLocation(unsigned int programIndex, const char* uniformName);
  static void generateFramebuffers(unsigned int count, unsigned int* indices);
  static void bindFramebuffer(unsigned int index);
  static void unbindFramebuffer() { bindFramebuffer(0); }
  static void deleteFramebuffers(unsigned int count, unsigned int* indices);
  static void checkErrors();

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) noexcept = delete;

  ~Renderer() = delete;

private:
  static void clear(unsigned int mask);

  static inline bool s_isInitialized = false;
};

} // namespace Raz

#endif //RAZ_RENDERER_HPP
