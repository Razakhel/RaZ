#include <iostream>

#include "glew/include/GL/glew.h"
#if defined(_WIN32)
#include "glew/include/GL/wglew.h"
#elif defined(__gnu_linux__)
#include "glew/include/GL/glxew.h"
#endif
#include "RaZ/Utils/Window.hpp"

namespace Raz {

namespace {

void GLAPIENTRY callbackDebugLog(GLenum source,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei/* length*/,
                                 const GLchar* message,
                                 const void*/* userParam*/) {
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
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, AASampleCount);

  m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!m_window) {
    close();
    throw std::runtime_error("Error: Failed to create GLFW Window");
  }

  glfwMakeContextCurrent(m_window);
  updateKeyCallbacks();

  glViewport(0, 0, width, height);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
    std::cerr << "Error: Failed to initialize GLEW." << std::endl;

  glDebugMessageCallback(&callbackDebugLog, nullptr);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void Window::enableVerticalSync(bool value) {
#if defined(_WIN32)
  if (wglGetExtensionsStringEXT())
    wglSwapIntervalEXT(value);
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
  m_keyCallbacks.emplace_back(key, func);
  glfwSetWindowUserPointer(m_window, &m_keyCallbacks);

  updateKeyCallbacks();
}

void Window::updateKeyCallbacks() const {
  glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int/* scancode*/, int action, int/* mode*/) {
    const CallbacksList& callbackList = *static_cast<CallbacksList*>(glfwGetWindowUserPointer(window));

    for (const auto& callback : callbackList) {
      if (key == callback.first && action != GLFW_RELEASE)
        callback.second();
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  });
}

bool Window::run() const {
  if (glfwWindowShouldClose(m_window))
    return false;

  glfwSwapBuffers(m_window);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwPollEvents();

  return true;
}

} // namespace Raz
