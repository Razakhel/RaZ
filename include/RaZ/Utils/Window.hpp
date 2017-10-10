#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include "GLFW/glfw3.h"

namespace Raz {

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string& name = "");

  bool run() const;
  void close() const { glfwTerminate(); }

  ~Window() { close(); }

private:
  GLFWwindow* m_window;
};

} // namespace Raz

#endif // RAZ_WINDOW_HPP
