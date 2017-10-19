#include <cmath>

#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Render/Model.hpp"

namespace Raz {

void Model::draw() const {
  m_material.getShaderProgram().use();
  m_mesh->draw();
}

void Model::translate(float x, float y, float z) {
  m_position[3] += x;
  m_position[7] += y;
  m_position[11] += z;

  updatePosition();
}

void Model::rotate(float angle, float x, float y, float z) {
  const Quaternion<float> quaternion(angle * M_PI / 180, x, y, z);

  m_position = m_position * quaternion.computeMatrix();

  updatePosition();
}

void Model::updatePosition() const {
  const GLint location = glGetUniformLocation(m_material.getShaderProgram().getIndex(), "uniTransform");
  glUniformMatrix4fv(location, 1, GL_TRUE, m_position.getData().data());
}

} // namespace Raz
