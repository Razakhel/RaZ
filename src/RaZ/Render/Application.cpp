#include "RaZ/Render/Application.hpp"

namespace Raz {

Application::Application(WindowPtr window, ScenePtr scene, CameraPtr camera)
  : m_window{ std::move(window) },
    m_scene{ std::move(scene) },
    m_camera{ std::move(camera) } {
  m_cameraUbo.bindUniformBlock(m_scene->getProgram(), "uboCameraMatrices", 0);
  m_cameraUbo.bindBufferBase(0);
}

bool Application::run() const {
  const bool windowRunning = m_window->run();

  m_scene->render(m_camera);

  return windowRunning;
}

void Application::updateShaders() const {
  m_scene->getProgram().updateShaders();
  m_scene->load();
  m_scene->updateLights();
}

} // namespace Raz
