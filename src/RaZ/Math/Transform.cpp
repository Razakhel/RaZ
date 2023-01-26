#include "RaZ/Math/Transform.hpp"

namespace Raz {

void Transform::setPosition(const Vec3f& position) {
  m_position = position;
  m_updated  = true;
}

void Transform::setRotation(const Quaternionf& rotation) {
  m_rotation = rotation;
  m_updated  = true;
}

void Transform::setScale(const Vec3f& scale) {
  m_scale   = scale;
  m_updated = true;
}

void Transform::translate(float x, float y, float z) {
  m_position.x() += x;
  m_position.y() += y;
  m_position.z() += z;

  m_updated = true;
}

void Transform::rotate(const Quaternionf& rotation) {
  m_rotation *= rotation;
  m_updated   = true;
}

void Transform::rotate(Radiansf angle, const Vec3f& axis) {
  assert("Error: Rotation axis must be normalized." && FloatUtils::areNearlyEqual(axis.computeLength(), 1.f));
  rotate(Quaternionf(angle, axis));
}

void Transform::rotate(Radiansf xAngle, Radiansf yAngle) {
  const Quaternionf xQuat(xAngle, Axis::X);
  const Quaternionf yQuat(yAngle, Axis::Y);
  m_rotation = yQuat * m_rotation * xQuat;

  m_updated = true;
}

void Transform::rotate(Radiansf xAngle, Radiansf yAngle, Radiansf zAngle) {
  const Quaternionf xQuat(xAngle, Axis::X);
  const Quaternionf yQuat(yAngle, Axis::Y);
  const Quaternionf zQuat(zAngle, Axis::Z);
  m_rotation *= zQuat * xQuat * yQuat;

  m_updated = true;
}

void Transform::scale(float x, float y, float z) {
  m_scale.x() *= x;
  m_scale.y() *= y;
  m_scale.z() *= z;

  m_updated = true;
}

Mat4f Transform::computeTranslationMatrix(bool reverseTranslation) const {
  const Vec3f translation = (reverseTranslation ? -m_position : m_position);
  const Mat4f translationMat(1.f, 0.f, 0.f, translation.x(),
                             0.f, 1.f, 0.f, translation.y(),
                             0.f, 0.f, 1.f, translation.z(),
                             0.f, 0.f, 0.f,             1.f);

  return translationMat;
}

Mat4f Transform::computeTransformMatrix() const {
  const Mat4f scale(m_scale.x(), 0.f,         0.f,         0.f,
                    0.f,         m_scale.y(), 0.f,         0.f,
                    0.f,         0.f,         m_scale.z(), 0.f,
                    0.f,         0.f,         0.f,         1.f);

  return computeTranslationMatrix() * m_rotation.computeMatrix() * scale;
}

} // namespace Raz
