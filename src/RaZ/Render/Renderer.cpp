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

} // namespace Raz
