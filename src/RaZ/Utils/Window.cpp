#include <iostream>

#include "GL/glew.h"
#include "RaZ/Utils/Window.hpp"

namespace Raz {

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
}

} // namespace Raz
