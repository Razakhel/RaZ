#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "GL/glew.h"

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

#include <algorithm>
#include <array>
#include <cassert>

namespace Raz {

namespace {

#if !defined(RAZ_PLATFORM_MAC) && !defined(USE_OPENGL_ES)
inline void GLAPIENTRY logCallback(GLenum source,
                                   GLenum type,
                                   unsigned int id,
                                   GLenum severity,
                                   int /* length */,
                                   const char* message,
                                   const void* /* userParam */) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  if (id == 131218)
    return; // "Shader is being recompiled based on GL state". May be avoidable, but disabled for now

  std::string errorMsg = "[OpenGL]\n\t";

  switch (source) {
    case GL_DEBUG_SOURCE_API:             errorMsg += "Source: OpenGL\n\t"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   errorMsg += "Source: Window system\n\t"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: errorMsg += "Source: Shader compiler\n\t"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     errorMsg += "Source: Third party\n\t"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     errorMsg += "Source: Application\n\t"; break;
    case GL_DEBUG_SOURCE_OTHER:           errorMsg += "Source: Other\n\t"; break;
    default: break;
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:               errorMsg += "Type: Error\n\t"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: errorMsg += "Type: Deprecated behavior\n\t"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  errorMsg += "Type: Undefined behavior\n\t"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         errorMsg += "Type: Portability\n\t"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         errorMsg += "Type: Performance\n\t"; break;
    case GL_DEBUG_TYPE_OTHER:               errorMsg += "Type: Other\n\t"; break;
    default: break;
  }

  errorMsg += "ID: " + std::to_string(id) + "\n\t";

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:   errorMsg += "Severity: High\n\t"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: errorMsg += "Severity: Medium\n\t"; break;
    case GL_DEBUG_SEVERITY_LOW:    errorMsg += "Severity: Low\n\t"; break;
    // Messages with a GL_DEBUG_SEVERITY_NOTIFICATION severity are ignored at the beginning of this function
    default: break;
  }

  errorMsg += "Message: " + std::string(message);

  Logger::error(errorMsg);
}
#endif

inline constexpr const char* recoverGlErrorStr(unsigned int errorCode) {
  switch (errorCode) {
    case GL_INVALID_ENUM:                  return "Unrecognized error code (Invalid enum)";
    case GL_INVALID_VALUE:                 return "Numeric argument out of range (Invalid value)";
    case GL_INVALID_OPERATION:             return "Operation illegal in current state (Invalid operation)";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "Framebuffer object is incomplete (Invalid framebuffer operation)";
    case GL_STACK_OVERFLOW:                return "Stack overflow";
    case GL_STACK_UNDERFLOW:               return "Stack underflow";
    case GL_OUT_OF_MEMORY:                 return "Not enough memory left (Out of memory)";
    case GL_CONTEXT_LOST:                  return "OpenGL context has been lost due to a graphics card reset (Context lost)";
    case GL_NO_ERROR:                      return "No error";
    default:                               return "Unknown error";
  }
}

} // namespace

void Renderer::initialize() {
  ZoneScopedN("Renderer::initialize");

  if (s_isInitialized)
    return;

  Logger::debug("[Renderer] Initializing...");

  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    Logger::error("[Renderer] Failed to initialize GLEW");
    return;
  }

  s_isInitialized = true;

  TracyGpuContext

  getParameter(StateParameter::MAJOR_VERSION, &s_majorVersion);
  getParameter(StateParameter::MINOR_VERSION, &s_minorVersion);

  // Recovering supported extensions
  {
    int extCount {};
    getParameter(StateParameter::EXTENSION_COUNT, &extCount);

    s_extensions.reserve(static_cast<std::size_t>(extCount));

    for (int extIndex = 0; extIndex < extCount; ++extIndex)
      s_extensions.emplace(getExtension(static_cast<unsigned int>(extIndex)));
  }

#if !defined(NDEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  {
    std::string extMsg = "[Renderer] Available extensions:";
    for (const std::string& extension : s_extensions)
      extMsg += "\n    - " + extension;
    Logger::debug(extMsg);
  }
#endif

  recoverDefaultFramebufferColorFormat();
  recoverDefaultFramebufferDepthFormat();

#if !defined(RAZ_PLATFORM_MAC) && !defined(USE_OPENGL_ES) // Setting the debug message callback provokes a crash on macOS & isn't available on OpenGL ES
  if (checkVersion(4, 3)) {
    enable(Capability::DEBUG_OUTPUT);
    enable(Capability::DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&logCallback, nullptr);
  }
#endif

  Logger::debug("[Renderer] Initialized; using OpenGL "
#if defined(USE_OPENGL_ES)
    "ES "
#endif
    + std::to_string(s_majorVersion) + '.' + std::to_string(s_minorVersion));
}

void Renderer::enable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glEnable(static_cast<unsigned int>(capability));

  printConditionalErrors();
}

void Renderer::disable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDisable(static_cast<unsigned int>(capability));

  printConditionalErrors();
}

bool Renderer::isEnabled(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isEnabled = (glIsEnabled(static_cast<unsigned int>(capability)) == GL_TRUE);

  printConditionalErrors();

  return isEnabled;
}

std::string Renderer::getContextInfo(ContextInfo info) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  std::string res = reinterpret_cast<const char*>(glGetString(static_cast<unsigned int>(info)));

  printConditionalErrors();

  return res;
}

std::string Renderer::getExtension(unsigned int extIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
#if defined(RAZ_CONFIG_DEBUG)
  int extCount {};
  getParameter(StateParameter::EXTENSION_COUNT, &extCount);
  assert("Error: Extension index must be less than the total extension count." && static_cast<int>(extIndex) < extCount);
#endif

  std::string extension = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, extIndex));

  printConditionalErrors();

  return extension;
}

void Renderer::getParameter(StateParameter parameter, unsigned char* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetBooleanv(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetIntegerv(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, int64_t* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetInteger64v(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetFloatv(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, double* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetDoublev(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, unsigned int index, unsigned char* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetBooleani_v(static_cast<unsigned int>(parameter), index, values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, unsigned int index, int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetIntegeri_v(static_cast<unsigned int>(parameter), index, values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, unsigned int index, int64_t* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetInteger64i_v(static_cast<unsigned int>(parameter), index, values);

  printConditionalErrors();
}

unsigned int Renderer::getActiveTexture() {
  int texture {};
  getParameter(StateParameter::ACTIVE_TEXTURE, &texture);

  return static_cast<unsigned int>(texture - GL_TEXTURE0);
}

unsigned int Renderer::getCurrentProgram() {
  int program {};
  getParameter(StateParameter::CURRENT_PROGRAM, &program);

  return static_cast<unsigned int>(program);
}

void Renderer::clearColor(float red, float green, float blue, float alpha) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glClearColor(red, green, blue, alpha);

  printConditionalErrors();
}

void Renderer::clear(MaskType mask) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::clear")

  glClear(static_cast<unsigned int>(mask));

  printConditionalErrors();
}

void Renderer::setDepthFunction(DepthStencilFunction func) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::setDepthFunction")

  glDepthFunc(static_cast<unsigned int>(func));

  printConditionalErrors();
}

void Renderer::setStencilFunction(DepthStencilFunction func, int ref, unsigned int mask, FaceOrientation orientation) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glStencilFuncSeparate(static_cast<unsigned int>(orientation), static_cast<unsigned int>(func), ref, mask);

  printConditionalErrors();
}

void Renderer::setStencilOperations(StencilOperation stencilFailOp, StencilOperation depthFailOp, StencilOperation successOp, FaceOrientation orientation) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glStencilOpSeparate(static_cast<unsigned int>(stencilFailOp),
                      static_cast<unsigned int>(depthFailOp),
                      static_cast<unsigned int>(successOp),
                      static_cast<unsigned int>(orientation));

  printConditionalErrors();
}

void Renderer::setStencilMask(unsigned int mask, FaceOrientation orientation) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glStencilMaskSeparate(static_cast<unsigned int>(orientation), mask);

  printConditionalErrors();
}

void Renderer::setBlendFunction(BlendFactor source, BlendFactor destination) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBlendFunc(static_cast<unsigned int>(source), static_cast<unsigned int>(destination));

  printConditionalErrors();
}

void Renderer::setFaceCulling(FaceOrientation orientation) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glCullFace(static_cast<unsigned int>(orientation));

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::setPolygonMode(FaceOrientation orientation, PolygonMode mode) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glPolygonMode(static_cast<unsigned int>(orientation), static_cast<unsigned int>(mode));

  printConditionalErrors();
}

void Renderer::setClipControl(ClipOrigin origin, ClipDepth depth) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Setting clip control requires OpenGL 4.5+ or the 'GL_ARB_clip_control' extension."
      && (checkVersion(4, 5) || isExtensionSupported("GL_ARB_clip_control")));

  glClipControl(static_cast<unsigned int>(origin), static_cast<unsigned int>(depth));

  printConditionalErrors();
}

void Renderer::setPatchVertexCount(int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Setting patch vertices requires OpenGL 4.0+ or the 'GL_ARB_tessellation_shader' extension."
      && (checkVersion(4, 0) || isExtensionSupported("GL_ARB_tessellation_shader")));
  assert("Error: A patch needs at least one vertex." && value > 0);

  glPatchParameteri(GL_PATCH_VERTICES, value);

  printConditionalErrors();
}

void Renderer::setPatchParameter(PatchParameter param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Setting a patch parameter requires OpenGL 4.0+ or the 'GL_ARB_tessellation_shader' extension."
      && (checkVersion(4, 0) || isExtensionSupported("GL_ARB_tessellation_shader")));

  glPatchParameterfv(static_cast<unsigned int>(param), values);

  printConditionalErrors();
}
#endif

void Renderer::setPixelStorage(PixelStorage storage, unsigned int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glPixelStorei(static_cast<unsigned int>(storage), static_cast<int>(value));

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = Renderer::recoverErrors();

  if (errorCodes[ErrorCode::INVALID_VALUE])
    Logger::error("Renderer::setPixelStorage - " + std::to_string(value) + " is not a valid alignment value. Only 1, 2, 4 & 8 are accepted");
#endif
}

void Renderer::recoverFrame(unsigned int width, unsigned int height, TextureFormat format, PixelDataType dataType, void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::recoverFrame")

  glReadPixels(0, 0, static_cast<int>(width), static_cast<int>(height), static_cast<unsigned int>(format), static_cast<unsigned int>(dataType), data);

  printConditionalErrors();
}

void Renderer::generateVertexArrays(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenVertexArrays(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::bindVertexArray(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindVertexArray(index);

  printConditionalErrors();
}

void Renderer::enableVertexAttribArray(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glEnableVertexAttribArray(index);

  printConditionalErrors();
}

void Renderer::setVertexAttrib(unsigned int index, AttribDataType dataType, uint8_t size, unsigned int stride, unsigned int offset, bool normalize) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glVertexAttribPointer(index, size, static_cast<unsigned int>(dataType), normalize, static_cast<int>(stride), reinterpret_cast<const void*>(offset));

  printConditionalErrors();
}

void Renderer::setVertexAttribDivisor(unsigned int index, unsigned int divisor) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glVertexAttribDivisor(index, divisor);

  printConditionalErrors();
}

void Renderer::deleteVertexArrays(unsigned int count, const unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteVertexArrays(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::generateBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenBuffers(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::bindBuffer(BufferType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBuffer(static_cast<unsigned int>(type), index);

  printConditionalErrors();
}

void Renderer::bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBufferBase(static_cast<unsigned int>(type), bindingIndex, bufferIndex);

  printConditionalErrors();
}

void Renderer::bindBufferRange(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex, std::ptrdiff_t offset, std::ptrdiff_t size) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBufferRange(static_cast<unsigned int>(type), bindingIndex, bufferIndex, offset, size);

  printConditionalErrors();
}

void Renderer::sendBufferData(BufferType type, std::ptrdiff_t size, const void* data, BufferDataUsage usage) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBufferData(static_cast<unsigned int>(type), size, data, static_cast<unsigned int>(usage));

  printConditionalErrors();
}

void Renderer::sendBufferSubData(BufferType type, std::ptrdiff_t offset, std::ptrdiff_t dataSize, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBufferSubData(static_cast<unsigned int>(type), offset, dataSize, data);

  printConditionalErrors();
}

void Renderer::deleteBuffers(unsigned int count, const unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteBuffers(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::generateTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenTextures(static_cast<int>(count), indices);

  printConditionalErrors();
}

bool Renderer::isTexture(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isTexture = (glIsTexture(index) == GL_TRUE);

  printConditionalErrors();

  return isTexture;
}

void Renderer::bindTexture(TextureType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindTexture(static_cast<unsigned int>(type), index);

  printConditionalErrors();
}

#if !defined(USE_WEBGL)
void Renderer::bindImageTexture(unsigned int imageUnitIndex, unsigned int textureIndex, int textureLevel,
                                bool isLayered, int layer,
                                ImageAccess imgAccess, ImageInternalFormat imgFormat) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
#if !defined(USE_OPENGL_ES)
  assert("Error: Binding an image texture requires OpenGL 4.2+." && checkVersion(4, 2));
#else
  assert("Error: Binding an image texture requires OpenGL ES 3.1+." && checkVersion(3, 1));
#endif

  glBindImageTexture(imageUnitIndex, textureIndex, textureLevel, isLayered, layer, static_cast<unsigned int>(imgAccess), static_cast<unsigned int>(imgFormat));

  printConditionalErrors();
}
#endif

void Renderer::setActiveTexture(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glActiveTexture(GL_TEXTURE0 + index);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParameter param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteri(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParameter param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterf(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParameter param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteriv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParameter param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterfv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::setTextureParameter(unsigned int textureIndex, TextureParameter param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameteri(textureIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParameter param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameterf(textureIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParameter param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameteriv(textureIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParameter param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameterfv(textureIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::sendImageData1D(TextureType type,
                               unsigned int mipmapLevel,
                               TextureInternalFormat internalFormat,
                               unsigned int width,
                               TextureFormat format,
                               PixelDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::sendImageData1D")

  glTexImage1D(static_cast<unsigned int>(type),
               static_cast<int>(mipmapLevel),
               static_cast<int>(internalFormat),
               static_cast<int>(width),
               0,
               static_cast<unsigned int>(format),
               static_cast<unsigned int>(dataType),
               data);

  printConditionalErrors();
}

void Renderer::sendImageSubData1D(TextureType type,
                                  unsigned int mipmapLevel,
                                  unsigned int offsetX,
                                  unsigned int width,
                                  TextureFormat format,
                                  PixelDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::sendImageSubData1D")

  glTexSubImage1D(static_cast<unsigned int>(type),
                  static_cast<int>(mipmapLevel),
                  static_cast<int>(offsetX),
                  static_cast<int>(width),
                  static_cast<unsigned int>(format),
                  static_cast<unsigned int>(dataType),
                  data);

  printConditionalErrors();
}
#endif

void Renderer::sendImageData2D(TextureType type,
                               unsigned int mipmapLevel,
                               TextureInternalFormat internalFormat,
                               unsigned int width, unsigned int height,
                               TextureFormat format,
                               PixelDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::sendImageData2D")

  glTexImage2D(static_cast<unsigned int>(type),
               static_cast<int>(mipmapLevel),
               static_cast<int>(internalFormat),
               static_cast<int>(width),
               static_cast<int>(height),
               0,
               static_cast<unsigned int>(format),
               static_cast<unsigned int>(dataType),
               data);

  printConditionalErrors();
}

void Renderer::sendImageSubData2D(TextureType type,
                                  unsigned int mipmapLevel,
                                  unsigned int offsetX, unsigned int offsetY,
                                  unsigned int width, unsigned int height,
                                  TextureFormat format,
                                  PixelDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::sendImageSubData2D")

  glTexSubImage2D(static_cast<unsigned int>(type),
                  static_cast<int>(mipmapLevel),
                  static_cast<int>(offsetX),
                  static_cast<int>(offsetY),
                  static_cast<int>(width),
                  static_cast<int>(height),
                  static_cast<unsigned int>(format),
                  static_cast<unsigned int>(dataType),
                  data);

  printConditionalErrors();
}

void Renderer::sendImageData3D(TextureType type,
                               unsigned int mipmapLevel,
                               TextureInternalFormat internalFormat,
                               unsigned int width, unsigned int height, unsigned int depth,
                               TextureFormat format,
                               PixelDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::sendImageData3D")

  glTexImage3D(static_cast<unsigned int>(type),
               static_cast<int>(mipmapLevel),
               static_cast<int>(internalFormat),
               static_cast<int>(width),
               static_cast<int>(height),
               static_cast<int>(depth),
               0,
               static_cast<unsigned int>(format),
               static_cast<unsigned int>(dataType),
               data);

  printConditionalErrors();
}

void Renderer::sendImageSubData3D(TextureType type,
                                  unsigned int mipmapLevel,
                                  unsigned int offsetX, unsigned int offsetY, unsigned int offsetZ,
                                  unsigned int width, unsigned int height, unsigned int depth,
                                  TextureFormat format,
                                  PixelDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::sendImageSubData3D")

  glTexSubImage3D(static_cast<unsigned int>(type),
                  static_cast<int>(mipmapLevel),
                  static_cast<int>(offsetX),
                  static_cast<int>(offsetY),
                  static_cast<int>(offsetZ),
                  static_cast<int>(width),
                  static_cast<int>(height),
                  static_cast<int>(depth),
                  static_cast<unsigned int>(format),
                  static_cast<unsigned int>(dataType),
                  data);

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetTexLevelParameteriv(static_cast<unsigned int>(type), static_cast<int>(mipmapLevel), static_cast<unsigned int>(attribute), values);

  printConditionalErrors();
}

void Renderer::recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetTexLevelParameterfv(static_cast<unsigned int>(type), static_cast<int>(mipmapLevel), static_cast<unsigned int>(attribute), values);

  printConditionalErrors();
}

int Renderer::recoverTextureWidth(TextureType type, unsigned int mipmapLevel) {
  int width {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::WIDTH, &width);

  return width;
}

int Renderer::recoverTextureHeight(TextureType type, unsigned int mipmapLevel) {
  int height {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::HEIGHT, &height);

  return height;
}

int Renderer::recoverTextureDepth(TextureType type, unsigned int mipmapLevel) {
  int depth {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::DEPTH, &depth);

  return depth;
}

TextureInternalFormat Renderer::recoverTextureInternalFormat(TextureType type, unsigned int mipmapLevel) {
  int format {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::INTERNAL_FORMAT, &format);

  return static_cast<TextureInternalFormat>(format);
}

void Renderer::recoverTextureData(TextureType type, unsigned int mipmapLevel, TextureFormat format, PixelDataType dataType, void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::recoverTextureData")

  glGetTexImage(static_cast<unsigned int>(type),
                static_cast<int>(mipmapLevel),
                static_cast<unsigned int>(format),
                static_cast<unsigned int>(dataType),
                data);

  printConditionalErrors();
}
#endif

void Renderer::generateMipmap(TextureType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::generateMipmap")

  glGenerateMipmap(static_cast<unsigned int>(type));

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::generateMipmap(unsigned int textureIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to generate mipmap with a texture index." && checkVersion(4, 5));

  TracyGpuZone("Renderer::generateMipmap")

  glGenerateTextureMipmap(textureIndex);

  printConditionalErrors();
}
#endif

void Renderer::deleteTextures(unsigned int count, const unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteTextures(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::generateSamplers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenSamplers(static_cast<int>(count), indices);

  printConditionalErrors();
}

bool Renderer::isSampler(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isSampler = (glIsSampler(index) == GL_TRUE);

  printConditionalErrors();

  return isSampler;
}

void Renderer::bindSampler(unsigned int textureUnit, unsigned int samplerIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindSampler(textureUnit, samplerIndex);

  printConditionalErrors();
}

void Renderer::setSamplerParameter(unsigned int samplerIndex, SamplerParameter param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glSamplerParameteri(samplerIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setSamplerParameter(unsigned int samplerIndex, SamplerParameter param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glSamplerParameterf(samplerIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setSamplerParameter(unsigned int samplerIndex, SamplerParameter param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glSamplerParameteriv(samplerIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setSamplerParameter(unsigned int samplerIndex, SamplerParameter param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glSamplerParameterfv(samplerIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::deleteSamplers(unsigned int count, const unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteSamplers(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glViewport(xOrigin, yOrigin, static_cast<int>(width), static_cast<int>(height));

  printConditionalErrors();
}

unsigned int Renderer::createProgram() {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int programIndex = glCreateProgram();

  printConditionalErrors();

  return programIndex;
}

void Renderer::getProgramParameter(unsigned int index, ProgramParameter parameter, int* parameters) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetProgramiv(index, static_cast<unsigned int>(parameter), parameters);

  printConditionalErrors();
}

bool Renderer::isProgramLinked(unsigned int index) {
  int linkStatus {};
  getProgramParameter(index, ProgramParameter::LINK_STATUS, &linkStatus);

  return (linkStatus == GL_TRUE);
}

unsigned int Renderer::recoverActiveUniformCount(unsigned int programIndex) {
  int uniformCount {};
  getProgramParameter(programIndex, ProgramParameter::ACTIVE_UNIFORMS, &uniformCount);

  return static_cast<unsigned int>(uniformCount);
}

std::vector<unsigned int> Renderer::recoverAttachedShaders(unsigned int programIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int attachedShaderCount {};
  getProgramParameter(programIndex, ProgramParameter::ATTACHED_SHADERS, &attachedShaderCount);

  if (attachedShaderCount == 0)
    return {};

  std::vector<unsigned int> shaderIndices(static_cast<std::size_t>(attachedShaderCount));

  int recoveredShaderCount {};
  glGetAttachedShaders(programIndex, attachedShaderCount, &recoveredShaderCount, shaderIndices.data());

  printConditionalErrors();

  if (recoveredShaderCount == 0)
    return {};

  return shaderIndices;
}

void Renderer::linkProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::linkProgram")

  glLinkProgram(index);

  if (!isProgramLinked(index)) {
    char infoLog[512];

    glGetProgramInfoLog(index, static_cast<int>(std::size(infoLog)), nullptr, infoLog);
    Logger::error("Shader program link failed (ID " + std::to_string(index) + "): " + infoLog);
  }

  printConditionalErrors();
}

void Renderer::useProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::useProgram")

  glUseProgram(index);

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = Renderer::recoverErrors();

  if (errorCodes[ErrorCode::INVALID_VALUE])
    Logger::error("Renderer::useProgram - Invalid shader program index (" + std::to_string(index) + ')');

  if (errorCodes[ErrorCode::INVALID_OPERATION]) {
    std::string errorMsg = "Renderer::useProgram - ";

    if (!isProgramLinked(index))
      errorMsg += "A shader program must be linked before being defined as used.";
    else
      errorMsg += "Unknown invalid operation.";

    Logger::error(errorMsg);
  }
#endif
}

void Renderer::deleteProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteProgram(index);

  printConditionalErrors();
}

unsigned int Renderer::createShader(ShaderType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
#if !defined(USE_OPENGL_ES)
  assert("Error: Creating a tessellation shader requires OpenGL 4.0+ or the 'GL_ARB_tessellation_shader' extension."
      && ((type != ShaderType::TESSELLATION_CONTROL && type != ShaderType::TESSELLATION_EVALUATION)
      || checkVersion(4, 0) || isExtensionSupported("GL_ARB_tessellation_shader")));
  assert("Error: Creating a compute shader requires OpenGL 4.3+ or the 'GL_ARB_compute_shader' extension."
      && (type != ShaderType::COMPUTE || checkVersion(4, 3) || isExtensionSupported("GL_ARB_compute_shader")));
#else
  assert("Error: Geometry shaders are unsupported with OpenGL ES." && type != ShaderType::GEOMETRY);
  assert("Error: Tessellation shaders are unsupported with OpenGL ES."
         && type != ShaderType::TESSELLATION_CONTROL && type != ShaderType::TESSELLATION_EVALUATION);
  assert("Error: Creating a compute shader requires OpenGL ES 3.1+ or the 'GL_ARB_compute_shader' extension."
      && (type != ShaderType::COMPUTE || checkVersion(3, 1) || isExtensionSupported("GL_ARB_compute_shader")));
#endif

  const unsigned int shaderIndex = glCreateShader(static_cast<unsigned int>(type));

  printConditionalErrors();

  return shaderIndex;
}

int Renderer::recoverShaderInfo(unsigned int index, ShaderInfo info) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int res {};
  glGetShaderiv(index, static_cast<unsigned int>(info), &res);

  printConditionalErrors();

  return res;
}

void Renderer::sendShaderSource(unsigned int index, const char* source, int length) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::sendShaderSource")

  glShaderSource(index, 1, &source, &length);

  printConditionalErrors();
}

std::string Renderer::recoverShaderSource(unsigned int index) {
  TracyGpuZone("Renderer::recoverShaderSource")

  const int sourceLength = recoverShaderInfo(index, ShaderInfo::SOURCE_LENGTH);

  if (sourceLength == 0)
    return {};

  std::string source;
  source.resize(static_cast<std::size_t>(sourceLength - 1)); // The recovered length includes the null terminator, hence the -1

  glGetShaderSource(index, sourceLength, nullptr, source.data());

  printConditionalErrors();

  return source;
}

void Renderer::compileShader(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::compileShader")

  glCompileShader(index);

  if (!isShaderCompiled(index)) {
    char infoLog[512];

    glGetShaderInfoLog(index, static_cast<int>(std::size(infoLog)), nullptr, infoLog);
    Logger::error("Shader compilation failed (ID " + std::to_string(index) + "): " + infoLog);
  }

  printConditionalErrors();
}

void Renderer::attachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glAttachShader(programIndex, shaderIndex);

  printConditionalErrors();
}

void Renderer::detachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDetachShader(programIndex, shaderIndex);

  printConditionalErrors();
}

bool Renderer::isShaderAttached(unsigned int programIndex, unsigned int shaderIndex) {
  const std::vector<unsigned int> shaderIndices = recoverAttachedShaders(programIndex);
  return (std::find(shaderIndices.cbegin(), shaderIndices.cend(), shaderIndex) != shaderIndices.cend());
}

void Renderer::deleteShader(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteShader(index);

  printConditionalErrors();
}

int Renderer::recoverUniformLocation(unsigned int programIndex, const char* uniformName) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const int location = glGetUniformLocation(programIndex, uniformName);

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  printErrors();

  if (location == -1)
    Logger::warn("Uniform '" + std::string(uniformName) + "' unrecognized");
#endif

  return location;
}

void Renderer::recoverUniformInfo(unsigned int programIndex, unsigned int uniformIndex, UniformType& type, std::string& name, int* size) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int nameLength {};
  int uniformSize {};
  unsigned int uniformType {};
  std::array<char, 256> uniformName {};

  glGetActiveUniform(programIndex, uniformIndex, static_cast<int>(uniformName.size()), &nameLength, &uniformSize, &uniformType, uniformName.data());

  type = static_cast<UniformType>(uniformType);

  name.resize(static_cast<std::size_t>(nameLength));
  std::copy(uniformName.cbegin(), uniformName.cbegin() + nameLength, name.begin());

  if (size)
    *size = uniformSize;

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes.isEmpty())
    return;

  if (errorCodes[ErrorCode::INVALID_OPERATION])
    Logger::error("Renderer::recoverUniformInfo - Tried to fetch program information from a non-program object");

  if (errorCodes[ErrorCode::INVALID_VALUE]) {
    std::string errorMsg = "Renderer::recoverUniformInfo - ";

    const unsigned int uniCount = recoverActiveUniformCount(programIndex);

    if (uniformIndex >= uniCount) {
      errorMsg += "The given uniform index (" + std::to_string(uniformIndex) + ") "
                  "is greater than or equal to the program's active uniform count (" + std::to_string(uniCount) + ").";
    } else {
      errorMsg += "The given program index has not been created by OpenGL.";
    }

    Logger::error(errorMsg);
  }
#endif
}

UniformType Renderer::recoverUniformType(unsigned int programIndex, unsigned int uniformIndex) {
  UniformType type {};
  std::string name;
  recoverUniformInfo(programIndex, uniformIndex, type, name);

  return type;
}

std::string Renderer::recoverUniformName(unsigned int programIndex, unsigned int uniformIndex) {
  UniformType type {};
  std::string name;
  recoverUniformInfo(programIndex, uniformIndex, type, name);

  return name;
}

void Renderer::recoverUniformData(unsigned int programIndex, int uniformIndex, int* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetUniformiv(programIndex, uniformIndex, data);

  printConditionalErrors();
}

void Renderer::recoverUniformData(unsigned int programIndex, int uniformIndex, unsigned int* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetUniformuiv(programIndex, uniformIndex, data);

  printConditionalErrors();
}

void Renderer::recoverUniformData(unsigned int programIndex, int uniformIndex, float* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetUniformfv(programIndex, uniformIndex, data);

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::recoverUniformData(unsigned int programIndex, int uniformIndex, double* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Recovering uniform data of type double requires OpenGL 4.0+." && checkVersion(4, 0));

  glGetUniformdv(programIndex, uniformIndex, data);

  printConditionalErrors();
}
#endif

void Renderer::bindUniformBlock(unsigned int programIndex, unsigned int uniformBlockIndex, unsigned int bindingIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformBlockBinding(programIndex, uniformBlockIndex, bindingIndex);

  printConditionalErrors();
}

unsigned int Renderer::recoverUniformBlockIndex(unsigned int programIndex, const char* uniformBlockName) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int index = glGetUniformBlockIndex(programIndex, uniformBlockName);

  printConditionalErrors();

  return index;
}

void Renderer::sendUniform(int uniformIndex, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1i(uniformIndex, value);

  printConditionalErrors();
}

void Renderer::sendUniform(int uniformIndex, unsigned int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1ui(uniformIndex, value);

  printConditionalErrors();
}

void Renderer::sendUniform(int uniformIndex, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1f(uniformIndex, value);

  printConditionalErrors();
}

void Renderer::sendUniformVector1i(int uniformIndex, const int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1iv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector2i(int uniformIndex, const int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform2iv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector3i(int uniformIndex, const int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform3iv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector4i(int uniformIndex, const int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform4iv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector1ui(int uniformIndex, const unsigned int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1uiv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector2ui(int uniformIndex, const unsigned int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform2uiv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector3ui(int uniformIndex, const unsigned int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform3uiv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector4ui(int uniformIndex, const unsigned int* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform4uiv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector1(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector2(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform2fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector3(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform3fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector4(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform4fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformMatrix2x2(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix2fv(uniformIndex, count, transpose, values);

  printConditionalErrors();
}

void Renderer::sendUniformMatrix3x3(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix3fv(uniformIndex, count, transpose, values);

  printConditionalErrors();
}

void Renderer::sendUniformMatrix4x4(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix4fv(uniformIndex, count, transpose, values);

  printConditionalErrors();
}

void Renderer::generateFramebuffers(int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenFramebuffers(count, indices);

  printConditionalErrors();
}

void Renderer::bindFramebuffer(unsigned int index, FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindFramebuffer(static_cast<unsigned int>(type), index);

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes[ErrorCode::INVALID_OPERATION])
    Logger::error("Renderer::bindFramebuffer - Bound object is not a valid framebuffer");
#endif
}

FramebufferStatus Renderer::getFramebufferStatus(FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int status = glCheckFramebufferStatus(static_cast<unsigned int>(type));

  printConditionalErrors();

  return static_cast<FramebufferStatus>(status);
}

#if !defined(USE_OPENGL_ES)
void Renderer::setFramebufferTexture(FramebufferAttachment attachment,
                                     unsigned int textureIndex, unsigned int mipmapLevel,
                                     FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::setFramebufferTexture")

  glFramebufferTexture(static_cast<unsigned int>(type),
                       static_cast<unsigned int>(attachment),
                       textureIndex,
                       static_cast<int>(mipmapLevel));

  printConditionalErrors();
}

void Renderer::setFramebufferTexture1D(FramebufferAttachment attachment,
                                       unsigned int textureIndex, unsigned int mipmapLevel,
                                       FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::setFramebufferTexture1D")

  glFramebufferTexture1D(static_cast<unsigned int>(type),
                         static_cast<unsigned int>(attachment),
                         static_cast<unsigned int>(TextureType::TEXTURE_1D),
                         textureIndex,
                         static_cast<int>(mipmapLevel));

  printConditionalErrors();
}
#endif

void Renderer::setFramebufferTexture2D(FramebufferAttachment attachment,
                                       unsigned int textureIndex, unsigned int mipmapLevel,
                                       TextureType textureType,
                                       FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::setFramebufferTexture2D")

  glFramebufferTexture2D(static_cast<unsigned int>(type),
                         static_cast<unsigned int>(attachment),
                         static_cast<unsigned int>(textureType),
                         textureIndex,
                         static_cast<int>(mipmapLevel));

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::setFramebufferTexture3D(FramebufferAttachment attachment,
                                       unsigned int textureIndex, unsigned int mipmapLevel, unsigned int layer,
                                       FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::setFramebufferTexture3D")

  glFramebufferTexture3D(static_cast<unsigned int>(type),
                         static_cast<unsigned int>(attachment),
                         static_cast<unsigned int>(TextureType::TEXTURE_3D),
                         textureIndex,
                         static_cast<int>(mipmapLevel),
                         static_cast<int>(layer));

  printConditionalErrors();
}
#endif

void Renderer::recoverFramebufferAttachmentParameter(FramebufferAttachment attachment,
                                                     FramebufferAttachmentParam param,
                                                     int* values,
                                                     FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetFramebufferAttachmentParameteriv(static_cast<unsigned int>(type), static_cast<unsigned int>(attachment), static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setReadBuffer(ReadBuffer buffer) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glReadBuffer(static_cast<unsigned int>(buffer));

  printConditionalErrors();
}

void Renderer::setDrawBuffers(unsigned int count, const DrawBuffer* buffers) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDrawBuffers(static_cast<int>(count), reinterpret_cast<const unsigned int*>(buffers));

  printConditionalErrors();
}

void Renderer::blitFramebuffer(int readMinX, int readMinY, int readMaxX, int readMaxY,
                               int writeMinX, int writeMinY, int writeMaxX, int writeMaxY,
                               MaskType mask, BlitFilter filter) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::blitFramebuffer")

  glBlitFramebuffer(readMinX, readMinY, readMaxX, readMaxY,
                    writeMinX, writeMinY, writeMaxX, writeMaxY,
                    static_cast<unsigned int>(mask), static_cast<unsigned int>(filter));

  printConditionalErrors();
}

void Renderer::deleteFramebuffers(unsigned int count, const unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteFramebuffers(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::drawArrays(PrimitiveType type, unsigned int first, unsigned int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::drawArrays")

  glDrawArrays(static_cast<unsigned int>(type), static_cast<int>(first), static_cast<int>(count));

  printConditionalErrors();
}

void Renderer::drawArraysInstanced(PrimitiveType type, unsigned int first, unsigned int primitiveCount, unsigned int instanceCount) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::drawArraysInstanced")

  glDrawArraysInstanced(static_cast<unsigned int>(type), static_cast<int>(first), static_cast<int>(primitiveCount), static_cast<int>(instanceCount));

  printConditionalErrors();
}

void Renderer::drawElements(PrimitiveType type, unsigned int count, ElementDataType dataType, const void* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::drawElements")

  glDrawElements(static_cast<unsigned int>(type), static_cast<int>(count), static_cast<unsigned int>(dataType), indices);

  printConditionalErrors();
}

void Renderer::drawElementsInstanced(PrimitiveType type, unsigned int primitiveCount,
                                     ElementDataType dataType, const void* indices,
                                     unsigned int instanceCount) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  TracyGpuZone("Renderer::drawElementsInstanced")

  glDrawElementsInstanced(static_cast<unsigned int>(type), static_cast<int>(primitiveCount),
                          static_cast<unsigned int>(dataType), indices,
                          static_cast<int>(instanceCount));

  printConditionalErrors();
}

void Renderer::dispatchCompute(unsigned int groupCountX, unsigned int groupCountY, unsigned int groupCountZ) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
#if !defined(USE_OPENGL_ES)
  assert("Error: Launching a compute operation requires OpenGL 4.3+ or the 'GL_ARB_compute_shader' extension."
      && (checkVersion(4, 3) || isExtensionSupported("GL_ARB_compute_shader")));
#else
  assert("Error: Launching a compute operation requires OpenGL ES 3.1+ or the 'GL_ARB_compute_shader' extension."
      && (checkVersion(3, 1) || isExtensionSupported("GL_ARB_compute_shader")));
#endif

  TracyGpuZone("Renderer::dispatchCompute")

  glDispatchCompute(groupCountX, groupCountY, groupCountZ);

  printConditionalErrors();
}

void Renderer::setMemoryBarrier(BarrierType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
#if !defined(USE_OPENGL_ES)
  assert("Error: Setting a memory barrier requires OpenGL 4.2+." && checkVersion(4, 2));
#else
  assert("Error: Setting a memory barrier requires OpenGL ES 3.1+." && checkVersion(3, 1));
#endif

  TracyGpuZone("Renderer::setMemoryBarrier")

  glMemoryBarrier(static_cast<unsigned int>(type));

  printConditionalErrors();
}

void Renderer::setMemoryBarrierByRegion(RegionBarrierType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
#if !defined(USE_OPENGL_ES)
  assert("Error: Setting a memory barrier by region requires OpenGL 4.5+." && checkVersion(4, 5));
#else
  assert("Error: Setting a memory barrier by region requires OpenGL ES 3.1+." && checkVersion(3, 1));
#endif

  TracyGpuZone("Renderer::setMemoryBarrierByRegion")

  glMemoryBarrierByRegion(static_cast<unsigned int>(type));

  printConditionalErrors();
}

void Renderer::generateQueries(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenQueries(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::beginQuery(QueryType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBeginQuery(static_cast<unsigned int>(type), index);

  printConditionalErrors();
}

void Renderer::endQuery(QueryType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glEndQuery(static_cast<unsigned int>(type));

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::recoverQueryResult(unsigned int index, int64_t& result) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetQueryObjecti64v(index, GL_QUERY_RESULT, &result);

  printConditionalErrors();
}

void Renderer::recoverQueryResult(unsigned int index, uint64_t& result) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetQueryObjectui64v(index, GL_QUERY_RESULT, &result);

  printConditionalErrors();
}
#endif

void Renderer::deleteQueries(unsigned int count, const unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteQueries(static_cast<int>(count), indices);

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::setLabel(RenderObjectType type, unsigned int objectIndex, const char* label) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Setting an object label requires OpenGL 4.3+." && checkVersion(4, 3));

  glObjectLabel(static_cast<unsigned int>(type), objectIndex, -1, label);

  printConditionalErrors();
}

std::string Renderer::recoverLabel(RenderObjectType type, unsigned int objectIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Recovering an object label requires OpenGL 4.3+." && checkVersion(4, 3));

  int labelLength {};
  std::array<char, 256> labelName {};

  glGetObjectLabel(static_cast<unsigned int>(type), objectIndex, static_cast<int>(labelName.size()), &labelLength, labelName.data());

  std::string label;
  label.resize(static_cast<std::size_t>(labelLength));
  std::copy(labelName.cbegin(), labelName.cbegin() + labelLength, label.begin());

  printConditionalErrors();

  return label;
}

void Renderer::pushDebugGroup(const std::string& name) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Pushing a debug group requires OpenGL 4.3+." && checkVersion(4, 3));

  glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<int>(name.size()), name.c_str());

  printConditionalErrors();
}

void Renderer::popDebugGroup() {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Popping a debug group requires OpenGL 4.3+." && checkVersion(4, 3));

  glPopDebugGroup();

  printConditionalErrors();
}
#endif

ErrorCodes Renderer::recoverErrors() noexcept {
  static constexpr auto recoverErrorCodeIndex = [] (ErrorCode code) constexpr noexcept -> uint8_t {
    return static_cast<uint8_t>(static_cast<unsigned int>(code) - static_cast<unsigned int>(ErrorCode::INVALID_ENUM));
  };

  ErrorCodes errorCodes;

  while (true) {
    const unsigned int errorCode = glGetError();

    if (errorCode == GL_NO_ERROR)
      break;

    const uint8_t errorCodeIndex = recoverErrorCodeIndex(static_cast<ErrorCode>(errorCode));

    // An error code cannot be returned twice in a row; if it is, the error checking should be stopped
    if (errorCodes.codes[errorCodeIndex])
      break;

    errorCodes.codes[errorCodeIndex] = true;
  }

  return errorCodes;
}

void Renderer::printErrors() {
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes.isEmpty())
    return;

  for (uint8_t errorIndex = 0; errorIndex < static_cast<uint8_t>(errorCodes.codes.size()); ++errorIndex) {
    if (errorCodes.codes[errorIndex]) {
      const unsigned int errorValue = errorIndex + static_cast<unsigned int>(ErrorCode::INVALID_ENUM);
      Logger::error("[OpenGL] " + std::string(recoverGlErrorStr(errorValue)) + " (code " + std::to_string(errorValue) + ')');
    }
  }
}

void Renderer::recoverDefaultFramebufferColorFormat() {
  struct ColorInfo {
    int redBitCount {};
    int greenBitCount {};
    int blueBitCount {};
    int alphaBitCount {};
    int compType {};
    int encoding {};
  };

  struct ColorFormat {
    ColorInfo colorInfo {};
    TextureInternalFormat format {};
    std::string_view formatStr;
  };

  constexpr std::array<ColorFormat, 26> formats = {{
    { ColorInfo{ 8,  8,  8,  0,  GL_UNSIGNED_NORMALIZED, GL_LINEAR }, TextureInternalFormat::RGB8,           "RGB8"           },
    { ColorInfo{ 8,  8,  8,  8,  GL_UNSIGNED_NORMALIZED, GL_LINEAR }, TextureInternalFormat::RGBA8,          "RGBA8"          },
    { ColorInfo{ 8,  8,  8,  0,  GL_UNSIGNED_NORMALIZED, GL_SRGB   }, TextureInternalFormat::SRGB8,          "SRGB8"          },
    { ColorInfo{ 8,  8,  8,  8,  GL_UNSIGNED_NORMALIZED, GL_SRGB   }, TextureInternalFormat::SRGBA8,         "SRGBA8"         },
    { ColorInfo{ 8,  8,  8,  0,  GL_INT,                 GL_LINEAR }, TextureInternalFormat::RGB8I,          "RGB8I"          },
    { ColorInfo{ 8,  8,  8,  8,  GL_INT,                 GL_LINEAR }, TextureInternalFormat::RGBA8I,         "RGBA8I"         },
    { ColorInfo{ 8,  8,  8,  0,  GL_UNSIGNED_INT,        GL_LINEAR }, TextureInternalFormat::RGB8UI,         "RGB8UI"         },
    { ColorInfo{ 8,  8,  8,  8,  GL_UNSIGNED_INT,        GL_LINEAR }, TextureInternalFormat::RGBA8UI,        "RGBA8UI"        },
    { ColorInfo{ 8,  8,  8,  0,  GL_SIGNED_NORMALIZED,   GL_LINEAR }, TextureInternalFormat::RGB8_SNORM,     "RGB8_SNORM"     },
    { ColorInfo{ 8,  8,  8,  8,  GL_SIGNED_NORMALIZED,   GL_LINEAR }, TextureInternalFormat::RGBA8_SNORM,    "RGBA8_SNORM"    },
    { ColorInfo{ 16, 16, 16, 16, GL_UNSIGNED_NORMALIZED, GL_LINEAR }, TextureInternalFormat::RGBA16,         "RGBA16"         },
    { ColorInfo{ 16, 16, 16, 0,  GL_INT,                 GL_LINEAR }, TextureInternalFormat::RGB16I,         "RGB16I"         },
    { ColorInfo{ 16, 16, 16, 16, GL_INT,                 GL_LINEAR }, TextureInternalFormat::RGBA16I,        "RGBA16I"        },
    { ColorInfo{ 16, 16, 16, 0,  GL_UNSIGNED_INT,        GL_LINEAR }, TextureInternalFormat::RGB16UI,        "RGB16UI"        },
    { ColorInfo{ 16, 16, 16, 16, GL_UNSIGNED_INT,        GL_LINEAR }, TextureInternalFormat::RGBA16UI,       "RGBA16UI"       },
    { ColorInfo{ 16, 16, 16, 0,  GL_FLOAT,               GL_LINEAR }, TextureInternalFormat::RGB16F,         "RGB16F"         },
    { ColorInfo{ 16, 16, 16, 16, GL_FLOAT,               GL_LINEAR }, TextureInternalFormat::RGBA16F,        "RGBA16F"        },
    { ColorInfo{ 32, 32, 32, 0,  GL_INT,                 GL_LINEAR }, TextureInternalFormat::RGB32I,         "RGB32I"         },
    { ColorInfo{ 32, 32, 32, 32, GL_INT,                 GL_LINEAR }, TextureInternalFormat::RGBA32I,        "RGBA32I"        },
    { ColorInfo{ 32, 32, 32, 0,  GL_UNSIGNED_INT,        GL_LINEAR }, TextureInternalFormat::RGB32UI,        "RGB32UI"        },
    { ColorInfo{ 32, 32, 32, 32, GL_UNSIGNED_INT,        GL_LINEAR }, TextureInternalFormat::RGBA32UI,       "RGBA32UI"       },
    { ColorInfo{ 32, 32, 32, 0,  GL_FLOAT,               GL_LINEAR }, TextureInternalFormat::RGB32F,         "RGB32F"         },
    { ColorInfo{ 32, 32, 32, 32, GL_FLOAT,               GL_LINEAR }, TextureInternalFormat::RGBA32F,        "RGBA32F"        },
    { ColorInfo{ 10, 10, 10, 2,  GL_UNSIGNED_NORMALIZED, GL_LINEAR }, TextureInternalFormat::RGB10_A2,       "RGB10_A2"       },
    { ColorInfo{ 10, 10, 10, 2,  GL_UNSIGNED_INT,        GL_LINEAR }, TextureInternalFormat::RGB10_A2UI,     "RGB10_A2UI"     },
    { ColorInfo{ 11, 11, 10, 0,  GL_FLOAT,               GL_LINEAR }, TextureInternalFormat::R11F_G11F_B10F, "R11F_G11F_B10F" }
  }};

#if defined(USE_WEBGL)
  // WebGL requires getting a color attachment for the default framebuffer
  // See: https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getFramebufferAttachmentParameter#attachment
  constexpr FramebufferAttachment attachment = FramebufferAttachment::COLOR0;
#elif defined(USE_OPENGL_ES)
  constexpr FramebufferAttachment attachment = FramebufferAttachment::DEFAULT_BACK;
#else
  constexpr FramebufferAttachment attachment = FramebufferAttachment::DEFAULT_BACK_LEFT;
#endif
  ColorInfo colorInfo;
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::RED_SIZE, &colorInfo.redBitCount);
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::GREEN_SIZE, &colorInfo.greenBitCount);
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::BLUE_SIZE, &colorInfo.blueBitCount);
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::ALPHA_SIZE, &colorInfo.alphaBitCount);
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::COMPONENT_TYPE, &colorInfo.compType);
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::COLOR_ENCODING, &colorInfo.encoding);

  const auto colorFormatIter = std::find_if(formats.cbegin(), formats.cend(), [&colorInfo] (const ColorFormat& format) {
    return (colorInfo.redBitCount == format.colorInfo.redBitCount
         && colorInfo.greenBitCount == format.colorInfo.greenBitCount
         && colorInfo.blueBitCount == format.colorInfo.blueBitCount
         && colorInfo.alphaBitCount == format.colorInfo.alphaBitCount
         && colorInfo.compType == format.colorInfo.compType
         && colorInfo.encoding == format.colorInfo.encoding);
  });

  if (colorFormatIter == formats.cend()) {
    Logger::error("[Renderer] Unknown default framebuffer color bits combination (red " + std::to_string(colorInfo.redBitCount) + ", green "
                + std::to_string(colorInfo.greenBitCount) + ", blue " + std::to_string(colorInfo.blueBitCount) + ", alpha "
                + std::to_string(colorInfo.alphaBitCount) + ", component type " + std::to_string(colorInfo.compType) + ", encoding "
                + std::to_string(colorInfo.encoding) + ')');
    return;
  }

  s_defaultFramebufferColor = colorFormatIter->format;

  Logger::debug("[Renderer] Found default framebuffer color format " + std::string(colorFormatIter->formatStr) + " (value "
              + std::to_string(static_cast<unsigned int>(s_defaultFramebufferColor)) + "; red " + std::to_string(colorInfo.redBitCount)
              + ", green " + std::to_string(colorInfo.greenBitCount) + ", blue " + std::to_string(colorInfo.blueBitCount) + ", alpha "
              + std::to_string(colorInfo.alphaBitCount) + ", component type " + std::to_string(colorInfo.compType) + ", encoding "
              + std::to_string(colorInfo.encoding) + ')');
}

void Renderer::recoverDefaultFramebufferDepthFormat() {
  struct DepthInfo {
    int depthBitCount {};
    int stencilBitCount {};
    int compType {};
  };

  struct DepthFormat {
    DepthInfo depthInfo {};
    TextureInternalFormat format {};
    std::string_view formatStr;
  };

  constexpr std::array<DepthFormat, 6> formats = {{
    { DepthInfo{ 16, 0, GL_UNSIGNED_NORMALIZED }, TextureInternalFormat::DEPTH16,           "DEPTH16"           },
    { DepthInfo{ 24, 0, GL_UNSIGNED_NORMALIZED }, TextureInternalFormat::DEPTH24,           "DEPTH24"           },
    { DepthInfo{ 24, 8, GL_UNSIGNED_NORMALIZED }, TextureInternalFormat::DEPTH24_STENCIL8,  "DEPTH24_STENCIL8"  },
    { DepthInfo{ 32, 0, GL_UNSIGNED_NORMALIZED }, TextureInternalFormat::DEPTH32,           "DEPTH32"           },
    { DepthInfo{ 32, 0, GL_FLOAT               }, TextureInternalFormat::DEPTH32F,          "DEPTH32F"          },
    { DepthInfo{ 32, 8, GL_FLOAT               }, TextureInternalFormat::DEPTH32F_STENCIL8, "DEPTH32F_STENCIL8" }
  }};

#if defined(USE_WEBGL)
  // WebGL requires getting explicitly the depth attachment for the default framebuffer
  // See: https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getFramebufferAttachmentParameter#attachment
  constexpr FramebufferAttachment attachment = FramebufferAttachment::DEPTH;
#else
  constexpr FramebufferAttachment attachment = FramebufferAttachment::DEFAULT_DEPTH;
#endif
  DepthInfo depthInfo {};
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::DEPTH_SIZE, &depthInfo.depthBitCount);
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::STENCIL_SIZE, &depthInfo.stencilBitCount);
  Renderer::recoverFramebufferAttachmentParameter(attachment, FramebufferAttachmentParam::COMPONENT_TYPE, &depthInfo.compType);

  const auto depthFormatIter = std::find_if(formats.cbegin(), formats.cend(), [&depthInfo] (const DepthFormat& format) {
    return (depthInfo.depthBitCount == format.depthInfo.depthBitCount
         && depthInfo.stencilBitCount == format.depthInfo.stencilBitCount
         && depthInfo.compType == format.depthInfo.compType);
  });

  if (depthFormatIter == formats.cend()) {
    Logger::error("[Renderer] Unknown default framebuffer depth bits combination (depth " + std::to_string(depthInfo.depthBitCount)
                + ", stencil " + std::to_string(depthInfo.stencilBitCount) + ", component type: " + std::to_string(depthInfo.compType) + ')');
    return;
  }

  s_defaultFramebufferDepth = depthFormatIter->format;

  Logger::debug("[Renderer] Found default framebuffer depth format " + std::string(depthFormatIter->formatStr) + " (value "
              + std::to_string(static_cast<unsigned int>(s_defaultFramebufferDepth)) + "; depth " + std::to_string(depthInfo.depthBitCount)
              + ", stencil " + std::to_string(depthInfo.stencilBitCount) + ", component type " + std::to_string(depthInfo.compType) + ')');
}

} // namespace Raz
