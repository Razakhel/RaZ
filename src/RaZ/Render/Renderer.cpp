#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"

#include <cassert>
#include <iostream>

namespace Raz {

namespace {

inline constexpr const char* recoverGlErrorStr(unsigned int errorCode) {
  switch (errorCode) {
    case GL_INVALID_ENUM:                  return "Unrecognized error code (Invalid enum)";
    case GL_INVALID_VALUE:                 return "Numeric argument out of range (Invalid value)";
    case GL_INVALID_OPERATION:             return "Operation illegal in current state (Invalid operation)";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "Framebuffer object is incomplete (Invalid framebuffer operation)";
    case GL_STACK_OVERFLOW:                return "Stack overflow";
    case GL_STACK_UNDERFLOW:               return "Stack underflow";
    case GL_OUT_OF_MEMORY:                 return "Not enough memory left (Out of memory)";
    case GL_NO_ERROR:                      return "No error";
    default:                               return "Unknown error";
  }
}

} // namespace

void Renderer::initialize() {
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK)
    std::cerr << "Error: Failed to initialize GLEW." << std::endl;
  else
    s_isInitialized = true;
}

void Renderer::enable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glEnable(static_cast<unsigned int>(capability));

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::disable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDisable(static_cast<unsigned int>(capability));

#if !defined(NDEBUG)
  checkErrors();
#endif
}

bool Renderer::isEnabled(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isEnabled = glIsEnabled(static_cast<unsigned int>(capability));

#if !defined(NDEBUG)
  checkErrors();
#endif

  return isEnabled;
}

void Renderer::generateBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenBuffers(count, indices);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::bindBuffer(BufferType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBuffer(static_cast<unsigned int>(type), index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBufferBase(static_cast<unsigned int>(type), bindingIndex, bufferIndex);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::sendBufferSubData(BufferType type, ptrdiff_t offset, ptrdiff_t dataSize, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBufferSubData(static_cast<unsigned int>(type), offset, dataSize, data);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::deleteBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteBuffers(count, indices);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::generateTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenTextures(static_cast<int>(count), indices);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::activateTexture(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glActiveTexture(GL_TEXTURE0 + index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteri(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterf(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const int* value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteriv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const float* value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterfv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

#ifdef RAZ_USE_GL4
void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameteri(textureIndex, static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameterf(textureIndex, static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const int* value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameteriv(textureIndex, static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const float* value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameterfv(textureIndex, static_cast<unsigned int>(param), value);

#if !defined(NDEBUG)
  checkErrors();
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
  checkErrors();
#endif
}

void Renderer::generateMipmap(TextureType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenerateMipmap(static_cast<unsigned int>(type));

#if !defined(NDEBUG)
  checkErrors();
#endif
}

#ifdef RAZ_USE_GL4
void Renderer::generateMipmap(unsigned int textureIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenerateTextureMipmap(textureIndex);

#if !defined(NDEBUG)
  checkErrors();
#endif
}
#endif

void Renderer::bindTexture(TextureType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindTexture(static_cast<unsigned int>(type), index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::deleteTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteTextures(static_cast<int>(count), indices);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glViewport(xOrigin, yOrigin, static_cast<int>(width), static_cast<int>(height));

#if !defined(NDEBUG)
  checkErrors();
#endif
}

unsigned int Renderer::createProgram() {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int programIndex = glCreateProgram();

#if !defined(NDEBUG)
  checkErrors();
#endif

  return programIndex;
}

int Renderer::getProgramStatus(unsigned int index, ProgramStatus status) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int res;
  glGetProgramiv(index, static_cast<unsigned int>(status), &res);

#if !defined(NDEBUG)
  checkErrors();
#endif

  return res;
}

bool Renderer::isProgramLinked(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isLinked = getProgramStatus(index, ProgramStatus::LINK);

#if !defined(NDEBUG)
  checkErrors();
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
  checkErrors();
#endif
}

void Renderer::useProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUseProgram(index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::deleteProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteProgram(index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

unsigned int Renderer::createShader(ShaderType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int shaderIndex = glCreateShader(static_cast<unsigned int>(type));

#if !defined(NDEBUG)
  checkErrors();
#endif

  return shaderIndex;
}

int Renderer::getShaderStatus(unsigned int index, ShaderStatus status) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int res;
  glGetShaderiv(index, static_cast<unsigned int>(status), &res);

#if !defined(NDEBUG)
  checkErrors();
#endif

  return res;
}

bool Renderer::isShaderCompiled(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isCompiled = getShaderStatus(index, ShaderStatus::COMPILE);

#if !defined(NDEBUG)
  checkErrors();
#endif

  return isCompiled;
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
  checkErrors();
#endif
}

void Renderer::attachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glAttachShader(programIndex, shaderIndex);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::detachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDetachShader(programIndex, shaderIndex);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::deleteShader(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteShader(index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

int Renderer::recoverUniformLocation(unsigned int programIndex, const char* uniformName) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const int location = glGetUniformLocation(programIndex, uniformName);

#if !defined(NDEBUG)
  checkErrors();

  if (location == -1)
    std::cerr << "Warning: Uniform '" << uniformName << "' unrecognized." << std::endl;
#endif

  return location;
}

void Renderer::generateFramebuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenFramebuffers(count, indices);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::bindFramebuffer(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindFramebuffer(GL_FRAMEBUFFER, index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::deleteFramebuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteFramebuffers(count, indices);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::checkErrors() {
  while (true) {
    const unsigned int errorCode = glGetError();

    if (errorCode == GL_NO_ERROR)
      break;

    std::cerr << "OpenGL error - " << recoverGlErrorStr(errorCode) << " (code " << errorCode << ")\n";
  };

  std::cerr << std::flush;
}

void Renderer::clear(unsigned int mask) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glClear(mask);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

} // namespace Raz
