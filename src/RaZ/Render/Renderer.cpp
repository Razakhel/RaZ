#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"

#include <cassert>
#include <iostream>

namespace Raz {

namespace {

#ifdef RAZ_USE_GL4
inline void GLAPIENTRY callbackDebugLog(GLenum source,
                                        GLenum type,
                                        unsigned int id,
                                        GLenum severity,
                                        int /* length */,
                                        const char* message,
                                        const void* /* userParam */) {
  std::cerr << "OpenGL Debug - ";

  switch (source) {
    case GL_DEBUG_SOURCE_API:             std::cerr << "Source: OpenGL\t"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Windows\t"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader compiler\t"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third party\t"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application\t"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other\t"; break;
    default: break;
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error\t"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated behavior\t"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined behavior\t"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability\t"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance\t"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other\t"; break;
    default: break;
  }

  std::cerr << "ID: " << id << "\t";

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:   std::cerr << "Severity: High\t"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Severity: Medium\t"; break;
    case GL_DEBUG_SEVERITY_LOW:    std::cerr << "Severity: Low\t"; break;
    default: break;
  }

  std::cerr << "Message: " << message << std::endl;
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
#ifdef RAZ_USE_GL4
    case GL_CONTEXT_LOST:                  return "OpenGL context has been lost due to a graphics card reset (Context lost)";
#endif
    case GL_NO_ERROR:                      return "No error";
    default:                               return "Unknown error";
  }
}

} // namespace

void Renderer::initialize() {
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    std::cerr << "Error: Failed to initialize GLEW." << std::endl;
  } else {
    s_isInitialized = true;

#if !defined(RAZ_PLATFORM_MAC) && defined(RAZ_USE_GL4) // Setting the debug message callback provokes a crash on macOS
    glDebugMessageCallback(&callbackDebugLog, nullptr);
    enable(Capability::DEBUG_OUTPUT_SYNCHRONOUS);
#endif
  }
}

void Renderer::enable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glEnable(static_cast<unsigned int>(capability));

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::disable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDisable(static_cast<unsigned int>(capability));

#if !defined(NDEBUG)
  printErrors();
#endif
}

bool Renderer::isEnabled(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isEnabled = (glIsEnabled(static_cast<unsigned int>(capability)) == GL_TRUE);

#if !defined(NDEBUG)
  printErrors();
#endif

  return isEnabled;
}

void Renderer::clearColor(float red, float green, float blue, float alpha) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glClearColor(red, green, blue, alpha);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::clear(MaskType mask) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glClear(static_cast<unsigned int>(mask));

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setDepthFunction(DepthFunction func) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDepthFunc(static_cast<unsigned int>(func));

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setFaceCulling(FaceOrientation orientation) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glCullFace(static_cast<unsigned int>(orientation));

#if !defined(NDEBUG)
  printErrors();
#endif
}

#if !defined(USE_OPENGL_ES)
void Renderer::setPolygonMode(FaceOrientation orientation, PolygonMode mode) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glPolygonMode(static_cast<unsigned int>(orientation), static_cast<unsigned int>(mode));

#if !defined(NDEBUG)
  printErrors();
#endif
}
#endif

void Renderer::recoverFrame(unsigned int width, unsigned int height, TextureFormat format, TextureDataType dataType, void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glReadPixels(0, 0, static_cast<int>(width), static_cast<int>(height), static_cast<unsigned int>(format), static_cast<unsigned int>(dataType), data);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::generateBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenBuffers(count, indices);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::bindBuffer(BufferType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBuffer(static_cast<unsigned int>(type), index);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBufferBase(static_cast<unsigned int>(type), bindingIndex, bufferIndex);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::bindBufferRange(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex, std::ptrdiff_t offset, std::ptrdiff_t size) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBufferRange(static_cast<unsigned int>(type), bindingIndex, bufferIndex, offset, size);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendBufferData(BufferType type, std::ptrdiff_t size, const void* data, BufferDataUsage usage) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBufferData(static_cast<unsigned int>(type), size, data, static_cast<unsigned int>(usage));

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendBufferSubData(BufferType type, std::ptrdiff_t offset, std::ptrdiff_t dataSize, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBufferSubData(static_cast<unsigned int>(type), offset, dataSize, data);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::deleteBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteBuffers(count, indices);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::generateTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenTextures(static_cast<int>(count), indices);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::activateTexture(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glActiveTexture(GL_TEXTURE0 + index);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteri(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterf(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteriv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterfv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

#ifdef RAZ_USE_GL4
void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameteri(textureIndex, static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameterf(textureIndex, static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameteriv(textureIndex, static_cast<unsigned int>(param), values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameterfv(textureIndex, static_cast<unsigned int>(param), values);

#if !defined(NDEBUG)
  printErrors();
#endif
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

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetTexLevelParameteriv(static_cast<unsigned int>(type), static_cast<int>(mipmapLevel), static_cast<unsigned int>(attribute), values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetTexLevelParameterfv(static_cast<unsigned int>(type), static_cast<int>(mipmapLevel), static_cast<unsigned int>(attribute), values);

#if !defined(NDEBUG)
  printErrors();
#endif
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

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::generateMipmap(TextureType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenerateMipmap(static_cast<unsigned int>(type));

#if !defined(NDEBUG)
  printErrors();
#endif
}

#ifdef RAZ_USE_GL4
void Renderer::generateMipmap(unsigned int textureIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenerateTextureMipmap(textureIndex);

#if !defined(NDEBUG)
  printErrors();
#endif
}
#endif

void Renderer::bindTexture(TextureType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindTexture(static_cast<unsigned int>(type), index);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::deleteTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteTextures(static_cast<int>(count), indices);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glViewport(xOrigin, yOrigin, static_cast<int>(width), static_cast<int>(height));

#if !defined(NDEBUG)
  printErrors();
#endif
}

unsigned int Renderer::createProgram() {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int programIndex = glCreateProgram();

#if !defined(NDEBUG)
  printErrors();
#endif

  return programIndex;
}

int Renderer::getProgramStatus(unsigned int index, ProgramStatus status) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int res {};
  glGetProgramiv(index, static_cast<unsigned int>(status), &res);

#if !defined(NDEBUG)
  printErrors();
#endif

  return res;
}

bool Renderer::isProgramLinked(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isLinked = (getProgramStatus(index, ProgramStatus::LINK) == GL_TRUE);

#if !defined(NDEBUG)
  printErrors();
#endif

  return isLinked;
}

void Renderer::linkProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glLinkProgram(index);

  if (!isProgramLinked(index)) {
    char infoLog[512];

    glGetProgramInfoLog(index, static_cast<int>(std::size(infoLog)), nullptr, infoLog);
    std::cerr << "Error: Shader program link failed (ID " << index << ").\n" << infoLog << std::endl;
  }

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::useProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUseProgram(index);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::deleteProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteProgram(index);

#if !defined(NDEBUG)
  printErrors();
#endif
}

unsigned int Renderer::createShader(ShaderType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int shaderIndex = glCreateShader(static_cast<unsigned int>(type));

#if !defined(NDEBUG)
  printErrors();
#endif

  return shaderIndex;
}

int Renderer::getShaderStatus(unsigned int index, ShaderStatus status) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int res {};
  glGetShaderiv(index, static_cast<unsigned int>(status), &res);

#if !defined(NDEBUG)
  printErrors();
#endif

  return res;
}

bool Renderer::isShaderCompiled(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isCompiled = (getShaderStatus(index, ShaderStatus::COMPILE) == GL_TRUE);

#if !defined(NDEBUG)
  printErrors();
#endif

  return isCompiled;
}

void Renderer::sendShaderSource(unsigned int index, const char* source, int length) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glShaderSource(index, 1, &source, &length);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::compileShader(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glCompileShader(index);

  if (!isShaderCompiled(index)) {
    char infoLog[512];

    glGetShaderInfoLog(index, static_cast<int>(std::size(infoLog)), nullptr, infoLog);
    std::cerr << "Error: Shader compilation failed (ID " << index << ").\n" << infoLog << std::endl;
  }

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::attachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glAttachShader(programIndex, shaderIndex);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::detachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDetachShader(programIndex, shaderIndex);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::deleteShader(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteShader(index);

#if !defined(NDEBUG)
  printErrors();
#endif
}

int Renderer::recoverUniformLocation(unsigned int programIndex, const char* uniformName) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const int location = glGetUniformLocation(programIndex, uniformName);

#if !defined(NDEBUG)
  printErrors();

  if (location == -1)
    std::cerr << "Warning: Uniform '" << uniformName << "' unrecognized." << std::endl;
#endif

  return location;
}

void Renderer::sendUniform(int uniformIndex, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1i(uniformIndex, value);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniform(int uniformIndex, unsigned int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1ui(uniformIndex, value);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniform(int uniformIndex, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1f(uniformIndex, value);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniformVector1(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1fv(uniformIndex, count, values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniformVector2(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform2fv(uniformIndex, count, values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniformVector3(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform3fv(uniformIndex, count, values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniformVector4(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform4fv(uniformIndex, count, values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniformMatrix2x2(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix2fv(uniformIndex, count, transpose, values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniformMatrix3x3(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix3fv(uniformIndex, count, transpose, values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::sendUniformMatrix4x4(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix4fv(uniformIndex, count, transpose, values);

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::generateFramebuffers(int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenFramebuffers(count, indices);

#if !defined(NDEBUG)
  printErrors();
#endif
}

FramebufferStatus Renderer::getFramebufferStatus(FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int status = glCheckFramebufferStatus(static_cast<unsigned int>(type));

#if !defined(NDEBUG)
  printErrors();
#endif

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

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::setDrawBuffers(unsigned int count, const DrawBuffer* buffers) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDrawBuffers(count, reinterpret_cast<const unsigned int*>(buffers));

#if !defined(NDEBUG)
  printErrors();
#endif
}

void Renderer::bindFramebuffer(unsigned int index, FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindFramebuffer(static_cast<unsigned int>(type), index);

#if !defined(NDEBUG)
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes[recoverErrorCodeIndex(ErrorCode::INVALID_OPERATION)])
    std::cerr << "Renderer::bindFramebuffer - Bound object is not a valid framebuffer.\n";

  std::cerr << std::flush;
#endif
}

void Renderer::deleteFramebuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteFramebuffers(count, indices);

#if !defined(NDEBUG)
  printErrors();
#endif
}

ErrorCodes Renderer::recoverErrors() {
  ErrorCodes errorCodes;

  while (true) {
    const unsigned int errorCode = glGetError();

    if (errorCode == GL_NO_ERROR)
      break;

    const uint8_t errorCodeIndex = recoverErrorCodeIndex(static_cast<ErrorCode>(errorCode));

    // An error code cannot be returned twice in a row; if it is, the error checking should be stopped
    if (errorCodes[errorCodeIndex])
      break;

    errorCodes[errorCodeIndex] = true;
  }

  return errorCodes;
}

void Renderer::printErrors() {
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes.none())
    return;

  for (uint8_t errorIndex = 0; errorIndex < static_cast<uint8_t>(errorCodes.size()); ++errorIndex) {
    if (errorCodes[errorIndex]) {
      const unsigned int errorValue = errorIndex + static_cast<unsigned int>(ErrorCode::INVALID_ENUM);
      std::cerr << "OpenGL error - " << recoverGlErrorStr(errorValue) << " (code " << errorValue << ")\n";
    }
  }

  std::cerr << std::flush;
}

} // namespace Raz
