#include "RaZ/Math/Transform.hpp"
#include "RaZ/Math/Quaternion.hpp"

namespace Raz {

void Transform::translate(float x, float y, float z) {
  m_position[0] += x;
  m_position[1] += y;
  m_position[2] += z;
}

void Transform::rotate(float angle, float x, float y, float z) {
  const Quaternion<float> quaternion(angle, x, y, z);
  m_rotation = m_rotation * quaternion.computeMatrix();
}

void Transform::scale(float x, float y, float z) {
  m_scale[0] *= x;
  m_scale[1] *= y;
  m_scale[2] *= z;
}

Mat4f Transform::computeTransformMatrix() const {
  Mat4f transform({{ m_scale[0],        0.f,        0.f, 0.f },
                   {        0.f, m_scale[1],        0.f, 0.f },
                   {        0.f,        0.f, m_scale[2], 0.f },
                   {        0.f,        0.f,        0.f, 1.f }});

  transform = transform * m_rotation;

  transform[3] = m_position[0];
  transform[7] = m_position[1];
  transform[11] = m_position[2];

  return transform;
}

} // namespace Raz
