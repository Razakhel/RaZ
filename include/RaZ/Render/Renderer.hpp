#pragma once

#ifndef RAZ_RENDERER_HPP
#define RAZ_RENDERER_HPP

#include <cstddef>

namespace Raz {

enum class Capability : unsigned int {
  CULL           = 2884,  // GL_CULL_FACE
  DITHER         = 3024,  // GL_DITHER
  BLEND          = 3042,  // GL_BLEND
  COLOR_LOGIC_OP = 3058,  // GL_COLOR_LOGIC_OP

  DEPTH_CLAMP  = 34383, // GL_DEPTH_CLAMP
  DEPTH_TEST   = 2929,  // GL_DEPTH_TEST
  SCISSOR_TEST = 3089,  // GL_SCISSOR_TEST
  STENCIL_TEST = 2960,  // GL_STENCIL_TEST

  LINE_SMOOTH          = 2848,  // GL_LINE_SMOOTH
  POLYGON_SMOOTH       = 2881,  // GL_POLYGON_SMOOTH
  POLYGON_OFFSET_POINT = 10753, // GL_POLYGON_OFFSET_POINT
  POLYGON_OFFSET_LINE  = 10754, // GL_POLYGON_OFFSET_LINE
  POLYGON_OFFSET_FILL  = 32823, // GL_POLYGON_OFFSET_FILL

  MULTISAMPLE              = 32925, // GL_MULTISAMPLE
  SAMPLE_ALPHA_TO_COVERAGE = 32926, // GL_SAMPLE_ALPHA_TO_COVERAGE
  SAMPLE_ALPHA_TO_ONE      = 32927, // GL_SAMPLE_ALPHA_TO_ONE
  SAMPLE_COVERAGE          = 32928, // GL_SAMPLE_COVERAGE
  SAMPLE_SHADING           = 35894, // GL_SAMPLE_SHADING
  SAMPLE_MASK              = 36433, // GL_SAMPLE_MASK

  POINT_SIZE        = 34370, // GL_PROGRAM_POINT_SIZE
  CUBEMAP_SEAMLESS  = 34895, // GL_TEXTURE_CUBE_MAP_SEAMLESS
  FRAMEBUFFER_SRGB  = 36281, // GL_FRAMEBUFFER_SRGB
  PRIMITIVE_RESTART = 36765, // GL_PRIMITIVE_RESTART

  DEBUG_OUTPUT             = 37600, // GL_DEBUG_OUTPUT
  DEBUG_OUTPUT_SYNCHRONOUS = 33346  // GL_DEBUG_OUTPUT_SYNCHRONOUS
};

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
  NEAREST                = 9728,  // GL_NEAREST
  LINEAR                 = 9729,  // GL_LINEAR
  NEAREST_MIPMAP_NEAREST = 9984,  // GL_NEAREST_MIPMAP_NEAREST
  LINEAR_MIPMAP_NEAREST  = 9985,  // GL_LINEAR_MIPMAP_NEAREST
  NEAREST_MIPMAP_LINEAR  = 9986,  // GL_NEAREST_MIPMAP_LINEAR
  LINEAR_MIPMAP_LINEAR   = 9987,  // GL_LINEAR_MIPMAP_LINEAR
  REPEAT                 = 10497, // GL_REPEAT
  CLAMP_TO_EDGE          = 33071  // GL_CLAMP_TO_EDGE
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

enum class ProgramStatus : unsigned int {
  LINK = 35714 // GL_LINK_STATUS
};

enum class ShaderType : unsigned int {
  VERTEX   = 35633, // GL_VERTEX_SHADER
  FRAGMENT = 35632, // GL_FRAGMENT_SHADER
  GEOMETRY = 36313  // GL_GEOMETRY_SHADER
};

enum class ShaderStatus : unsigned int {
  COMPILE = 35713 // GL_COMPILE_STATUS
};

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void initialize();
  static bool isInitialized() { return s_isInitialized; }
  static void enable(Capability capability);
  static void disable(Capability capability);
  static bool isEnabled(Capability capability);
  static void clear(MaskType type) { clear(static_cast<unsigned int>(type)); }
  static void clear(MaskType type1, MaskType type2) { clear(static_cast<unsigned int>(type1) | static_cast<unsigned int>(type2)); }
  static void clear(MaskType type1, MaskType type2, MaskType type3) { clear(static_cast<unsigned int>(type1)
                                                                          | static_cast<unsigned int>(type2)
                                                                          | static_cast<unsigned int>(type3)); }
  static void generateBuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void generateBuffers(unsigned int (&indices)[N]) { generateBuffers(N, indices); }
  static void generateBuffer(unsigned int& index) { generateBuffers(1, &index); }
  static void bindBuffer(BufferType type, unsigned int index);
  static void unbindBuffer(BufferType type) { bindBuffer(type, 0); }
  static void bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex);
  static void sendBufferSubData(BufferType type, ptrdiff_t offset, ptrdiff_t dataSize, const void* data);
  template <typename T> static void sendBufferSubData(BufferType type, ptrdiff_t offset, const T& data) { sendBufferSubData(type, offset, sizeof(T), &data); }
  static void deleteBuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteBuffers(unsigned int (&indices)[N]) { deleteBuffers(N, indices); }
  static void deleteBuffer(unsigned int& index) { deleteBuffers(1, &index); }
  static void generateTextures(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void generateTextures(unsigned int (&indices)[N]) { generateTextures(N, indices); }
  static void generateTexture(unsigned int& index) { generateTextures(1, &index); }
  static void activateTexture(unsigned int index);
  static void setTextureParameter(TextureType type, TextureParam param, int value);
  static void setTextureParameter(TextureType type, TextureParam param, float value);
  static void setTextureParameter(TextureType type, TextureParam param, const int* value);
  static void setTextureParameter(TextureType type, TextureParam param, const float* value);
  static void setTextureParameter(TextureType type, TextureParam param, TextureParamValue value) { setTextureParameter(type, param, static_cast<int>(value)); }
#ifdef RAZ_USE_GL4
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, int value);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, float value);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, const int* value);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, const float* value);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, TextureParamValue value) { setTextureParameter(textureIndex, param,
                                                                                                                                static_cast<int>(value)); }
#endif
  /// Sends the image's data corresponding to the currently bound texture.
  /// \param type Type of the texture.
  /// \param mipmapLevel Mipmap (level of detail) of the texture. 0 is the most detailed.
  /// \param internalFormat Image internal format.
  /// \param width Image width.
  /// \param height Image height.
  /// \param format Image format.
  /// \param dataType Type of the data to be sent.
  /// \param data Data to be sent.
  static void sendImageData2D(TextureType type,
                              unsigned int mipmapLevel,
                              TextureInternalFormat internalFormat,
                              unsigned int width, unsigned int height,
                              TextureFormat format,
                              TextureDataType dataType, const void* data);
  /// Generate mipmaps (levels of detail) of the currently bound texture.
  /// \param type Type of the texture to generate mipmaps from.
  static void generateMipmap(TextureType type);
#ifdef RAZ_USE_GL4
  static void generateMipmap(unsigned int textureIndex);
#endif
  static void bindTexture(TextureType type, unsigned int index);
  static void unbindTexture(TextureType type) { bindTexture(type, 0); }
  static void deleteTextures(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteTextures(unsigned int (&indices)[N]) { deleteTextures(N, indices); }
  static void deleteTexture(unsigned int& index) { deleteTextures(1, &index); }
  static void resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height);
  static unsigned int createProgram();
  static int getProgramStatus(unsigned int index, ProgramStatus status);
  static bool isProgramLinked(unsigned int index);
  static void linkProgram(unsigned int index);
  static void useProgram(unsigned int index);
  static void deleteProgram(unsigned int index);
  static unsigned int createShader(ShaderType type);
  static int getShaderStatus(unsigned int index, ShaderStatus status);
  static bool isShaderCompiled(unsigned int index);
  static void compileShader(unsigned int index);
  static void attachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void detachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void deleteShader(unsigned int index);
  static int recoverUniformLocation(unsigned int programIndex, const char* uniformName);
  static void generateFramebuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void generateFramebuffers(unsigned int (&indices)[N]) { generateFramebuffers(N, indices); }
  static void generateFramebuffer(unsigned int& index) { generateFramebuffers(1, &index); }
  static void bindFramebuffer(unsigned int index);
  static void unbindFramebuffer() { bindFramebuffer(0); }
  static void deleteFramebuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteFramebuffers(unsigned int (&indices)[N]) { deleteFramebuffers(N, indices); }
  static void deleteFramebuffer(unsigned int& index) { deleteFramebuffers(1, &index); }
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
