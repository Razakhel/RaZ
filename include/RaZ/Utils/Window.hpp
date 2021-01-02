#pragma once

#ifndef RAZ_WINDOW_HPP
#define RAZ_WINDOW_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/Overlay.hpp"
#include "RaZ/Utils/Input.hpp"

#include <functional>
#include <vector>

namespace Raz {

class Window;
using WindowPtr = std::unique_ptr<Window>;

using KeyboardCallbacks    = std::vector<std::tuple<int, std::function<void(float)>, Input::ActionTrigger, std::function<void()>>>;
using MouseButtonCallbacks = std::vector<std::tuple<int, std::function<void(float)>, Input::ActionTrigger, std::function<void()>>>;
using MouseScrollCallback  = std::function<void(double, double)>;
using MouseMoveCallback    = std::tuple<double, double, std::function<void(double, double)>>;
using InputActions         = std::unordered_map<int, std::pair<std::function<void(float)>, Input::ActionTrigger>>;
using InputCallbacks       = std::tuple<KeyboardCallbacks, MouseButtonCallbacks, MouseScrollCallback, MouseMoveCallback, InputActions>;

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

  DEFAULT    = FOCUSED /*| RESIZABLE*/ | VISIBLE | DECORATED, ///< Default window settings.
  WINDOWED   = DEFAULT | MAXIMIZED,                           ///< Windowed full-screen window (with decorations).
  BORDERLESS = FOCUSED | VISIBLE,                             ///< Borderless full-screen window (without decorations).
  INVISIBLE  = 0                                              ///< Invisible window.
};
MAKE_ENUM_FLAG(WindowSetting)

/// Graphical window to render the scenes on, with input custom actions.
class Window {
public:
  /// Creates a window.
  /// \param width Width of the window.
  /// \param height Height of the window.
  /// \param title Title of the window.
  /// \param settings Settings to create the window with.
  /// \param antiAliasingSampleCount Number of anti-aliasing samples.
  Window(unsigned int width, unsigned int height,
         const std::string& title = "",
         WindowSetting settings = WindowSetting::DEFAULT,
         uint8_t antiAliasingSampleCount = 1);
  Window(const Window&) = delete;
  Window(Window&&) noexcept = default;

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  const Vec4f& getClearColor() const { return m_clearColor; }
  const InputCallbacks& getCallbacks() const { return m_callbacks; }
  InputCallbacks& getCallbacks() { return m_callbacks; }

  void setClearColor(const Vec4f& clearColor) { m_clearColor = clearColor; }
  void setClearColor(float red, float green, float blue, float alpha = 1.f) { setClearColor(Vec4f(red, green, blue, alpha)); }
  void setTitle(const std::string& title) const;
  void setIcon(const Image& img) const;
  void setIcon(const FilePath& filePath) const { setIcon(Image(filePath, false)); }

  template <typename... Args>
  static WindowPtr create(Args&&... args) { return std::make_unique<Window>(std::forward<Args>(args)...); }

  /// Resizes the window.
  /// \param width New window width.
  /// \param height New window height.
  void resize(unsigned int width, unsigned int height);
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
  void changeCursorState(Cursor::State state) const;
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
#if defined(RAZ_USE_OVERLAY)
  /// Enables the overlay.
  void enableOverlay() { m_overlay = Overlay::create(m_window); }
  /// Disables the overlay.
  void disableOverlay() { m_overlay.reset(); }
  /// Adds a label on the overlay.
  /// \param label Text to be displayed.
  void addOverlayLabel(std::string label);
  /// Adds a button on the overlay.
  /// \param label Text to be displayed beside the button.
  /// \param action Action to be executed when clicked.
  void addOverlayButton(std::string label, std::function<void()> action);
  /// Adds a checkbox on the overlay.
  /// \param label Text to be displayed beside the checkbox.
  /// \param actionOn Action to be executed when toggled on.
  /// \param actionOff Action to be executed when toggled off.
  /// \param initVal Initial value, checked or not.
  void addOverlayCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal);
  /// Adds a floating-point slider on the overlay.
  /// \param label Text to be displayed beside the slider.
  /// \param actionSlide Action to be executed on a value change.
  /// \param minValue Lower value bound.
  /// \param maxValue Upper value bound.
  /// \param initValue Initial value.
  void addOverlaySlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue);
  /// Adds a texbox on the overlay.
  /// \param label Text to be displayed beside the checkbox.
  /// \param callback Function to be called every time the content is modified.
  void addOverlayTextbox(std::string label, std::function<void(const std::string&)> callback);
  /// Adds a texture on the overlay.
  /// \param texture Texture to be displayed.
  /// \param maxWidth Maximum texture's width.
  /// \param maxHeight Maximum texture's height.
  void addOverlayTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight);
  /// Adds a texture on the overlay. The maximum width & height will be those of the texture.
  /// \param texture Texture to be displayed.
  void addOverlayTexture(const Texture& texture);
  /// Adds an horizontal separator on the overlay.
  void addOverlaySeparator();
  /// Adds a frame time display on the overlay.
  /// \param formattedLabel Text with a formatting placeholder to display the frame time (%.Xf, X being the precision after the comma).
  void addOverlayFrameTime(std::string formattedLabel);
  /// Adds a FPS (frames per second) counter on the overlay.
  /// \param formattedLabel Text with a formatting placeholder to display the FPS (%.Xf, X being the precision after the comma).
  void addOverlayFpsCounter(std::string formattedLabel);
#endif
  /// Runs the window, refreshing its state by displaying the rendered scene, drawing the overlay, etc.
  /// \param deltaTime Amount of time elapsed since the last frame.
  /// \return True if the window hasn't been required to close, false otherwise.
  bool run(float deltaTime);
  /// Fetches the mouse position onto the window.
  /// \return 2D vector representing the mouse's position relative to the window.
  Vec2f recoverMousePosition() const;
  /// Tells the window that it should close.
  void setShouldClose() const;
  /// Closes the window.
  void close();

  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) noexcept = default;

  ~Window() { close(); }

private:
  unsigned int m_width {};
  unsigned int m_height {};
  Vec4f m_clearColor = Vec4f(0.15f, 0.15f, 0.15f, 1.f);

  GLFWwindow* m_window {};
  InputCallbacks m_callbacks {};

#if defined(RAZ_USE_OVERLAY)
  OverlayPtr m_overlay {};
#endif
};

} // namespace Raz

#endif // RAZ_WINDOW_HPP
