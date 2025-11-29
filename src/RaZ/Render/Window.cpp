#include "RaZ/Data/Image.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"
#include "RaZ/Utils/Logger.hpp"

// GLEW is needed for V-sync management
#include "GL/glew.h"
#if defined(RAZ_PLATFORM_WINDOWS)
#include "GL/wglew.h"
#elif defined(RAZ_PLATFORM_LINUX)
#include "GL/glxew.h"
#endif

#include "GLFW/glfw3.h"

#if !defined(RAZ_NO_OVERLAY)
// Needed to set ImGui's callbacks
#include "imgui_impl_glfw.h"
#endif

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <emscripten/html5.h>
#endif

namespace Raz {

Window::Window(RenderSystem& renderSystem,
               unsigned int width, unsigned int height,
               const std::string& title,
               WindowSetting settings,
               uint8_t antiAliasingSampleCount) : m_renderSystem{ &renderSystem } {
  ZoneScopedN("Window::Window");

  Logger::debug("[Window] Initializing...");

  glfwSetErrorCallback([] (int errorCode, const char* description) {
    Logger::error("[GLFW] {} (error code {})", description, errorCode);
  });

  if (!glfwInit())
    throw std::runtime_error("Error: Failed to initialize GLFW");

#if !defined(USE_OPENGL_ES)
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif

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

    m_windowHandle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, glfwGetCurrentContext());

    if (m_windowHandle)
      break;

    if (glfwGetError(nullptr) == GLFW_VERSION_UNAVAILABLE) {
      Logger::error("[Window] OpenGL {}.{} unsupported; attempting to fallback to a lower version", major, minor);
      continue;
    }

    close();
    throw std::runtime_error("Error: Failed to create GLFW Window");
  }
#else
  m_windowHandle = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, glfwGetCurrentContext());
#endif

  glfwSetWindowUserPointer(m_windowHandle, this);
  glfwGetWindowSize(m_windowHandle, &m_width, &m_height);
  glfwGetWindowPos(m_windowHandle, &m_posX, &m_posY);

  if (glfwGetCurrentContext() == nullptr)
    glfwMakeContextCurrent(m_windowHandle);

  Renderer::initialize();
  setClearColor(0.15f, 0.15f, 0.15f);

  glfwSetFramebufferSizeCallback(m_windowHandle, [] (GLFWwindow* windowHandle, int newWidth, int newHeight) {
    const auto* window = static_cast<const Window*>(glfwGetWindowUserPointer(windowHandle));
    window->m_renderSystem->resizeViewport(static_cast<unsigned int>(newWidth), static_cast<unsigned int>(newHeight));
  });

#if !defined(RAZ_NO_OVERLAY)
  Overlay::initialize(m_windowHandle);

  glfwSetWindowContentScaleCallback(m_windowHandle, [] (GLFWwindow*, float horizScale, float /* vertScale */) {
    Overlay::rescale(horizScale);
  });

#if !defined(RAZ_PLATFORM_EMSCRIPTEN) // glfwGetWindowContentScale() isn't available with Emscripten
  float windowHorizScale {};
  glfwGetWindowContentScale(m_windowHandle, &windowHorizScale, nullptr);

  if (windowHorizScale != 0.f)
    Overlay::rescale(windowHorizScale);
#endif // RAZ_PLATFORM_EMSCRIPTEN
#endif // RAZ_NO_OVERLAY

  ++s_refCounter;

  Logger::debug("[Window] Initialized");
}

void Window::setClearColor(const Color& color, float alpha) const {
  Renderer::clearColor(color.red(), color.green(), color.blue(), alpha);
}

void Window::setTitle(const std::string& title) const {
  glfwSetWindowTitle(m_windowHandle, title.c_str());
}

void Window::setIcon(const Image& img) const {
  if (img.isEmpty()) {
    Logger::error("[Window] Empty image given as window icon");
    return;
  }

  if (img.getColorspace() != ImageColorspace::RGBA) {
    Logger::error("[Window] The window icon can only be created from an image having an RGBA colorspace");
    return;
  }

  if (img.getDataType() != ImageDataType::BYTE) {
    Logger::error("[Window] The window icon can only be created from an image having byte data");
    return;
  }

  const GLFWimage icon = { static_cast<int>(img.getWidth()),
                           static_cast<int>(img.getHeight()),
                           const_cast<unsigned char*>(static_cast<const uint8_t*>(img.getDataPtr())) };
  glfwSetWindowIcon(m_windowHandle, 1, &icon);
}

void Window::resize(unsigned int width, unsigned int height) {
  glfwSetWindowSize(m_windowHandle, static_cast<int>(width), static_cast<int>(height));
  glfwGetWindowSize(m_windowHandle, &m_width, &m_height);
}

void Window::makeFullscreen() {
  glfwGetWindowSize(m_windowHandle, &m_width, &m_height);
  glfwGetWindowPos(m_windowHandle, &m_posX, &m_posY);

  GLFWmonitor* monitor    = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  glfwSetWindowMonitor(m_windowHandle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void Window::makeWindowed() {
  glfwSetWindowMonitor(m_windowHandle, nullptr, m_posX, m_posY, m_width, m_height, GLFW_DONT_CARE);
}

void Window::enableFaceCulling(bool value) const {
  if (value)
    Renderer::enable(Capability::CULL);
  else
    Renderer::disable(Capability::CULL);
}

bool Window::recoverVerticalSyncState() const {
  ZoneScopedN("Window::recoverVerticalSyncState");

#if defined(RAZ_PLATFORM_WINDOWS)
  if (wglGetExtensionsStringEXT())
    return static_cast<bool>(wglGetSwapIntervalEXT());

  return true;
#elif defined(RAZ_PLATFORM_LINUX)
  if (glXQueryExtensionsString(glXGetCurrentDisplay(), 0)) {
    unsigned int interval {};
    glXQueryDrawable(glXGetCurrentDisplay(), glXGetCurrentDrawable(), GLX_SWAP_INTERVAL_EXT, &interval);

    return static_cast<bool>(interval);
  }

  return true;
#elif defined(RAZ_PLATFORM_MAC)
  return true;
#else
  Logger::warn("Vertical synchronization unsupported");
  return false;
#endif
}

void Window::enableVerticalSync([[maybe_unused]] bool value) const {
  ZoneScopedN("Window::enableVerticalSync");

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
  Logger::warn("Vertical synchronization unsupported");
#endif
}

void Window::setCursorState(Cursor::State state) const {
  glfwSetInputMode(m_windowHandle, GLFW_CURSOR, state);
}

void Window::addKeyCallback(Keyboard::Key key, std::function<void(float)> actionPress,
                                               Input::ActionTrigger frequency,
                                               std::function<void()> actionRelease) {
  m_keyboardCallbacks.emplace_back(key, std::move(actionPress), frequency, std::move(actionRelease));
  updateCallbacks();
}

void Window::addMouseButtonCallback(Mouse::Button button, std::function<void(float)> actionPress,
                                                          Input::ActionTrigger frequency,
                                                          std::function<void()> actionRelease) {
  m_mouseButtonCallbacks.emplace_back(button, std::move(actionPress), frequency, std::move(actionRelease));
  updateCallbacks();
}

void Window::setMouseScrollCallback(std::function<void(double, double)> func) {
  m_mouseScrollCallback = std::move(func);
  updateCallbacks();
}

void Window::setMouseMoveCallback(std::function<void(double, double)> func) {
  m_mouseMoveCallback = { m_width * 0.5, m_height * 0.5, std::move(func) };
  updateCallbacks();
}

void Window::setCloseCallback(std::function<void()> func) {
  m_closeCallback = std::move(func);

  glfwSetWindowCloseCallback(m_windowHandle, [] (GLFWwindow* windowHandle) {
    static_cast<const Window*>(glfwGetWindowUserPointer(windowHandle))->m_closeCallback();
  });
}

void Window::updateCallbacks() const {
  ZoneScopedN("Window::updateCallbacks");

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
  glfwSetKeyCallback(m_windowHandle, [] (GLFWwindow* windowHandle, int key, int scancode, int action, int mods) {
#if !defined(RAZ_NO_OVERLAY)
    ImGui_ImplGlfw_KeyCallback(windowHandle, key, scancode, action, mods);

    // Key callbacks shouldn't be executed if the overlay requested keyboard focus
    if (ImGui::GetIO().WantCaptureKeyboard)
      return;
#endif

    Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));

    for (const auto& callback : window.m_keyboardCallbacks) {
      if (key != callback.key)
        continue;

      auto& actions = window.m_inputActions;

      if (action == GLFW_PRESS) {
        actions.emplace(key, InputAction{ callback.actionPress, callback.frequency });
      } else if (action == GLFW_RELEASE) {
        actions.erase(key);

        if (const auto& actionRelease = callback.actionRelease)
          actionRelease();
      }
    }
  });

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
  glfwSetMouseButtonCallback(m_windowHandle, [] (GLFWwindow* windowHandle, int button, int action, int mods) {
#if !defined(RAZ_NO_OVERLAY)
    ImGui_ImplGlfw_MouseButtonCallback(windowHandle, button, action, mods);

    // Mouse buttons callbacks shouldn't be executed if the overlay requested mouse focus
    if (ImGui::GetIO().WantCaptureMouse)
      return;
#endif

    Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));

    for (const auto& callback : window.m_mouseButtonCallbacks) {
      if (button != callback.button)
        continue;

      if (action == GLFW_PRESS) {
        window.m_inputActions.emplace(button, InputAction{ callback.actionPress, callback.frequency });
      } else if (action == GLFW_RELEASE) {
        window.m_inputActions.erase(button);

        if (const auto& actionRelease = callback.actionRelease)
          actionRelease();
      }
    }
  });

  // Mouse scroll input
  glfwSetScrollCallback(m_windowHandle, [] (GLFWwindow* windowHandle, double xOffset, double yOffset) {
#if !defined(RAZ_NO_OVERLAY)
    ImGui_ImplGlfw_ScrollCallback(windowHandle, xOffset, yOffset);

    // Scroll callback shouldn't be executed if the overlay requested mouse focus
    if (ImGui::GetIO().WantCaptureMouse)
      return;
#endif

    if (const auto& scrollCallback = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))->m_mouseScrollCallback)
      scrollCallback(xOffset, yOffset);
  });

  // Mouse move input
  glfwSetCursorPosCallback(m_windowHandle, [] (GLFWwindow* windowHandle, double xPosition, double yPosition) {
#if !defined(RAZ_NO_OVERLAY)
    ImGui_ImplGlfw_CursorPosCallback(windowHandle, xPosition, yPosition);

    if (ImGui::GetIO().WantCaptureMouse)
      return;
#endif

    auto& [xPrevPos, yPrevPos, action] = static_cast<Window*>(glfwGetWindowUserPointer(windowHandle))->m_mouseMoveCallback;

    if (action == nullptr)
      return;

    action(xPosition - xPrevPos, yPosition - yPrevPos);
    xPrevPos = xPosition;
    yPrevPos = yPosition;
  });
}

bool Window::run(float deltaTime) {
  ZoneScopedN("Window::run");

  if (glfwWindowShouldClose(m_windowHandle))
    return false;

  processInputs(deltaTime);

#if !defined(RAZ_NO_OVERLAY)
  if (m_isOverlayEnabled && !m_overlay.isEmpty())
    m_overlay.render();
#endif

  {
    ZoneScopedN("glfwSwapBuffers");
    TracyGpuZone("SwapBuffers")
    glfwSwapBuffers(m_windowHandle);
  }

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
  emscripten_webgl_commit_frame();
#endif

  {
    TracyGpuZone("TracyGpuCollect")
    TracyGpuCollect
  }

  return true;
}

Vec2f Window::recoverMousePosition() const {
  double xPos {};
  double yPos {};
  glfwGetCursorPos(m_windowHandle, &xPos, &yPos);

  return Vec2f(static_cast<float>(xPos), static_cast<float>(yPos));
}

void Window::processInputs(float deltaTime) {
  ZoneScopedN("Window::processInputs");

  {
    ZoneScopedN("glfwPollEvents");
    glfwPollEvents();
  }

  auto actionIter = m_inputActions.cbegin();

  while (actionIter != m_inputActions.cend()) {
    const auto& [action, frequency] = actionIter->second;

    action(deltaTime);

    // Removing the current action if it should be executed only once, or simply increment the iterator
    if (frequency == Input::ONCE)
      actionIter = m_inputActions.erase(actionIter); // std::unordered_map::erase(iter) returns an iterator on the next element
    else
      ++actionIter;
  }
}

void Window::setShouldClose() const {
  glfwSetWindowShouldClose(m_windowHandle, true);
}

void Window::close() {
  ZoneScopedN("Window::close");

  if (!m_windowHandle.isValid())
    return;

  Logger::debug("[Window] Closing...");

  --s_refCounter;

  if (s_refCounter == 0) {
#if !defined(RAZ_NO_OVERLAY)
    Overlay::destroy();
#endif

    {
      ZoneScopedN("glfwTerminate");
      glfwTerminate();
    }
    m_windowHandle = nullptr;
  }

  Logger::debug("[Window] Closed");
}

} // namespace Raz
