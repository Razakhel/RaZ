#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

namespace {

inline constexpr const char* recoverGlErrorStr(unsigned int errorCode) {
  switch (errorCode) {
    case GL_INVALID_ENUM:      return "Unrecognized error code (Invalid enum)";
    case GL_INVALID_VALUE:     return "Numeric argument out of range (Invalid value)";
    case GL_INVALID_OPERATION: return "Operation illegal in current state (Invalid operation)";
    case GL_STACK_OVERFLOW:    return "Stack overflow";
    case GL_STACK_UNDERFLOW:   return "Stack underflow";
    case GL_OUT_OF_MEMORY:     return "Not enough memory left (Out of memory)";
    case GL_NO_ERROR:          return "No error";
    default:                   return "Unknown error";
  }
}

} // namespace

void Renderer::checkErrors() {
  while (true) {
    const unsigned int errorCode = glGetError();

    if (errorCode == GL_NO_ERROR)
      break;

    std::cerr << "OpenGL error - " << recoverGlErrorStr(errorCode) << " (code " << errorCode << ")\n";
  };

  std::cerr << std::flush;
}

void Renderer::enable(unsigned int code) {
  glEnable(code);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::disable(unsigned int code) {
  glDisable(code);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::bindBuffer(unsigned int type, unsigned int index) {
  glBindBuffer(type, index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::bindTexture(unsigned int type, unsigned int index) {
  glBindTexture(type, index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height) {
  glViewport(xOrigin, yOrigin, width, height);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::generateBuffers(unsigned int count, unsigned int* index) {
  glGenBuffers(count, index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::deleteBuffers(unsigned int count, unsigned int* index) {
  glDeleteBuffers(count, index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

unsigned int Renderer::createShader(ShaderType type) {
  const unsigned int shaderIndex = glCreateShader(static_cast<unsigned int>(type));

#if !defined(NDEBUG)
  checkErrors();
#endif

  return shaderIndex;
}

int Renderer::getProgramInfo(unsigned int index, unsigned int infoType) {
  int res;
  glGetProgramiv(index, infoType, &res);

#if !defined(NDEBUG)
  checkErrors();
#endif

  return res;
}

bool Renderer::isProgramLinked(unsigned int index) {
  const bool isLinked = getProgramInfo(index, GL_LINK_STATUS);

#if !defined(NDEBUG)
  checkErrors();
#endif

  return isLinked;
}

void Renderer::linkProgram(unsigned int index) {
  glLinkProgram(index);

  if (!isProgramLinked(index)) {
    std::array<char, 512> infoLog {};

    glGetProgramInfoLog(index, static_cast<int>(infoLog.size()), nullptr, infoLog.data());
    std::cerr << "Error: Shader program link failed (ID " << index << ").\n" << infoLog.data() << std::endl;
  }

#if !defined(NDEBUG)
  checkErrors();
#endif
}

void Renderer::useProgram(unsigned int index) {
  glUseProgram(index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

int Renderer::recoverUniformLocation(unsigned int programIndex, const char* uniformName) {
  const int location = glGetUniformLocation(programIndex, uniformName);

#if !defined(NDEBUG)
  checkErrors();

  if (location == -1)
    std::cerr << "Warning: Uniform '" << uniformName << "' unrecognized." << std::endl;
#endif

  return location;
}

void Renderer::activateTexture(unsigned int index) {
  glActiveTexture(GL_TEXTURE0 + index);

#if !defined(NDEBUG)
  checkErrors();
#endif
}

} // namespace Raz
