#include <iostream>

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
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Window.hpp"

namespace Raz {

namespace {

void GLAPIENTRY callbackDebugLog(GLenum source,
                                 GLenum type,
                                 unsigned int id,
                                 GLenum severity,
                                 int /* length */,
                                 const char* message,
                                 const void* /* userParam */) {
  std::cerr << "OpenGL Debug - ";

  switch (source) {
    case GL_DEBUG_SOURCE_API:             std::cerr << "Source: OpenGL\t"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Windows\t"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader compiler\t"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third party\t"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application\t"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other\t"; break;
    default: break;
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error\t"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated behavior\t"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined behavior\t"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability\t"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance\t"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other\t"; break;
    default: break;
  }

  std::cerr << "ID: " << id << "\t";

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:   std::cerr << "Severity: High\t"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Severity: Medium\t"; break;
    case GL_DEBUG_SEVERITY_LOW:    std::cerr << "Severity: Low\t"; break;
    default: break;
  }

  std::cerr << "Message: " << message << std::endl;
}

} // namespace

Window::Window(unsigned int width, unsigned int height, const std::string& title, uint8_t AASampleCount) : m_width{ width },
                                                                                                           m_height{ height } {
  glfwSetErrorCallback([] (int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
  });

  if (!glfwInit())
    throw std::runtime_error("Error: Failed to initialize GLFW");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, AASampleCount);

#if defined(__APPLE__) // Setting the OpenGL forward compatibility is required on macOS
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
  if (!m_window) {
    close();
    throw std::runtime_error("Error: Failed to create GLFW Window");
  }

  glfwMakeContextCurrent(m_window);

  glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

  Renderer::initialize();

#if !defined(__APPLE__) // Setting the debug message callback provokes a crash on macOS
  glDebugMessageCallback(&callbackDebugLog, nullptr);
#endif
  Renderer::enable(Capability::DEBUG_OUTPUT_SYNCHRONOUS);

  glfwSetWindowUserPointer(m_window, this);

  enableFaceCulling();
  Renderer::enable(Capability::DEPTH_TEST);
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

void Window::enableFaceCulling(bool value) const {
  if (value)
    Renderer::enable(Capability::CULL);
  else
    Renderer::disable(Capability::CULL);
}

bool Window::recoverVerticalSyncState() const {
#if defined(_WIN32)
  if (wglGetExtensionsStringEXT())
    return static_cast<bool>(wglGetSwapIntervalEXT());
#elif defined(__gnu_linux__)
  if (glXQueryExtensionsString(glXGetCurrentDisplay(), 0)) {
    unsigned int interval;
    glXQueryDrawable(glXGetCurrentDisplay(), glXGetCurrentDrawable(), GLX_SWAP_INTERVAL_EXT, &interval);

    return static_cast<bool>(interval);
  }
#endif

  std::cerr << "Warning: Vertical synchronization unsupported." << std::endl;
  return false;
}

void Window::enableVerticalSync(bool value) const {
#if defined(_WIN32)
  if (wglGetExtensionsStringEXT())
    wglSwapIntervalEXT(static_cast<int>(value));
    return;
  }
#elif defined(__gnu_linux__)
  if (glXQueryExtensionsString(glXGetCurrentDisplay(), 0)) {
    glXSwapIntervalEXT(glXGetCurrentDisplay(), glXGetCurrentDrawable(), value);
    glXSwapIntervalMESA(static_cast<unsigned int>(value));
    return;
  }
#endif

  std::cerr << "Warning: Vertical synchronization unsupported." << std::endl;
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

void Window::updateCallbacks() const {
  // Keyboard inputs
  if (!std::get<0>(m_callbacks).empty()) {
    glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/) {
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

void Window::addOverlayLabel(std::string label) {
  m_overlay->addLabel(std::move(label));
}

void Window::addOverlayButton(std::string label, std::function<void()> action) {
  m_overlay->addButton(std::move(label), std::move(action));
}

void Window::addOverlayCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal) {
  m_overlay->addCheckbox(std::move(label), std::move(actionOn), std::move(actionOff), initVal);
}

void Window::addOverlayTextbox(std::string label, std::function<void(const std::string&)> callback) {
  m_overlay->addTextbox(std::move(label), std::move(callback));
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

bool Window::run(float deltaTime) {
  if (glfwWindowShouldClose(m_window))
    return false;

  glfwPollEvents();

  // Input callbacks should not be executed if the overlay requested keyboard focus
  if (!m_overlay || !m_overlay->hasKeyboardFocus()) {
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

  if (m_overlay)
    m_overlay->render();

  glfwSwapBuffers(m_window);

  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
  Renderer::clear(MaskType::COLOR, MaskType::DEPTH);

  return true;
}

Vec2f Window::recoverMousePosition() const {
  double xPos {}, yPos {};
  glfwGetCursorPos(m_window, &xPos, &yPos);

  return Vec2f({ static_cast<float>(xPos), static_cast<float>(yPos) });
}

void Window::setShouldClose() const {
  glfwSetWindowShouldClose(m_window, true);
}

void Window::close() {
  disableOverlay();
  glfwTerminate();
}

} // namespace Raz
