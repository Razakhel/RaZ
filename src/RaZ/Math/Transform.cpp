#include "RaZ/Math/Transform.hpp"
#include "RaZ/Math/Quaternion.hpp"

namespace Raz {

Mat4f Transform::computeTranslationMatrix(bool inverseTranslation) const {
  const Vec3f translation = (inverseTranslation ? -m_position : m_position);
  const Mat4f translationMat({{            1.f,            0.f,            0.f, 0.f },
                              {            0.f,            1.f,            0.f, 0.f },
                              {            0.f,            0.f,            1.f, 0.f },
                              { translation[0], translation[1], translation[2], 1.f }});

  return translationMat;
}

void Transform::translate(float x, float y, float z) {
  m_position[0] += x;
  m_position[1] += y;
  m_position[2] += z;
}

void Transform::rotate(float angle, float x, float y, float z) {
  const Quaternionf quaternion(angle, x, y, z);
  m_rotation = m_rotation * quaternion.computeMatrix();
}

void Transform::scale(float x, float y, float z) {
  m_scale[0] *= x;
  m_scale[1] *= y;
  m_scale[2] *= z;
}

Mat4f Transform::computeTransformMatrix() const {
  const Mat4f scale({{ m_scale[0],        0.f,        0.f, 0.f },
                     {        0.f, m_scale[1],        0.f, 0.f },
                     {        0.f,        0.f, m_scale[2], 0.f },
                     {        0.f,        0.f,        0.f, 1.f }});

  return computeTranslationMatrix() * m_rotation * scale;
}

} // namespace Raz
