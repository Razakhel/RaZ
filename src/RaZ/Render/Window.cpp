// GLEW is needed for V-sync management
#include "GL/glew.h"
#if defined(RAZ_PLATFORM_WINDOWS)
#include "GL/wglew.h"
#elif defined(RAZ_PLATFORM_LINUX)
#include "GL/glxew.h"
#endif

#if !defined(RAZ_NO_OVERLAY)
// Needed to set ImGui's callbacks
#include "imgui/imgui_impl_glfw.h"
#endif

#include "GLFW/glfw3.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Window.hpp"
#include "RaZ/Utils/Logger.hpp"

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <emscripten/html5.h>
#endif

namespace Raz {

Window::Window(unsigned int width, unsigned int height,
               const std::string& title,
               WindowSetting settings,
               uint8_t antiAliasingSampleCount) : m_width{ width }, m_height{ height } {
  Logger::debug("[Window] Initializing...");

  glfwSetErrorCallback([] (int errorCode, const char* description) {
    Logger::error("[GLFW] " + std::string(description) + " (error code " + std::to_string(errorCode) + ").");
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

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  static constexpr std::array<std::pair<int, int>, 8> glVersions = {{
    { 4, 6 },
    { 4, 5 },
    { 4, 4 },
    { 4, 3 },
    { 4, 2 },
    { 4, 1 },
    { 4, 0 },
    { 3, 3 }
  }};

  for (auto [major, minor] : glVersions) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

    m_windowHandle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);

    if (m_windowHandle)
      break;

    if (glfwGetError(nullptr) == GLFW_VERSION_UNAVAILABLE) {
      Logger::error("[Window] OpenGL " + std::to_string(major) + '.' + std::to_string(minor) + " unsupported; attempting to fallback to a lower version.");
      continue;
    }

    close();
    throw std::runtime_error("Error: Failed to create GLFW Window");
  }
#else
  m_windowHandle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
#endif

  glfwSetWindowUserPointer(m_windowHandle, this);
  glfwMakeContextCurrent(m_windowHandle);

  Renderer::initialize();
  setClearColor(0.15f, 0.15f, 0.15f);

  glfwSetFramebufferSizeCallback(m_windowHandle, [] (GLFWwindow*, int newWidth, int newHeight) {
    Renderer::resizeViewport(0, 0, static_cast<unsigned int>(newWidth), static_cast<unsigned int>(newHeight));
  });

#if !defined(RAZ_NO_OVERLAY)
  m_overlay.initialize(m_windowHandle);
#endif

  Logger::debug("[Window] Initialized");
}

void Window::setClearColor(const Vec4f& clearColor) const {
  Renderer::clearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
}

void Window::setTitle(const std::string& title) const {
  glfwSetWindowTitle(m_windowHandle, title.c_str());
}

void Window::setIcon(const Image& img) const {
  const GLFWimage icon = { static_cast<int>(img.getWidth()),
                           static_cast<int>(img.getHeight()),
                           const_cast<unsigned char*>(static_cast<const uint8_t*>(img.getDataPtr())) };
  glfwSetWindowIcon(m_windowHandle, 1, &icon);
}

void Window::resize(unsigned int width, unsigned int height) {
  m_width  = width;
  m_height = height;

  glfwSetWindowSize(m_windowHandle, static_cast<int>(width), static_cast<int>(height));
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

  return true;
#elif defined(RAZ_PLATFORM_LINUX)
  if (glXQueryExtensionsString(glXGetCurrentDisplay(), 0)) {
    unsigned int interval;
    glXQueryDrawable(glXGetCurrentDisplay(), glXGetCurrentDrawable(), GLX_SWAP_INTERVAL_EXT, &interval);

    return static_cast<bool>(interval);
  }

  return true;
#elif defined(RAZ_PLATFORM_MAC)
  return true;
#else
  Logger::warn("Vertical synchronization unsupported.");
  return false;
#endif
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
#else
  Logger::warn("Vertical synchronization unsupported.");
#endif
}

void Window::setCursorState(Cursor::State state) const {
  glfwSetInputMode(m_windowHandle, GLFW_CURSOR, state);
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

void Window::setMouseScrollCallback(std::function<void(double, double)> func) {
  std::get<2>(m_callbacks) = std::move(func);
  updateCallbacks();
}

void Window::setMouseMoveCallback(std::function<void(double, double)> func) {
  std::get<3>(m_callbacks) = std::make_tuple(m_width / 2, m_height / 2, std::move(func));
  updateCallbacks();
}

void Window::setCloseCallback(std::function<void()> func) {
  m_closeCallback = std::move(func);

  glfwSetWindowCloseCallback(m_windowHandle, [] (GLFWwindow* window) {
    const CloseCallback& closeCallback = static_cast<Window*>(glfwGetWindowUserPointer(window))->getCloseCallback();
    closeCallback();
  });
}

void Window::updateCallbacks() const {
#if !defined(RAZ_NO_OVERLAY)
  // Monitor events
  glfwSetMonitorCallback([] (GLFWmonitor* monitorHandle, int event) {
    ImGui_ImplGlfw_MonitorCallback(monitorHandle, event);
  });
#endif

#if !defined(RAZ_NO_OVERLAY)
  // Window focus
  glfwSetWindowFocusCallback(m_windowHandle, [] (GLFWwindow* windowHandle, int focused) {
    ImGui_ImplGlfw_WindowFocusCallback(windowHandle, focused);
  });
#endif

  // Keyboard inputs
  if (!std::get<0>(m_callbacks).empty()) {
    glfwSetKeyCallback(m_windowHandle, [] (GLFWwindow* windowHandle, int key, int scancode, int action, int mods) {
#if !defined(RAZ_NO_OVERLAY)
      ImGui_ImplGlfw_KeyCallback(windowHandle, key, scancode, action, mods);

      // Key callbacks should not be executed if the overlay requested keyboard focus
      if (ImGui::GetIO().WantCaptureKeyboard)
        return;
#endif

      InputCallbacks& callbacks = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))->getCallbacks();
      const auto& keyCallbacks  = std::get<0>(callbacks);

      for (const auto& callback : keyCallbacks) {
        if (key != std::get<0>(callback))
          continue;

        auto& actions = std::get<InputActions>(callbacks);

        if (action == GLFW_PRESS) {
          actions.emplace(key, std::make_pair(std::get<1>(callback), std::get<2>(callback)));
        } else if (action == GLFW_RELEASE) {
          actions.erase(key);

          const auto& actionRelease = std::get<3>(callback);

          if (actionRelease)
            actionRelease();
        }
      }
    });
  }

#if !defined(RAZ_NO_OVERLAY)
  // Unicode character inputs
  glfwSetCharCallback(m_windowHandle, [] (GLFWwindow* windowHandle, unsigned int codePoint) {
    ImGui_ImplGlfw_CharCallback(windowHandle, codePoint);
  });
#endif

#if !defined(RAZ_NO_OVERLAY)
  // Cursor enter event
  glfwSetCursorEnterCallback(m_windowHandle, [] (GLFWwindow* windowHandle, int entered) {
    ImGui_ImplGlfw_CursorEnterCallback(windowHandle, entered);
  });
#endif

  // Mouse buttons inputs
  if (!std::get<1>(m_callbacks).empty()) {
    glfwSetMouseButtonCallback(m_windowHandle, [] (GLFWwindow* windowHandle, int button, int action, int mods) {
#if !defined(RAZ_NO_OVERLAY)
      ImGui_ImplGlfw_MouseButtonCallback(windowHandle, button, action, mods);

      // Mouse buttons callbacks should not be executed if the overlay requested mouse focus
      if (ImGui::GetIO().WantCaptureMouse)
        return;
#endif

      InputCallbacks& callbacks  = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))->getCallbacks();
      const auto& mouseCallbacks = std::get<1>(callbacks);

      for (const auto& callback : mouseCallbacks) {
        if (button != std::get<0>(callback))
          continue;

        auto& actions = std::get<InputActions>(callbacks);

        if (action == GLFW_PRESS) {
          actions.emplace(button, std::make_pair(std::get<1>(callback), std::get<2>(callback)));
        } else if (action == GLFW_RELEASE) {
          actions.erase(button);

          const auto& actionRelease = std::get<3>(callback);

          if (actionRelease)
            actionRelease();
        }
      }
    });
  }

  // Mouse scroll input
  if (std::get<2>(m_callbacks)) {
    glfwSetScrollCallback(m_windowHandle, [] (GLFWwindow* windowHandle, double xOffset, double yOffset) {
#if !defined(RAZ_NO_OVERLAY)
      ImGui_ImplGlfw_ScrollCallback(windowHandle, xOffset, yOffset);

      // Scroll callback should not be executed if the overlay requested mouse focus
      if (ImGui::GetIO().WantCaptureMouse)
        return;
#endif

      const auto& scrollCallback = std::get<2>(static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))->getCallbacks());
      scrollCallback(xOffset, yOffset);
    });
  }

  // Mouse move input
  if (std::get<2>(std::get<3>(m_callbacks))) {
    glfwSetCursorPosCallback(m_windowHandle, [] (GLFWwindow* windowHandle, double xPosition, double yPosition) {
      MouseMoveCallback& moveCallback = std::get<3>(static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))->getCallbacks());

      double& xPrevPos = std::get<0>(moveCallback);
      double& yPrevPos = std::get<1>(moveCallback);

      std::get<2>(moveCallback)(xPosition - xPrevPos, yPosition - yPrevPos);

      xPrevPos = xPosition;
      yPrevPos = yPosition;
    });
  }
}

bool Window::run(float deltaTime) {
  if (glfwWindowShouldClose(m_windowHandle))
    return false;

  glfwPollEvents();

  // Process actions belonging to pressed keys & mouse buttons
  auto& actions   = std::get<4>(m_callbacks);
  auto actionIter = actions.cbegin();

  while (actionIter != actions.cend()) {
    const auto& action = actionIter->second;

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

#if !defined(RAZ_NO_OVERLAY)
  if (m_isOverlayEnabled && !m_overlay.isEmpty())
    m_overlay.render();
#endif

  glfwSwapBuffers(m_windowHandle);

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
  emscripten_webgl_commit_frame();
#endif

  return true;
}

Vec2f Window::recoverMousePosition() const {
  double xPos {};
  double yPos {};
  glfwGetCursorPos(m_windowHandle, &xPos, &yPos);

  return Vec2f(static_cast<float>(xPos), static_cast<float>(yPos));
}

void Window::setShouldClose() const {
  glfwSetWindowShouldClose(m_windowHandle, true);
}

void Window::close() {
  Logger::debug("[Window] Closing...");

#if !defined(RAZ_NO_OVERLAY)
  m_overlay.destroy();
#endif

  glfwTerminate();

  m_windowHandle = nullptr;

  Logger::debug("[Window] Closed");
}

} // namespace Raz
