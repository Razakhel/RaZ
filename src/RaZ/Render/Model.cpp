#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Render/Model.hpp"

namespace Raz {

void Model::draw() const {
  m_material.getShaderProgram().use();
  m_material.getTexture()->bind();

  updateTransform();
  m_mesh->draw();
}

void Model::updateTransform() const {
  const GLint uniTransformLocation = glGetUniformLocation(m_material.getShaderProgram().getIndex(), "uniTransform");
  glUniformMatrix4fv(uniTransformLocation, 1, GL_TRUE, m_transform.computeTransformMatrix().getDataPtr());
}

} // namespace Raz
