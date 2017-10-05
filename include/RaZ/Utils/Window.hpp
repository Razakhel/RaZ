#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include "GLFW/glfw3.h"

namespace Raz {

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string& name = "");

  int shouldClose() { return glfwWindowShouldClose(window); }

  void pollEvents() { glfwPollEvents(); }
  void swapBuffers() { glfwSwapBuffers(window); }
  void close() { glfwTerminate(); }

  ~Window() { close(); }

private:
  GLFWwindow* window;
};

} // namespace Raz

#endif // RAZ_WINDOW_HPP
