#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include "RaZ/Data/OwnerValue.hpp"
#include "RaZ/Render/Overlay.hpp"
#include "RaZ/Utils/EnumUtils.hpp"
#include "RaZ/Utils/Input.hpp"

#include <functional>
#include <unordered_map>
#include <vector>

namespace Raz {

class Image;
class RenderSystem;
class Window;
using WindowPtr = std::unique_ptr<Window>;

enum class WindowSetting : unsigned int {
  FOCUSED        = 1,   ///< Forces the window to take the focus.
  RESIZABLE      = 2,   ///< Makes the window able to be resized, either by dragging the edges & corners or by maximizing it.
  VISIBLE        = 4,   ///< Makes the window visible.
  DECORATED      = 8,   ///< Defines if there are borders, minimize/maximize/close buttons, etc.
  AUTO_MINIMIZE  = 16,  ///< Automatically minimizes the full-screen window on focus loss.
  ALWAYS_ON_TOP  = 32,  ///< Forces the window to be floating, on top of everything on the screen.
  MAXIMIZED      = 64,  ///< Maximizes the window, taking the whole screen space.
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  CENTER_CURSOR  = 128, ///< Centers the cursor on created full-screen windows.
  TRANSPARENT_FB = 256, ///< Blends what is behind the window according to the framebuffer's alpha channel.
  AUTOFOCUS      = 512, ///< Focuses the window every time it is shown.
#endif

  DEFAULT       = FOCUSED | RESIZABLE | VISIBLE | DECORATED, ///< Default window settings.
  NON_RESIZABLE = FOCUSED | VISIBLE | DECORATED,             ///< Default window settings without resizing capabilities.
  WINDOWED      = DEFAULT | MAXIMIZED,                       ///< Windowed full-screen window (with decorations).
  BORDERLESS    = FOCUSED /*| MAXIMIZED*/ | VISIBLE,         ///< Borderless full-screen window (without decorations).
  INVISIBLE     = 0                                          ///< Invisible window.
};
MAKE_ENUM_FLAG(WindowSetting)

/// Graphical window to render the scenes on, with input custom actions.
class Window {
  friend class RenderSystem;

public:
  /// Creates a window.
  /// \param renderSystem Render system containing this window.
  /// \param width Width of the window.
  /// \param height Height of the window.
  /// \param title Title of the window.
  /// \param settings Settings to create the window with.
  /// \param antiAliasingSampleCount Number of anti-aliasing samples.
  /// \note The width & height are to be considered just hints; the window manager remains responsible for the actual dimensions, which may be lower.
  ///   This can notably happen when the requested window size exceeds what the screens can display. The actual window's size can be queried afterward.
  /// \see getWidth(), getHeight()
  Window(RenderSystem& renderSystem,
         unsigned int width, unsigned int height,
         const std::string& title = {},
         WindowSetting settings = WindowSetting::DEFAULT,
         uint8_t antiAliasingSampleCount = 1);
  Window(const Window&) = delete;
  Window(Window&&) noexcept = default;

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
#if !defined(RAZ_NO_OVERLAY)
  Overlay& getOverlay() noexcept { return m_overlay; }
#endif

  void setClearColor(const Color& color, float alpha = 1.f) const;
  void setClearColor(float red, float green, float blue, float alpha = 1.f) const { setClearColor(Color(red, green, blue), alpha); }
  void setTitle(const std::string& title) const;
  /// Sets an image as window icon.
  /// \param img Image to be set as the window's icon. Must have an RGBA colorspace & a byte data type.
  void setIcon(const Image& img) const;

  template <typename... Args>
  static WindowPtr create(Args&&... args) { return std::make_unique<Window>(std::forward<Args>(args)...); }

  /// Resizes the window.
  /// \param width New window width.
  /// \param height New window height.
  /// \note The width & height are to be considered just hints; the window manager remains responsible for the actual dimensions, which may be lower.
  ///   This can notably happen when the requested window size exceeds what the screens can display. The actual size can be queried afterward.
  /// \see getWidth(), getHeight()
  void resize(unsigned int width, unsigned int height);
  /// Sets the window in a fullscreen mode, taking the whole main monitor's screen.
  /// \note To quit fullscreen, call makeWindowed().
  /// \see makeWindowed()
  void makeFullscreen();
  /// Sets the window in its windowed mode.
  void makeWindowed();
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
  void setCursorState(Cursor::State state) const;
  /// Shows the mouse cursor.
  /// Default behavior.
  void showCursor() const { setCursorState(Cursor::State::NORMAL); }
  /// Hides the mouse cursor.
  /// The cursor becomes invisible while being inside the window's frame. It can go out of the window.
  void hideCursor() const { setCursorState(Cursor::State::HIDDEN); }
  /// Disables the mouse cursor.
  /// The cursor always goes back to the window's center and becomes totally invisible. It can't go out of the window.
  void disableCursor() const { setCursorState(Cursor::State::DISABLED); }
  /// Adds action(s) to be executed on keyboard's key press and/or release.
  /// \param key Key triggering the given action(s).
  /// \param actionPress Action to be executed when the given key is pressed.
  /// \param frequency Frequency at which to execute the actions.
  /// \param actionRelease Action to be executed when the given key is released.
  void addKeyCallback(Keyboard::Key key, std::function<void(float)> actionPress,
                                         Input::ActionTrigger frequency = Input::ALWAYS,
                                         std::function<void()> actionRelease = nullptr);
  /// Adds action(s) to be executed on mouse button click and/or release.
  /// \param button Button triggering the given action(s).
  /// \param actionPress Action to be executed when the given mouse button is pressed.
  /// \param frequency Frequency at which to execute the actions.
  /// \param actionRelease Action to be executed when the given mouse button is released.
  void addMouseButtonCallback(Mouse::Button button, std::function<void(float)> actionPress,
                                                    Input::ActionTrigger frequency = Input::ALWAYS,
                                                    std::function<void()> actionRelease = nullptr);
  /// Sets the action to be executed on mouse wheel scroll.
  /// \param func Action to be executed when scrolling.
  void setMouseScrollCallback(std::function<void(double, double)> func);
  /// Sets the action to be executed on mouse move.
  /// \param func Action to be executed when the mouse is moved.
  void setMouseMoveCallback(std::function<void(double, double)> func);
  /// Sets the action to be executed on window close.
  /// \param func Action to be executed when the window is closed.
  void setCloseCallback(std::function<void()> func);
  /// Associates all the callbacks, making them active.
  void updateCallbacks() const;
#if !defined(RAZ_NO_OVERLAY)
  /// Changes the overlay's enabled state.
  /// \param enable True if it should be enabled, false otherwise.
  void enableOverlay(bool enable = true) { m_isOverlayEnabled = enable; }
  /// Disables the overlay.
  void disableOverlay() { enableOverlay(false); }
#endif
  /// Runs the window, refreshing its state by displaying the rendered scene, drawing the overlay, etc.
  /// \param deltaTime Amount of time elapsed since the last frame.
  /// \return True if the window hasn't been required to close, false otherwise.
  bool run(float deltaTime);
  /// Fetches the mouse position onto the window.
  /// \return 2D vector representing the mouse's position relative to the window.
  Vec2f recoverMousePosition() const;

  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) noexcept = default;

  ~Window() { close(); }

private:
  struct KeyboardCallback {
    Keyboard::Key key;
    std::function<void(float)> actionPress;
    Input::ActionTrigger frequency;
    std::function<void()> actionRelease;
  };

  struct MouseButtonCallback {
    Mouse::Button button;
    std::function<void(float)> actionPress;
    Input::ActionTrigger frequency;
    std::function<void()> actionRelease;
  };

  struct MouseMoveCallback {
    double xPrevPos {};
    double yPrevPos {};
    std::function<void(double, double)> action;
  };

  struct InputAction {
    std::function<void(float)> action;
    Input::ActionTrigger frequency;
  };

  /// Processes actions corresponding to keyboard & mouse inputs.
  /// \param deltaTime Amount of time elapsed since the last frame.
  void processInputs(float deltaTime);
  /// Tells the window that it should close.
  void setShouldClose() const;
  /// Closes the window.
  void close();

  static inline int s_refCounter = 0;

  OwnerValue<GLFWwindow*, nullptr> m_windowHandle {};
  RenderSystem* m_renderSystem {};

  int m_width {};
  int m_height {};
  int m_posX {};
  int m_posY {};

  std::vector<KeyboardCallback> m_keyboardCallbacks;
  std::vector<MouseButtonCallback> m_mouseButtonCallbacks;
  std::function<void(double, double)> m_mouseScrollCallback;
  MouseMoveCallback m_mouseMoveCallback;
  std::unordered_map<int, InputAction> m_inputActions;
  std::function<void()> m_closeCallback;

#if !defined(RAZ_NO_OVERLAY)
  Overlay m_overlay {};
  bool m_isOverlayEnabled = true;
#endif
};

} // namespace Raz

#endif // RAZ_WINDOW_HPP
