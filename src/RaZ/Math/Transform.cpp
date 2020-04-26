#include "RaZ/Math/Transform.hpp"

namespace Raz {

void Transform::setPosition(const Vec3f& position) {
  m_position = position;
  m_updated  = true;
}

void Transform::setRotation(const Mat4f& rotation) {
  m_rotation = rotation;
  m_updated  = true;
}

void Transform::setScale(const Vec3f& scale) {
  m_scale   = scale;
  m_updated = true;
}

void Transform::translate(float x, float y, float z) {
  m_position[0] += x;
  m_position[1] += y;
  m_position[2] += z;

  m_updated = true;
}

void Transform::rotate(Radiansf angle, const Vec3f& axis) {
  assert("Error: Rotation axis must be normalized." && FloatUtils::areNearlyEqual(axis.computeLength(), 1.f));

  const Quaternionf quaternion(angle, axis);
  m_rotation = quaternion.computeMatrix() * m_rotation;

  m_updated = true;
}

void Transform::rotate(Radiansf xAngle, Radiansf yAngle, Radiansf zAngle) {
  const Quaternionf xQuat(xAngle, Axis::X);
  const Quaternionf yQuat(yAngle, Axis::Y);
  const Quaternionf zQuat(zAngle, Axis::Z);
  m_rotation = (xQuat * yQuat * zQuat).computeMatrix() * m_rotation;

  m_updated = true;
}

void Transform::scale(float x, float y, float z) {
  m_scale[0] *= x;
  m_scale[1] *= y;
  m_scale[2] *= z;

  m_updated = true;
}

Mat4f Transform::computeTranslationMatrix(bool reverseTranslation) const {
  const Vec3f translation = (reverseTranslation ? -m_position : m_position);
  const Mat4f translationMat(1.f,            0.f,            0.f,            0.f,
                             0.f,            1.f,            0.f,            0.f,
                             0.f,            0.f,            1.f,            0.f,
                             translation[0], translation[1], translation[2], 1.f);

  return translationMat;
}

Mat4f Transform::computeTransformMatrix() const {
  const Mat4f scale(m_scale[0], 0.f,        0.f,        0.f,
                    0.f,        m_scale[1], 0.f,        0.f,
                    0.f,        0.f,        m_scale[2], 0.f,
                    0.f,        0.f,        0.f,        1.f);

  return scale * m_rotation * computeTranslationMatrix();
}

} // namespace Raz
