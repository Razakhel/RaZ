#pragma once

#ifndef RAZ_KEYFRAME_HPP
#define RAZ_KEYFRAME_HPP

#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Math/Vector.hpp"

#include <vector>

namespace Raz {

class JointTransform {
public:
  constexpr JointTransform() = default;
  constexpr explicit JointTransform(const Quaternionf& rotation, const Vec3f& translation = Raz::Vec3f(0.f))
    : m_rotation{ rotation }, m_translation{ translation } {}

  constexpr const Quaternionf& getRotation() const noexcept { return m_rotation; }
  constexpr const Vec3f& getTranslation() const noexcept { return m_translation; }

  constexpr void setRotation(const Quaternionf& rotation) { m_rotation = rotation; }
  constexpr void setTranslation(const Vec3f& translation) { m_translation = translation; }

  constexpr void lerp(const JointTransform& jointTransform, float coeff) noexcept;
  constexpr void nlerp(const JointTransform& jointTransform, float coeff) noexcept;

private:
  Quaternionf m_rotation = Quaternionf::identity();
  Vec3f m_translation {};
};

constexpr void JointTransform::lerp(const JointTransform& jointTransform, float coeff) noexcept {
  m_rotation    = m_rotation.lerp(jointTransform.m_rotation, coeff);
  m_translation = m_translation.lerp(jointTransform.m_translation, coeff);
}

constexpr void JointTransform::nlerp(const JointTransform& jointTransform, float coeff) noexcept {
  m_rotation    = m_rotation.nlerp(jointTransform.m_rotation, coeff);
  m_translation = m_translation.nlerp(jointTransform.m_translation, coeff);
}

class Keyframe {
public:
  Keyframe() = default;

  std::size_t getJointTransformCount() const noexcept { return m_transforms.size(); }
  const JointTransform& getJointTransform(std::size_t index) const noexcept { return m_transforms[index]; }
  JointTransform& getJointTransform(std::size_t index) noexcept { return m_transforms[index]; }

  template <typename... Args>
  void addJointTransform(Args&&... args) { m_transforms.emplace_back(std::forward<Args>(args)...); }

private:
  std::vector<JointTransform> m_transforms {};
};

} // namespace Raz

#endif // RAZ_KEYFRAME_HPP
