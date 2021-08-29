#include "GL/glew.h"
#if defined(RAZ_PLATFORM_WINDOWS)
#include "GL/wglew.h"
#elif defined(RAZ_PLATFORM_LINUX)
#include "GL/glxew.h"
#endif
#include "GLFW/glfw3.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/Window.hpp"

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <emscripten/html5.h>
#endif

namespace Raz {

Window::Window(unsigned int width, unsigned int height,
               const std::string& title,
               WindowSetting settings,
               uint8_t antiAliasingSampleCount) : m_width{ width }, m_height{ height } {
  glfwSetErrorCallback([] (int errorCode, const char* description) {
    Logger::error("[GLFW] " + std::string(description) + "(error code " + std::to_string(errorCode) + ").");
  });

  if (!glfwInit())
    throw std::runtime_error("Error: Failed to initialize GLFW");

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(RAZ_CONFIG_DEBUG)
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

#if defined(RAZ_PLATFORM_MAC) // Setting the OpenGL forward compatibility is required on macOS
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

#if defined(RAZ_USE_GL4)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

  glfwWindowHint(GLFW_FOCUSED, static_cast<int>(settings & WindowSetting::FOCUSED));
  glfwWindowHint(GLFW_RESIZABLE, static_cast<int>(settings & WindowSetting::RESIZABLE));
  glfwWindowHint(GLFW_VISIBLE, static_cast<int>(settings & WindowSetting::VISIBLE));
  glfwWindowHint(GLFW_DECORATED, static_cast<int>(settings & WindowSetting::DECORATED));
  glfwWindowHint(GLFW_AUTO_ICONIFY, static_cast<int>(settings & WindowSetting::AUTO_MINIMIZE));
  glfwWindowHint(GLFW_FLOATING, static_cast<int>(settings & WindowSetting::ALWAYS_ON_TOP));
  glfwWindowHint(GLFW_MAXIMIZED, static_cast<int>(settings & WindowSetting::MAXIMIZED));
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  glfwWindowHint(GLFW_CENTER_CURSOR, static_cast<int>(settings & WindowSetting::CENTER_CURSOR));
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, static_cast<int>(settings & WindowSetting::TRANSPARENT_FB));
  glfwWindowHint(GLFW_FOCUS_ON_SHOW, static_cast<int>(settings & WindowSetting::AUTOFOCUS));
#endif

  glfwWindowHint(GLFW_SAMPLES, antiAliasingSampleCount);

  m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
  if (!m_window) {
    close();
    throw std::runtime_error("Error: Failed to create GLFW Window");
  }

  glfwMakeContextCurrent(m_window);

  Renderer::initialize();
  Renderer::enable(Capability::DEPTH_TEST);

  glfwSetWindowUserPointer(m_window, this);

  glfwSetFramebufferSizeCallback(m_window, [] (GLFWwindow*, int newWidth, int newHeight) {
    Renderer::resizeViewport(0, 0, static_cast<unsigned int>(newWidth), static_cast<unsigned int>(newHeight));
  });

  enableFaceCulling();
}

void Window::setTitle(const std::string& title) const {
  glfwSetWindowTitle(m_window, title.c_str());
}

void Window::setIcon(const Image& img) const {
  const GLFWimage icon = { static_cast<int>(img.getWidth()),
                           static_cast<int>(img.getHeight()),
                           const_cast<unsigned char*>(static_cast<const uint8_t*>(img.getDataPtr())) };
  glfwSetWindowIcon(m_window, 1, &icon);
}

void Window::resize(unsigned int width, unsigned int height) {
  m_width  = width;
  m_height = height;

  glfwSetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
}

void Window::enableFaceCulling(bool value) const {
  if (value)
    Renderer::enable(Capability::CULL);
  else
    Renderer::disable(Capability::CULL);
}

bool Window::recoverVerticalSyncState() const {
#if defined(RAZ_PLATFORM_WINDOWS)
  if (wglGetExtensionsStringEXT())
    return static_cast<bool>(wglGetSwapIntervalEXT());
#elif defined(RAZ_PLATFORM_LINUX)
  if (glXQueryExtensionsString(glXGetCurrentDisplay(), 0)) {
    unsigned int interval;
    glXQueryDrawable(glXGetCurrentDisplay(), glXGetCurrentDrawable(), GLX_SWAP_INTERVAL_EXT, &interval);

    return static_cast<bool>(interval);
  }
#elif defined(RAZ_PLATFORM_MAC)
  return true;
#endif

  Logger::warn("Vertical synchronization unsupported.");
  return false;
}

void Window::enableVerticalSync([[maybe_unused]] bool value) const {
#if defined(RAZ_PLATFORM_WINDOWS)
  if (wglGetExtensionsStringEXT()) {
    wglSwapIntervalEXT(static_cast<int>(value));
    return;
  }
#elif defined(RAZ_PLATFORM_LINUX)
  if (glXQueryExtensionsString(glXGetCurrentDisplay(), 0)) {
    glXSwapIntervalEXT(glXGetCurrentDisplay(), glXGetCurrentDrawable(), value);
    glXSwapIntervalMESA(static_cast<unsigned int>(value));
    return;
  }
#elif defined(RAZ_PLATFORM_MAC)
  glfwSwapInterval(value);
#endif

  Logger::warn("Vertical synchronization unsupported.");
}

void Window::changeCursorState(Cursor::State state) const {
  glfwSetInputMode(m_window, GLFW_CURSOR, state);
}

void Window::addKeyCallback(Keyboard::Key key, std::function<void(float)> actionPress,
                                               Input::ActionTrigger frequency,
                                               std::function<void()> actionRelease) {
  std::get<0>(m_callbacks).emplace_back(key, std::move(actionPress), frequency, std::move(actionRelease));
  updateCallbacks();
}

void Window::addMouseButtonCallback(Mouse::Button button, std::function<void(float)> actionPress,
                                                          Input::ActionTrigger frequency,
                                                          std::function<void()> actionRelease) {
  std::get<1>(m_callbacks).emplace_back(button, std::move(actionPress), frequency, std::move(actionRelease));
  updateCallbacks();
}

void Window::addMouseScrollCallback(std::function<void(double, double)> func) {
  std::get<2>(m_callbacks) = std::move(func);
  updateCallbacks();
}

void Window::addMouseMoveCallback(std::function<void(double, double)> func) {
  std::get<3>(m_callbacks) = std::make_tuple(m_width / 2, m_height / 2, std::move(func));
  updateCallbacks();
}

void Window::setCloseCallback(std::function<void()> func) {
  m_closeCallback = std::move(func);

  glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
    CloseCallback& closeCallback = static_cast<Window*>(glfwGetWindowUserPointer(window))->getCloseCallback();
    closeCallback();
  });
}

void Window::updateCallbacks() const {
  // Keyboard inputs
  if (!std::get<0>(m_callbacks).empty()) {
    glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int /* scancode */, int action, int /* mode */) {
      InputCallbacks& callbacks = static_cast<Window*>(glfwGetWindowUserPointer(window))->getCallbacks();
      const auto& keyCallbacks = std::get<0>(callbacks);

      for (const auto& callback : keyCallbacks) {
        if (key == std::get<0>(callback)) {
          if (action == GLFW_PRESS) {
            std::get<4>(callbacks).emplace(key, std::make_pair(std::get<1>(callback), std::get<2>(callback)));
          } else if (action == GLFW_RELEASE) {
            std::get<4>(callbacks).erase(key);

            if (std::get<3>(callback))
              std::get<3>(callback)();
          }
        }
      }
    });
  }

  // Mouse buttons inputs
  if (!std::get<1>(m_callbacks).empty()) {
    glfwSetMouseButtonCallback(m_window, [] (GLFWwindow* window, int button, int action, int /* mods */) {
      InputCallbacks& callbacks = static_cast<Window*>(glfwGetWindowUserPointer(window))->getCallbacks();
      const MouseButtonCallbacks& mouseCallbacks = std::get<1>(callbacks);

      for (const auto& callback : mouseCallbacks) {
        if (button == std::get<0>(callback)) {
          if (action == GLFW_PRESS) {
            std::get<4>(callbacks).emplace(button, std::make_pair(std::get<1>(callback), std::get<2>(callback)));
          } else if (action == GLFW_RELEASE) {
            std::get<4>(callbacks).erase(button);

            if (std::get<3>(callback))
              std::get<3>(callback)();
          }
        }
      }
    });
  }

  // Mouse scroll input
  if (std::get<2>(m_callbacks)) {
    glfwSetScrollCallback(m_window, [] (GLFWwindow* window, double xOffset, double yOffset) {
      const MouseScrollCallback& scrollCallback = std::get<2>(static_cast<Window*>(glfwGetWindowUserPointer(window))->getCallbacks());
      scrollCallback(xOffset, yOffset);
    });
  }

  // Mouse move input
  if (std::get<2>(std::get<3>(m_callbacks))) {
    glfwSetCursorPosCallback(m_window, [] (GLFWwindow* window, double xPosition, double yPosition) {
      MouseMoveCallback& moveCallback = std::get<3>(static_cast<Window*>(glfwGetWindowUserPointer(window))->getCallbacks());

      double& xPrevPos = std::get<0>(moveCallback);
      double& yPrevPos = std::get<1>(moveCallback);

      std::get<2>(moveCallback)(xPosition - xPrevPos, yPosition - yPrevPos);

      xPrevPos = xPosition;
      yPrevPos = yPosition;
    });
  }
}

#if defined(RAZ_USE_OVERLAY)
void Window::addOverlayLabel(std::string label) {
  m_overlay->addLabel(std::move(label));
}

void Window::addOverlayButton(std::string label, std::function<void()> action) {
  m_overlay->addButton(std::move(label), std::move(action));
}

void Window::addOverlayCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal) {
  m_overlay->addCheckbox(std::move(label), std::move(actionOn), std::move(actionOff), initVal);
}

void Window::addOverlaySlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue) {
  m_overlay->addSlider(std::move(label), std::move(actionSlide), minValue, maxValue, initValue);
}

void Window::addOverlayTextbox(std::string label, std::function<void(const std::string&)> callback) {
  m_overlay->addTextbox(std::move(label), std::move(callback));
}

void Window::addOverlayTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight) {
  m_overlay->addTexture(texture, maxWidth, maxHeight);
}

void Window::addOverlayTexture(const Texture& texture) {
  m_overlay->addTexture(texture);
}

void Window::addOverlaySeparator() {
  m_overlay->addSeparator();
}

void Window::addOverlayFrameTime(std::string formattedLabel) {
  m_overlay->addFrameTime(std::move(formattedLabel));
}

void Window::addOverlayFpsCounter(std::string formattedLabel) {
  m_overlay->addFpsCounter(std::move(formattedLabel));
}
#endif

bool Window::run(float deltaTime) {
  if (glfwWindowShouldClose(m_window))
    return false;

  glfwPollEvents();

#if defined(RAZ_USE_OVERLAY)
  // Input callbacks should not be executed if the overlay requested keyboard focus
  if (!m_overlay || !m_overlay->hasKeyboardFocus())
#endif
  {
    // Process actions belonging to pressed keys & mouse buttons
    auto& actions   = std::get<4>(m_callbacks);
    auto actionIter = actions.begin();

    while (actionIter != actions.end()) {
      auto& action = actionIter->second;

      // An action consists of two parts:
      //   - a callback associated to the triggered key or button
      //   - a value indicating if it should be executed only once or every frame

      action.first(deltaTime);

      // Removing the current action if ONCE is given, or simply increment the iterator
      if (action.second == Input::ONCE)
        actionIter = actions.erase(actionIter); // std::unordered_map::erase(iter) returns an iterator on the next element
      else
        ++actionIter;
    }
  }

#if defined(RAZ_USE_OVERLAY)
  if (m_overlay)
    m_overlay->render();
#endif

  glfwSwapBuffers(m_window);

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
  emscripten_webgl_commit_frame();
#endif

  Renderer::clearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
  Renderer::clear(MaskType::COLOR | MaskType::DEPTH);

  return true;
}

Vec2f Window::recoverMousePosition() const {
  double xPos {};
  double yPos {};
  glfwGetCursorPos(m_window, &xPos, &yPos);

  return Vec2f(static_cast<float>(xPos), static_cast<float>(yPos));
}

void Window::setShouldClose() const {
  glfwSetWindowShouldClose(m_window, true);
}

void Window::close() {
#if defined(RAZ_USE_OVERLAY)
  disableOverlay();
#endif

  glfwTerminate();

  m_window = nullptr;
}

} // namespace Raz
