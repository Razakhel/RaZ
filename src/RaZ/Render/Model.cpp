#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Render/Model.hpp"

namespace Raz {

void Model::draw() const {
  m_material.getShaderProgram().use();
  m_material.getTexture()->bind();

  updatePosition();
  m_mesh->draw();
}

void Model::translate(float x, float y, float z) {
  m_position[3] += x;
  m_position[7] += y;
  m_position[11] += z;

  updatePosition();
}

void Model::rotate(float angle, float x, float y, float z) {
  const Quaternion<float> quaternion(angle * static_cast<float>(M_PI) / 180.f, x, y, z);
  m_position = m_position * quaternion.computeMatrix();

  updatePosition();
}

void Model::scale(float x, float y, float z) {
  m_position[0] *= x;
  m_position[5] *= y;
  m_position[10] *= z;

  updatePosition();
}

void Model::updatePosition() const {
  const GLint location = glGetUniformLocation(m_material.getShaderProgram().getIndex(), "uniTransform");
  glUniformMatrix4fv(location, 1, GL_TRUE, m_position.getData().data());
}

} // namespace Raz
