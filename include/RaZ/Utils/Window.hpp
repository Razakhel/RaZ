#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include <functional>
#include <memory>
#include <vector>

#include "glew/include/GL/glew.h"
#if defined(_WIN32)
#include "glew/include/GL/wglew.h"
#elif defined(__gnu_linux__)
#include "glew/include/GL/glxew.h"
#endif
#include "glfw/include/GLFW/glfw3.h"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/Overlay.hpp"
#include "RaZ/Utils/Input.hpp"

namespace Raz {

using KeyboardCallbacks    = std::vector<std::pair<int, std::function<void()>>>;
using MouseButtonCallbacks = std::vector<std::pair<int, std::function<void()>>>;
using MouseScrollCallback  = std::function<void(double, double)>;
using InputCallbacks       = std::tuple<KeyboardCallbacks, MouseButtonCallbacks, MouseScrollCallback>;

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string& title = "", uint8_t AASampleCount = 1);

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }

  void setTitle(const std::string& title) const { glfwSetWindowTitle(m_window, title.c_str()); }
  void setIcon(const Image& img) const;
  void setIcon(const std::string& fileName) const { setIcon(Image(fileName, true)); }
  void enableFaceCulling(bool value = true) const;
  void disableFaceCulling() const { enableFaceCulling(false); }
  bool recoverVerticalSyncState() const;
  void enableVerticalSync(bool value = true) const;
  void disableVerticalSync() const { enableVerticalSync(false); }
  void changeCursorState(Cursor::State state) const { glfwSetInputMode(m_window, GLFW_CURSOR, state); }
  void showCursor() const { changeCursorState(Cursor::State::NORMAL); }
  void hideCursor() const { changeCursorState(Cursor::State::HIDDEN); }
  void disableCursor() const { changeCursorState(Cursor::State::DISABLED); }
  void addKeyCallback(Keyboard::Key key, std::function<void()> func);
  void addMouseButtonCallback(Mouse::Button button, std::function<void()> func);
  void addMouseScrollCallback(std::function<void(double, double)> func);
  void updateCallbacks() const;
  void enableOverlay() { m_overlay = std::make_unique<Overlay>(m_window); }
  void disableOverlay() { m_overlay.reset(); }
  void addOverlayElement(OverlayElementType type, const std::string& text,
                         std::function<void()> actionOn = nullptr, std::function<void()> actionOff = nullptr);
  void addOverlayText(const std::string& text);
  void addOverlayButton(const std::string& text, std::function<void()> action);
  void addOverlayCheckbox(const std::string& text, bool initVal, std::function<void()> actionOn, std::function<void()> actionOff);
  void addOverlayFrameTime(const std::string& formattedText);
  void addOverlayFpsCounter(const std::string& formattedText);
  bool run() const;
  void close();

  ~Window() { close(); }

private:
  unsigned int m_width {};
  unsigned int m_height {};
  GLFWwindow* m_window {};
  InputCallbacks m_callbacks {};
  OverlayPtr m_overlay {};
};

using WindowPtr = std::unique_ptr<Window>;

} // namespace Raz

#endif // RAZ_WINDOW_HPP
