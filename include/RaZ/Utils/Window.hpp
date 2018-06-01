#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include <functional>
#include <memory>
#include <vector>

#include "glfw/include/GLFW/glfw3.h"
#include "RaZ/Utils/Keyboard.hpp"

namespace Raz {

using CallbacksList = std::vector<std::pair<int, std::function<void()>>>;

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string& title = "", uint8_t AASampleCount = 1);

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }

  void setTitle(const std::string& title) const { glfwSetWindowTitle(m_window, title.c_str()); }
  void enableVerticalSync(bool value = true) const;
  void disableVerticalSync() const { enableVerticalSync(false); }
  void addKeyCallback(Keyboard::Key key, std::function<void()> func);
  void updateKeyCallbacks() const;
  bool run() const;
  void close() const { glfwTerminate(); }

  ~Window() { close(); }

private:
  unsigned int m_width {};
  unsigned int m_height {};
  CallbacksList m_keyCallbacks {};
  GLFWwindow* m_window {};
};

using WindowPtr = std::unique_ptr<Window>;

} // namespace Raz

#endif // RAZ_WINDOW_HPP
