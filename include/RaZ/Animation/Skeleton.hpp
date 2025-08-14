#pragma once

#ifndef RAZ_SKELETON_HPP
#define RAZ_SKELETON_HPP

#include "RaZ/Data/Graph.hpp"
#include "RaZ/Math/Quaternion.hpp"

namespace Raz {

template <typename T, std::size_t W, std::size_t H>
class Matrix;
using Mat4f = Matrix<float, 4, 4>;

class SkeletonJoint final : public GraphNode<SkeletonJoint> {
public:
  SkeletonJoint() = default;
  SkeletonJoint(const Quaternionf& rotation, const Vec3f& translation) noexcept : m_rotation{ rotation }, m_translation{ translation } {}
  SkeletonJoint(const SkeletonJoint&) = delete;
  SkeletonJoint(SkeletonJoint&&) noexcept = default;

  constexpr const Quaternionf& getRotation() const noexcept { return m_rotation; }
  constexpr const Vec3f& getTranslation() const noexcept { return m_translation; }

  void setRotation(const Quaternionf& rotation) { m_rotation = rotation; }
  void setTranslation(const Vec3f& translation) { m_translation = translation; }

  void rotate(const Quaternionf& rotation);
  void translate(const Vec3f& translation) { m_translation += translation; }
  Mat4f computeTransformMatrix() const;

  SkeletonJoint& operator=(const SkeletonJoint&) = delete;
  SkeletonJoint& operator=(SkeletonJoint&& joint) noexcept = default;

private:
  Quaternionf m_rotation = Quaternionf::identity();
  Vec3f m_translation {};
};

using Skeleton = Graph<SkeletonJoint>;

} // namespace Raz

#endif // RAZ_SKELETON_HPP
