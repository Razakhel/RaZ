#include "RaZ/Render/Scene.hpp"

namespace Raz {

void Scene::load() const {
  for (const auto& model : m_models)
    model->load(m_program);
}

void Scene::render(const CameraPtr& camera) const {
  const Mat4f& viewMat    = camera->getViewMatrix();
  const Mat4f& projMat    = camera->getProjectionMatrix();
  const Mat4f viewProjMat = projMat * viewMat;

  m_program.use();
  m_program.sendUniform("uniCameraPos", camera->getPosition());

  for (const auto& model : m_models) {
    const auto modelMat = model->computeTransformMatrix();

    m_program.sendUniform("uniModelMatrix", modelMat);
    m_program.sendUniform("uniMvpMatrix", viewProjMat * modelMat);

    model->draw(m_program);
  }

  if (m_cubemap) {
    m_cubemap->getProgram().use();
    m_cubemap->getProgram().sendUniform("uniViewMatrix", Raz::Mat4f(Raz::Mat3f(viewMat)));
    m_cubemap->getProgram().sendUniform("uniProjectionMatrix", projMat);

    m_cubemap->draw();
  }
}

void Scene::updateLights() const {
  m_program.use();

  m_program.sendUniform("uniLightCount", m_lights.size());

  for (std::size_t lightIndex = 0; lightIndex < m_lights.size(); ++lightIndex) {
    const std::string locationBase = "uniLights[" + std::to_string(lightIndex) + "].";

    const std::string posLocation    = locationBase + "position";
    const std::string dirLocation    = locationBase + "direction";
    const std::string colorLocation  = locationBase + "color";
    const std::string energyLocation = locationBase + "energy";
    const std::string angleLocation  = locationBase + "angle";

    m_program.sendUniform(posLocation,    m_lights[lightIndex]->getHomogeneousPosition());
    m_program.sendUniform(dirLocation,    m_lights[lightIndex]->getDirection());
    m_program.sendUniform(colorLocation,  m_lights[lightIndex]->getColor());
    m_program.sendUniform(energyLocation, m_lights[lightIndex]->getEnergy());
    m_program.sendUniform(angleLocation,  m_lights[lightIndex]->getAngle());
  }
}

} // namespace Raz
