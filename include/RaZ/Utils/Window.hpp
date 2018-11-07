#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "glew/include/GL/glew.h"
#if defined(_WIN32)
#if defined(_MSC_VER)
#define NOMINMAX
#endif
#include "glew/include/GL/wglew.h"
#elif defined(__gnu_linux__)
#include "glew/include/GL/glxew.h"
#endif
#include "glfw/include/GLFW/glfw3.h"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/Overlay.hpp"
#include "RaZ/Utils/Input.hpp"

namespace Raz {

class Window;
using WindowPtr = std::unique_ptr<Window>;

using KeyboardCallbacks    = std::vector<std::tuple<int, std::function<void(float)>, Input::ActionTrigger, std::function<void()>>>;
using MouseButtonCallbacks = std::vector<std::tuple<int, std::function<void(float)>, Input::ActionTrigger, std::function<void()>>>;
using MouseScrollCallback  = std::function<void(double, double)>;
using MouseMoveCallback    = std::tuple<double, double, std::function<void(double, double)>>;
using InputActions         = std::unordered_map<int, std::pair<std::function<void(float)>, Input::ActionTrigger>>;
using InputCallbacks       = std::tuple<KeyboardCallbacks, MouseButtonCallbacks, MouseScrollCallback, MouseMoveCallback, InputActions>;

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string& title = "", uint8_t AASampleCount = 1);

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  const Vec4f& getClearColor() const { return m_clearColor; }

  void setClearColor(const Vec4f& clearColor) { m_clearColor = clearColor; }
  void setClearColor(float red, float green, float blue, float alpha = 1.f) { setClearColor(Vec4f({ red, green, blue, alpha })); }
  void setTitle(const std::string& title) const { glfwSetWindowTitle(m_window, title.c_str()); }
  void setIcon(const Image& img) const;
  void setIcon(const std::string& fileName) const { setIcon(Image(fileName, true)); }

  template <typename... Args> static WindowPtr create(Args&&... args) { return std::make_unique<Window>(std::forward<Args>(args)...); }

  void enableFaceCulling(bool value = true) const;
  void disableFaceCulling() const { enableFaceCulling(false); }
  bool recoverVerticalSyncState() const;
  void enableVerticalSync(bool value = true) const;
  void disableVerticalSync() const { enableVerticalSync(false); }
  void changeCursorState(Cursor::State state) const { glfwSetInputMode(m_window, GLFW_CURSOR, state); }
  void showCursor() const { changeCursorState(Cursor::State::NORMAL); }
  void hideCursor() const { changeCursorState(Cursor::State::HIDDEN); }
  void disableCursor() const { changeCursorState(Cursor::State::DISABLED); }
  void addKeyCallback(Keyboard::Key key, std::function<void(float)> actionPress,
                                         Input::ActionTrigger frequency = Input::ALWAYS,
                                         std::function<void()> actionRelease = nullptr);
  void addMouseButtonCallback(Mouse::Button button, std::function<void(float)> actionPress,
                                                    Input::ActionTrigger frequency = Input::ALWAYS,
                                                    std::function<void()> actionRelease = nullptr);
  void addMouseScrollCallback(std::function<void(double, double)> func);
  void addMouseMoveCallback(std::function<void(double, double)> func);
  void updateCallbacks() const;
  void enableOverlay() { m_overlay = Overlay::create(m_window); }
  void disableOverlay() { m_overlay.reset(); }
  void addOverlayElement(OverlayElementType type, const std::string& text,
                         std::function<void()> actionOn = nullptr, std::function<void()> actionOff = nullptr);
  void addOverlayText(const std::string& text);
  void addOverlayButton(const std::string& text, std::function<void()> action);
  void addOverlayCheckbox(const std::string& text, bool initVal, std::function<void()> actionOn, std::function<void()> actionOff);
  void addOverlaySeparator();
  void addOverlayFrameTime(const std::string& formattedText);
  void addOverlayFpsCounter(const std::string& formattedText);
  bool run(float deltaTime);
  Vec2f recoverMousePosition() const;
  void setShouldClose() const { glfwSetWindowShouldClose(m_window, true); }
  void close();

  ~Window() { close(); }

private:
  unsigned int m_width {};
  unsigned int m_height {};
  Vec4f m_clearColor = Vec4f({ 0.15f, 0.15f, 0.15f, 1.f });
  GLFWwindow* m_window {};
  InputCallbacks m_callbacks {};
  OverlayPtr m_overlay {};
};

} // namespace Raz

#endif // RAZ_WINDOW_HPP
