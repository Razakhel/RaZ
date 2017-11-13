#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include <vector>
#include <functional>

#include "GLFW/glfw3.h"

namespace Raz {

using CallbacksList = std::vector<std::pair<int, std::function<void()>>>;

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string& name = "");

  void addKeyCallback(int key, std::function<void()> func);
  void updateKeyCallback();
  bool run() const;
  void close() const { glfwTerminate(); }

  ~Window() { close(); }

private:
  CallbacksList m_keyCallbacks;
  GLFWwindow* m_window;
};

} // namespace Raz

#endif // RAZ_WINDOW_HPP
