#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "GL/glew.h"
#if defined(_WIN32)
#if defined(_MSC_VER)
#define NOMINMAX
#endif
#include "GL/wglew.h"
#elif defined(__gnu_linux__)
#include "GL/glxew.h"
#endif
#include "GLFW/glfw3.h"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/Overlay.hpp"
#include "RaZ/Utils/Input.hpp"

namespace Raz {

using KeyboardCallbacks    = std::vector<std::tuple<int, std::function<void(float)>, Input::ActionTrigger, std::function<void()>>>;
using MouseButtonCallbacks = std::vector<std::tuple<int, std::function<void(float)>, Input::ActionTrigger, std::function<void()>>>;
using MouseScrollCallback  = std::function<void(double, double)>;
using MouseMoveCallback    = std::tuple<double, double, std::function<void(double, double)>>;
using InputActions         = std::unordered_map<int, std::pair<std::function<void(float)>, Input::ActionTrigger>>;
using InputCallbacks       = std::tuple<KeyboardCallbacks, MouseButtonCallbacks, MouseScrollCallback, MouseMoveCallback, InputActions>;

/// Graphical window to render the scenes on, with input custom actions.
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

  /// Changes the face culling's state.
  /// Enables or disables face culling according to the given parameter.
  /// \param value Value to apply.
  void enableFaceCulling(bool value = true) const;
  /// Disables the face culling.
  void disableFaceCulling() const { enableFaceCulling(false); }
  /// Fetches the current vertical synchronization's state.
  /// \return True if vertical sync is enabled, false otherwise.
  bool recoverVerticalSyncState() const;
  /// Changes the vertical synchronization's state.
  /// Enables or disables vertical sync according to the given parameter.
  /// \param value Value to apply.
  void enableVerticalSync(bool value = true) const;
  /// Disables vertical synchronization.
  void disableVerticalSync() const { enableVerticalSync(false); }
  /// Changes the cursor's state.
  /// Defines the new behavior of the mouse's cursor, if it should be shown, hidden or disabled.
  /// The functions showCursor(), hideCursor() & disableCursor() can be used instead.
  /// \param state State to apply.
  void changeCursorState(Cursor::State state) const { glfwSetInputMode(m_window, GLFW_CURSOR, state); }
  /// Shows the mouse cursor.
  /// Default behavior.
  void showCursor() const { changeCursorState(Cursor::State::NORMAL); }
  /// Hides the mouse cursor.
  /// The cursor becomes invisible while being inside the window's frame. It can go out of the window.
  void hideCursor() const { changeCursorState(Cursor::State::HIDDEN); }
  /// Disables the mouse cursor.
  /// The cursor always goes back to the window's center and becomes totally invisible. It can't go out of the window.
  void disableCursor() const { changeCursorState(Cursor::State::DISABLED); }
  /// Defines an action on keyboard's key press & release.
  /// \param key Key triggering the given action(s).
  /// \param actionPress Action to be executed when the given key is pressed.
  /// \param frequency Frequency at which to execute the actions.
  /// \param actionRelease Action to be executed when the given key is released.
  void addKeyCallback(Keyboard::Key key, std::function<void(float)> actionPress,
                                         Input::ActionTrigger frequency = Input::ALWAYS,
                                         std::function<void()> actionRelease = nullptr);
  /// Defines an action on mouse button click or release.
  /// \param button Button triggering the given action(s).
  /// \param actionPress Action to be executed when the given mouse button is pressed.
  /// \param frequency Frequency at which to execute the actions.
  /// \param actionRelease Action to be executed when the given mouse button is released.
  void addMouseButtonCallback(Mouse::Button button, std::function<void(float)> actionPress,
                                                    Input::ActionTrigger frequency = Input::ALWAYS,
                                                    std::function<void()> actionRelease = nullptr);
  /// Defines an action on mouse wheel scroll.
  /// \param func Action to be executed when scrolling.
  void addMouseScrollCallback(std::function<void(double, double)> func);
  /// Defines an action on mouse move.
  /// \param func Action to be executed when the mouse is moved.
  void addMouseMoveCallback(std::function<void(double, double)> func);
  /// Associates all of the callbacks, making them active.
  void updateCallbacks() const;
  /// Enables the overlay.
  void enableOverlay() { m_overlay = Overlay::create(m_window); }
  /// Disables the overlay.
  void disableOverlay() { m_overlay.reset(); }
  /// Adds an element on the overlay.
  /// \param type Type of the element to add.
  /// \param text Text to be displayed beside the element.
  /// \param actionOn Action to be executed when clicked or toggled on.
  /// \param actionOff Action to be executed when toggled off.
  void addOverlayElement(OverlayElementType type, const std::string& text,
                         std::function<void()> actionOn = nullptr, std::function<void()> actionOff = nullptr);
  /// Adds text on the overlay.
  /// \param text Text to be displayed.
  void addOverlayText(const std::string& text);
  /// Adds a button on the overlay.
  /// \param text Text to be displayed beside the button.
  /// \param action Action to be executed when clicked.
  void addOverlayButton(const std::string& text, std::function<void()> action);
  /// Adds a checkbox on the overlay.
  /// \param text Text to be displayed beside the checkbox.
  /// \param initVal Initial value, checked or not.
  /// \param actionOn Action to be executed when toggled on.
  /// \param actionOff Action to be executed when toggled off.
  void addOverlayCheckbox(const std::string& text, bool initVal, std::function<void()> actionOn, std::function<void()> actionOff);
  /// Adds an horizontal separator on the overlay.
  void addOverlaySeparator();
  /// Adds a frame time display on the overlay.
  /// \param formattedText Text with a formatting placeholder to display the frame time (%.Xf, X being the precision after the comma).
  void addOverlayFrameTime(const std::string& formattedText);
  /// Adds a FPS (frames per second) counter on the overlay.
  /// \param formattedText Text with a formatting placeholder to display the FPS (%.Xf, X being the precision after the comma).
  void addOverlayFpsCounter(const std::string& formattedText);
  /// Runs the window, refreshing its state by displaying the rendered scene, drawing the overlay, etc.
  /// \param deltaTime Amount of time elapsed since the last frame.
  /// \return True if the window hasn't been required to close, false otherwise.
  bool run(float deltaTime);
  /// Fetches the mouse position onto the window.
  /// \return 2D vector representing the mouse's position relative to the window.
  Vec2f recoverMousePosition() const;
  /// Tells the window that it should close.
  void setShouldClose() const { glfwSetWindowShouldClose(m_window, true); }
  /// Closes the window.
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
