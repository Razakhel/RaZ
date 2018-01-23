#include "RaZ/Render/Scene.hpp"

namespace Raz {

void Scene::render(const Mat4f& viewProjMat) const {
  m_program.use();

  for (const auto& model : m_models) {
    m_program.sendUniform("uniMvpMatrix", viewProjMat * model->computeTransformMatrix());

    model->draw();
  }
}

void Scene::updateLights() const {
  m_program.use();

  m_program.sendUniform("uniLightCount", m_lights.size());

  for (std::size_t lightIndex = 0; lightIndex < m_lights.size(); ++lightIndex) {
    const std::string locationBase = "uniLights[" + std::to_string(lightIndex) + "].";

    const std::string posLocation = locationBase + "position";
    const std::string dirLocation = locationBase + "direction";
    const std::string colorLocation = locationBase + "color";
    const std::string angleLocation = locationBase + "angle";

    m_program.sendUniform(posLocation, m_lights[lightIndex]->getHomogeneousPosition());
    m_program.sendUniform(dirLocation, m_lights[lightIndex]->getDirection());
    m_program.sendUniform(colorLocation, m_lights[lightIndex]->getColor());
    m_program.sendUniform(angleLocation, m_lights[lightIndex]->getAngle());
  }
}

} // namespace Raz
