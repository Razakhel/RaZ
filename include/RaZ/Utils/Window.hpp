#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include <vector>
#include <functional>

#include "glfw/include/GLFW/glfw3.h"
#include "RaZ/Utils/Keyboard.hpp"

namespace Raz {

using CallbacksList = std::vector<std::pair<int, std::function<void()>>>;

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string& name = "");

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }

  void enableVerticalSync(bool value = true);
  void disableVerticalSync() { enableVerticalSync(false); }
  void addKeyCallback(Keyboard::Key key, std::function<void()> func);
  void updateKeyCallbacks() const;
  bool run() const;
  void close() const { glfwTerminate(); }

  ~Window() { close(); }

private:
  unsigned int m_width;
  unsigned int m_height;
  CallbacksList m_keyCallbacks;
  GLFWwindow* m_window;
};

} // namespace Raz

#endif // RAZ_WINDOW_HPP
