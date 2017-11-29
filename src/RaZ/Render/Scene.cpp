#include "RaZ/Render/Scene.hpp"

namespace Raz {

void Scene::render() const {
  m_shaderProgram.use();
  const GLint uniMvpLocation = glGetUniformLocation(m_shaderProgram.getIndex(), "uniMvpMatrix");

  for (const auto& model : m_models) {
    const Raz::Mat4f mvpMat = m_camera->computePerspectiveMatrix()    // Projection
                            * m_camera->lookAt(model->getPosition())  // View
                            * model->computeTransformMatrix();        // Model

    glUniformMatrix4fv(uniMvpLocation, 1, GL_FALSE, mvpMat.getDataPtr());

    model->draw();
  }
}

void Scene::updateLights() const {
  m_shaderProgram.use();
  const GLuint programIndex = m_shaderProgram.getIndex();

  const GLint uniLightCountLocation = glGetUniformLocation(programIndex, "uniLightCount");
  glUniform1ui(uniLightCountLocation, m_lights.size());

  for (std::size_t lightIndex = 0; lightIndex < m_lights.size(); ++lightIndex) {
    const std::string locationBase = "uniLights[" + std::to_string(lightIndex) + "].";
    const std::string posLocation = locationBase + "position";
    const std::string dirLocation = locationBase + "direction";
    const std::string colorLocation = locationBase + "color";
    const std::string angleLocation = locationBase + "angle";

    const GLint uniPositionLocation = glGetUniformLocation(programIndex, posLocation.c_str());
    glUniform4fv(uniPositionLocation, 1, m_lights[lightIndex].getPosition().getDataPtr());

    const GLint uniDirectionLocation = glGetUniformLocation(programIndex, dirLocation.c_str());
    glUniform3fv(uniDirectionLocation, 1, m_lights[lightIndex].getDirection().getDataPtr());

    const GLint uniColorLocation = glGetUniformLocation(programIndex, colorLocation.c_str());
    glUniform3fv(uniColorLocation, 1, m_lights[lightIndex].getColor().getDataPtr());

    const GLint uniAngleLocation = glGetUniformLocation(programIndex, angleLocation.c_str());
    glUniform1f(uniAngleLocation, m_lights[lightIndex].getAngle());
  }
}

} // namespace Raz
