#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Math/Matrix.hpp"

namespace Raz {

void SkeletonJoint::rotate(const Quaternionf& rotation) {
  m_rotation *= rotation;

  for (SkeletonJoint* child : m_children)
    child->rotate(rotation);
}

Mat4f SkeletonJoint::computeTransformMatrix() const {
  Mat4f transformMat = m_rotation.computeMatrix();
  transformMat[12]   = m_translation.x();
  transformMat[13]   = m_translation.y();
  transformMat[14]   = m_translation.z();

  return transformMat;
}

} // namespace Raz
