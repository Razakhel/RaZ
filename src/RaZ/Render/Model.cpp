#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Render/Model.hpp"

namespace Raz {

void Model::draw() const {
  m_material.getShaderProgram().use();
  m_material.getTexture()->bind();

  updateTransform();
  m_mesh->draw();
}

void Model::translate(float x, float y, float z) {
  m_transform[3] += x;
  m_transform[7] += y;
  m_transform[11] += z;
}

void Model::rotate(float angle, float x, float y, float z) {
  const Quaternion<float> quaternion(angle * static_cast<float>(M_PI) / 180.f, x, y, z);
  m_transform = m_transform * quaternion.computeMatrix();
}

void Model::scale(float x, float y, float z) {
  m_transform[0] *= x;
  m_transform[5] *= y;
  m_transform[10] *= z;
}

void Model::updateTransform() const {
  const GLint uniTransformLocation = glGetUniformLocation(m_material.getShaderProgram().getIndex(), "uniTransform");
  glUniformMatrix4fv(uniTransformLocation, 1, GL_TRUE, m_transform.getData().data());
}

} // namespace Raz
