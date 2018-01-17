#include "RaZ/Render/Scene.hpp"

namespace Raz {

void Scene::render(const Framebuffer* framebuffer) const {
  const Mat4f projectionMat = m_camera->computePerspectiveMatrix();
  const Mat4f viewMat = m_camera->lookAt(m_models.front()->getPosition());

  if (framebuffer) {
    framebuffer->getProgram().use();

    const GLint uniProjectionLocation = glGetUniformLocation(framebuffer->getProgram().getIndex(), "uniProjectionMatrix");
    const GLint uniInvProjLocation = glGetUniformLocation(framebuffer->getProgram().getIndex(), "uniInvProjMatrix");
    const GLint uniViewLocation = glGetUniformLocation(framebuffer->getProgram().getIndex(), "uniViewMatrix");
    const GLint uniInvViewLocation = glGetUniformLocation(framebuffer->getProgram().getIndex(), "uniInvViewMatrix");

    const Mat4f invProjMat = projectionMat.inverse();
    const Mat4f invViewMat = viewMat.inverse();

    glUniformMatrix4fv(uniProjectionLocation, 1, GL_FALSE, projectionMat.getDataPtr());
    glUniformMatrix4fv(uniInvProjLocation, 1, GL_FALSE, invProjMat.getDataPtr());
    glUniformMatrix4fv(uniViewLocation, 1, GL_FALSE, viewMat.getDataPtr());
    glUniformMatrix4fv(uniInvViewLocation, 1, GL_FALSE, invViewMat.getDataPtr());
  }

  m_program.use();

  const GLint uniViewProjLocation = glGetUniformLocation(m_program.getIndex(), "uniViewProjMatrix");
  const GLint uniMvpLocation = glGetUniformLocation(m_program.getIndex(), "uniMvpMatrix");

  const Mat4f viewProjMat = projectionMat * viewMat;

  glUniformMatrix4fv(uniViewProjLocation, 1, GL_FALSE, viewProjMat.getDataPtr());

  for (const auto& model : m_models) {
    const Mat4f mvpMat = viewProjMat * model->computeTransformMatrix();

    glUniformMatrix4fv(uniMvpLocation, 1, GL_FALSE, mvpMat.getDataPtr());

    model->draw();
  }

  updateLights();
}

void Scene::updateLights() const {
  const GLuint programIndex = m_program.getIndex();

  const GLint uniLightCountLocation = glGetUniformLocation(programIndex, "uniLightCount");
  glUniform1ui(uniLightCountLocation, m_lights.size());

  for (std::size_t lightIndex = 0; lightIndex < m_lights.size(); ++lightIndex) {
    const std::string locationBase = "uniLights[" + std::to_string(lightIndex) + "].";
    const std::string posLocation = locationBase + "position";
    const std::string dirLocation = locationBase + "direction";
    const std::string colorLocation = locationBase + "color";
    const std::string angleLocation = locationBase + "angle";

    const GLint uniPositionLocation = glGetUniformLocation(programIndex, posLocation.c_str());
    glUniform4fv(uniPositionLocation, 1, m_lights[lightIndex]->getHomogeneousPosition().getDataPtr());

    const GLint uniDirectionLocation = glGetUniformLocation(programIndex, dirLocation.c_str());
    glUniform3fv(uniDirectionLocation, 1, m_lights[lightIndex]->getDirection().getDataPtr());

    const GLint uniColorLocation = glGetUniformLocation(programIndex, colorLocation.c_str());
    glUniform3fv(uniColorLocation, 1, m_lights[lightIndex]->getColor().getDataPtr());

    const GLint uniAngleLocation = glGetUniformLocation(programIndex, angleLocation.c_str());
    glUniform1f(uniAngleLocation, m_lights[lightIndex]->getAngle());
  }
}

} // namespace Raz
