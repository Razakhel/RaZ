#include "RaZ/Animation/Skeleton.hpp"

namespace Raz {

Mat4f SkeletonJoint::computeTransformMatrix() const {
  Mat4f transformMat = m_rotation.computeMatrix();
  transformMat[12]   = m_translation[0];
  transformMat[13]   = m_translation[1];
  transformMat[14]   = m_translation[2];

  return transformMat;
}

void SkeletonJoint::rotate(const Quaternionf& rotation) {
  m_rotation *= rotation;

  for (SkeletonJoint* child : m_children)
    child->rotate(rotation);
}

} // namespace Raz
