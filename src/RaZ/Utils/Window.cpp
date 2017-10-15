#include <iostream>

#include "GL/glew.h"
#include "RaZ/Utils/Window.hpp"

namespace Raz {

namespace {

void GLAPIENTRY callbackDebugLog(GLenum source,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei length,
                                 const GLchar* message,
                                 const void* userParam) {
  std::cout << "OpenGL Debug - ";

  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "Source:OpenGL\t";
      break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "Source:Windows\t";
      break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "Source:Shader compiler\t";
      break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "Source:Third party\t";
      break;

    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Source:Application\t";
      break;

    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Source:Other\t";
      break;

    default:
      break;
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Type:Error\t";
      break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "Type:Deprecated behavior\t";
      break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "Type:Undefined behavior\t";
      break;

    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Type:Portability\t";
      break;

    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Type:Performance\t";
      break;

    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Type:Other\t";
      break;

    default:
      break;
  }

  std::cout << "ID:" << id << "\t";

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "Severity:High\t";
      break;

    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "Severity:Medium\t";
      break;

    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "Severity:Low\t";
      break;

    default:
      break;
  }

  std::cout << "Message:" << message << std::endl;
}

} // namespace

Window::Window(unsigned int width, unsigned int height, const std::string& name) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  if (!m_window) {
    std::cerr << "Error: Failed to create GLFW Window." << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(m_window);
  glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  });

  glViewport(0, 0, width, height);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
    std::cerr << "Error: Failed to initialize GLEW." << std::endl;

  glDebugMessageCallback(&callbackDebugLog, nullptr);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

bool Window::run() const {
  if (glfwWindowShouldClose(m_window))
    return false;

  glfwSwapBuffers(m_window);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glfwPollEvents();

  return true;
}

} // namespace Raz
