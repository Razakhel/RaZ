#include "RaZ/Render/Application.hpp"

namespace Raz {

Application::Application(WindowPtr window, ScenePtr scene)
  : m_window{ std::move(window) },
    m_scene{ std::move(scene) } {
  m_cameraUbo.bindUniformBlock(m_scene->getProgram(), "uboCameraMatrices", 0);
  m_cameraUbo.bindBufferBase(0);
}

bool Application::run() const {
  const bool windowRunning = m_window->run();

  sendViewMatrix(m_camera->getViewMatrix());
  sendInverseViewMatrix(m_camera->getViewMatrix().inverse());
  sendProjectionMatrix(m_camera->getProjectionMatrix());
  sendInverseProjectionMatrix(m_camera->getProjectionMatrix().inverse());
  sendCameraPosition(m_camera->getPosition());

  m_scene->render(m_camera);

  return windowRunning;
}

void Application::updateShaders() const {
  m_scene->getProgram().updateShaders();
  m_scene->load();
  m_scene->updateLights();
}

} // namespace Raz
