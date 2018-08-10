#include <iostream>

#include "RaZ/Utils/Window.hpp"

namespace Raz {

namespace {

void GLAPIENTRY callbackDebugLog(GLenum source,
                                 GLenum type,
                                 unsigned int id,
                                 GLenum severity,
                                 int /*length*/,
                                 const char* message,
                                 const void* /*userParam*/) {
  std::cerr << "OpenGL Debug - ";

  switch (source) {
    case GL_DEBUG_SOURCE_API: std::cerr << "Source: OpenGL\t"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cerr << "Source: Windows\t"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader compiler\t"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: std::cerr << "Source: Third party\t"; break;
    case GL_DEBUG_SOURCE_APPLICATION: std::cerr << "Source: Application\t"; break;
    case GL_DEBUG_SOURCE_OTHER: std::cerr << "Source: Other\t"; break;
    default: break;
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR: std::cerr << "Type: Error\t"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated behavior\t"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "Type: Undefined behavior\t"; break;
    case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "Type: Portability\t"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "Type: Performance\t"; break;
    case GL_DEBUG_TYPE_OTHER: std::cerr << "Type: Other\t"; break;
    default: break;
  }

  std::cout << "ID: " << id << "\t";

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH: std::cerr << "Severity: High\t"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Severity: Medium\t"; break;
    case GL_DEBUG_SEVERITY_LOW: std::cerr << "Severity: Low\t"; break;
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

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
    std::cerr << "Error: Failed to initialize GLEW." << std::endl;

#if !defined(__APPLE__) // Setting the debug message callback provokes a crash on macOS
  glDebugMessageCallback(&callbackDebugLog, nullptr);
#endif
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  enableFaceCulling();
  glEnable(GL_DEPTH_TEST);
}

void Window::setIcon(const Image& img) const {
  const GLFWimage icon = { static_cast<int>(img.getWidth()),
                           static_cast<int>(img.getHeight()),
                           const_cast<unsigned char*>(static_cast<const uint8_t*>(img.getDataPtr())) };
  glfwSetWindowIcon(m_window, 1, &icon);
}

void Window::enableFaceCulling(bool value) const {
  if (value)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
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

  std::cerr << "Warning: Vertical synchronisation unsupported." << std::endl;
  return false;
}

void Window::enableVerticalSync(bool value) const {
#if defined(_WIN32)
  if (wglGetExtensionsStringEXT())
    wglSwapIntervalEXT(static_cast<int>(value));
  else
    std::cerr << "Warning: Vertical synchronisation unsupported." << std::endl;
#elif defined(__gnu_linux__)
  if (glXQueryExtensionsString(glXGetCurrentDisplay(), 0)) {
    glXSwapIntervalEXT(glXGetCurrentDisplay(), glXGetCurrentDrawable(), value);
    glXSwapIntervalMESA(static_cast<unsigned int>(value));
  } else {
    std::cerr << "Warning: Vertical synchronisation unsupported." << std::endl;
  }
#endif
}

void Window::addKeyCallback(Keyboard::Key key, std::function<void()> func) {
  std::get<0>(m_callbacks).emplace_back(key, func);
  glfwSetWindowUserPointer(m_window, &m_callbacks);

  updateCallbacks();
}

void Window::addMouseButtonCallback(Mouse::Button button, std::function<void()> func) {
  std::get<1>(m_callbacks).emplace_back(button, func);
  glfwSetWindowUserPointer(m_window, &m_callbacks);

  updateCallbacks();
}

void Window::addMouseScrollCallback(std::function<void(double, double)> func) {
  std::get<2>(m_callbacks) = std::move(func);
  glfwSetWindowUserPointer(m_window, &m_callbacks);

  updateCallbacks();
}

void Window::updateCallbacks() const {
  glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/) {
    const auto& keyCallbacks = std::get<0>(*static_cast<InputCallbacks*>(glfwGetWindowUserPointer(window)));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);

    for (const auto& callback : keyCallbacks) {
      if (key == callback.first && action != GLFW_RELEASE)
        callback.second();
    }
  });

  glfwSetMouseButtonCallback(m_window, [] (GLFWwindow* window, int button, int action, int /* mods */) {
    const auto& mouseCallbacks = std::get<1>(*static_cast<InputCallbacks*>(glfwGetWindowUserPointer(window)));

    for (const auto& callback : mouseCallbacks) {
      if (button == callback.first && action != GLFW_RELEASE)
        callback.second();
    }
  });

  glfwSetScrollCallback(m_window, [] (GLFWwindow* window, double xOffset, double yOffset) {
    const auto& scrollCallbacks = std::get<2>(*static_cast<InputCallbacks*>(glfwGetWindowUserPointer(window)));

    for (const auto& callback : scrollCallbacks) {
      callback.second(xOffset, yOffset);
    }
  });
}

void Window::addOverlayElement(OverlayElementType type, const std::string& text,
                               std::function<void()> actionOn, std::function<void()> actionOff) {
  m_overlay->addElement(type, text, std::move(actionOn), std::move(actionOff));
}

void Window::addOverlayText(const std::string& text) {
  m_overlay->addText(text);
}

void Window::addOverlayButton(const std::string& text, std::function<void()> action) {
  m_overlay->addButton(text, std::move(action));
}

void Window::addOverlayCheckbox(const std::string& text, bool initVal,
                                std::function<void()> actionOn, std::function<void()> actionOff) {
  m_overlay->addCheckbox(text, initVal, std::move(actionOn), std::move(actionOff));
}

void Window::addOverlayFrameTime(const std::string& formattedText) {
  m_overlay->addFrameTime(formattedText);
}

void Window::addOverlayFpsCounter(const std::string& formattedText) {
  m_overlay->addFpsCounter(formattedText);
}

bool Window::run() const {
  if (glfwWindowShouldClose(m_window))
    return false;

  glfwPollEvents();

  if (m_overlay)
    m_overlay->render();

  glfwSwapBuffers(m_window);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return true;
}

void Window::close() {
  disableOverlay();
  glfwTerminate();
}

} // namespace Raz
