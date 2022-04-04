#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <algorithm>
#include <array>
#include <cassert>

namespace Raz {

namespace {

#if !defined(USE_OPENGL_ES)
inline void GLAPIENTRY callbackDebugLog(GLenum source,
                                        GLenum type,
                                        unsigned int id,
                                        GLenum severity,
                                        int /* length */,
                                        const char* message,
                                        const void* /* userParam */) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

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
  if (s_isInitialized)
    return;

  Logger::debug("[Renderer] Initializing...");

  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    Logger::error("Failed to initialize GLEW.");
    return;
  }

  s_isInitialized = true;

  getParameter(StateParameter::MAJOR_VERSION, &s_majorVersion);
  getParameter(StateParameter::MINOR_VERSION, &s_minorVersion);

#if !defined(RAZ_PLATFORM_MAC) && !defined(USE_OPENGL_ES) // Setting the debug message callback provokes a crash on macOS & isn't available on OpenGL ES
  if (checkVersion(4, 3)) {
    enable(Capability::DEBUG_OUTPUT);
    enable(Capability::DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&callbackDebugLog, nullptr);
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

  glClear(static_cast<unsigned int>(mask));

  printConditionalErrors();
}

void Renderer::setDepthFunction(DepthStencilFunction func) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

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

void Renderer::setPatchVertexCount(int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Setting patch vertices requires OpenGL 4.0+." && checkVersion(4, 0));
  assert("Error: A patch needs at least one vertex." && value > 0);

  glPatchParameteri(GL_PATCH_VERTICES, value);

  printConditionalErrors();
}

void Renderer::setPatchParameter(PatchParameter param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: Setting a patch parameter requires OpenGL 4.0+." && checkVersion(4, 0));

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
    Logger::error("Renderer::setPixelStorage - " + std::to_string(value) + " is not a valid alignment value. Only 1, 2, 4 & 8 are accepted.");
#endif
}

void Renderer::recoverFrame(unsigned int width, unsigned int height, TextureFormat format, TextureDataType dataType, void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

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

void Renderer::deleteVertexArrays(unsigned int count, unsigned int* indices) {
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

void Renderer::deleteBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteBuffers(static_cast<int>(count), indices);

  printConditionalErrors();
}

bool Renderer::isTexture(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isTexture = (glIsTexture(index) == GL_TRUE);

  printConditionalErrors();

  return isTexture;
}

void Renderer::generateTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenTextures(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::bindTexture(TextureType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindTexture(static_cast<unsigned int>(type), index);

  printConditionalErrors();
}

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

void Renderer::activateTexture(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glActiveTexture(GL_TEXTURE0 + index);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteri(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterf(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteriv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterfv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameteri(textureIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameterf(textureIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameteriv(textureIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to set a parameter with a texture index." && checkVersion(4, 5));

  glTextureParameterfv(textureIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}
#endif

void Renderer::sendImageData2D(TextureType type,
                               unsigned int mipmapLevel,
                               TextureInternalFormat internalFormat,
                               unsigned int width, unsigned int height,
                               TextureFormat format,
                               TextureDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

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

void Renderer::recoverTextureData(TextureType type, unsigned int mipmapLevel, TextureFormat format, TextureDataType dataType, void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

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

  glGenerateMipmap(static_cast<unsigned int>(type));

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::generateMipmap(unsigned int textureIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
  assert("Error: OpenGL 4.5+ is needed to generate mipmap with a texture index." && checkVersion(4, 5));

  glGenerateTextureMipmap(textureIndex);

  printConditionalErrors();
}
#endif

void Renderer::deleteTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteTextures(static_cast<int>(count), indices);

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

  glUseProgram(index);

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = Renderer::recoverErrors();

  if (errorCodes[ErrorCode::INVALID_VALUE])
    Logger::error("Renderer::useProgram - Invalid shader program index (" + std::to_string(index) + ").");

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
  assert("Error: Creating a tessellation shader requires OpenGL 4.0+."
         && ((type != ShaderType::TESSELLATION_CONTROL && type != ShaderType::TESSELLATION_EVALUATION) || checkVersion(4, 0)));
  assert("Error: Creating a compute shader requires OpenGL 4.3+." && (type != ShaderType::COMPUTE || (checkVersion(4, 3))));
#else
  assert("Error: Geometry shaders are unsupported with OpenGL ES." && type != ShaderType::GEOMETRY);
  assert("Error: Tessellation shaders are unsupported with OpenGL ES."
         && type != ShaderType::TESSELLATION_CONTROL && type != ShaderType::TESSELLATION_EVALUATION);
  assert("Error: Creating a compute shader requires OpenGL ES 3.1+." && (type != ShaderType::COMPUTE || (checkVersion(3, 1))));
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

  glShaderSource(index, 1, &source, &length);

  printConditionalErrors();
}

std::string Renderer::recoverShaderSource(unsigned int index) {
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

void Renderer::dispatchCompute(unsigned int groupCountX, unsigned int groupCountY, unsigned int groupCountZ) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());
#if !defined(USE_OPENGL_ES)
  assert("Error: Launching a compute operation requires OpenGL 4.3+." && checkVersion(4, 3));
#else
  assert("Error: Launching a compute operation requires OpenGL ES 3.1+." && checkVersion(3, 1));
#endif

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

  glMemoryBarrierByRegion(static_cast<unsigned int>(type));

  printConditionalErrors();
}

int Renderer::recoverUniformLocation(unsigned int programIndex, const char* uniformName) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const int location = glGetUniformLocation(programIndex, uniformName);

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  printErrors();

  if (location == -1)
    Logger::warn("Uniform '" + std::string(uniformName) + "' unrecognized.");
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
    Logger::error("Renderer::recoverUniformInfo - Tried to fetch program information from a non-program object.");

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
    Logger::error("Renderer::bindFramebuffer - Bound object is not a valid framebuffer.");
#endif
}

FramebufferStatus Renderer::getFramebufferStatus(FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int status = glCheckFramebufferStatus(static_cast<unsigned int>(type));

  printConditionalErrors();

  return static_cast<FramebufferStatus>(status);
}

void Renderer::setFramebufferTexture2D(FramebufferAttachment attachment,
                                       TextureType textureType, unsigned int textureIndex, int mipmapLevel,
                                       FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glFramebufferTexture2D(static_cast<unsigned int>(type),
                         static_cast<unsigned int>(attachment),
                         static_cast<unsigned int>(textureType),
                         textureIndex,
                         mipmapLevel);

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

  glBlitFramebuffer(readMinX, readMinY, readMaxX, readMaxY,
                    writeMinX, writeMinY, writeMaxX, writeMaxY,
                    static_cast<unsigned int>(mask), static_cast<unsigned int>(filter));

  printConditionalErrors();
}

void Renderer::deleteFramebuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteFramebuffers(static_cast<int>(count), indices);

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

} // namespace Raz
