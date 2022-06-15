#pragma once

#ifndef RAZ_RIGIDBODY_HPP
#define RAZ_RIGIDBODY_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class RigidBody final : public Component {
  friend class PhysicsSystem;

public:
  /// Creates a rigid body with given mass & bounciness.
  /// \param mass Mass of the rigid body. A value less than or equal to 0 represents an infinite mass.
  /// \param bounciness Coefficient of restitution (must be between 0 & 1).
  RigidBody(float mass, float bounciness) noexcept;

  constexpr float getMass() const noexcept { return m_mass; }
  constexpr float getInvMass() const noexcept { return m_invMass; }
  constexpr float getBounciness() const noexcept { return m_bounciness; }
  constexpr const Vec3f& getForces() const noexcept { return m_forces; }
  constexpr const Vec3f& getVelocity() const noexcept { return m_velocity; }

  void setMass(float mass) noexcept;
  void setBounciness(float bounciness) noexcept;
  template <typename... Args> constexpr void setForces(const Args&... forces) noexcept { m_forces = (forces + ...); }
  constexpr void setVelocity(const Vec3f& velocity) noexcept { m_velocity = velocity; }

private:
  float m_mass {}; ///< Mass of the rigid body.
  float m_invMass {}; ///< Inverse mass of the rigid body.
  float m_bounciness {}; ///< Coefficient of restitution, determining the amount of energy kept by the rigid body when bouncing off.

  Vec3f m_forces {}; ///< Additional forces applied to the rigid body; gravity is computed independently later.
  Vec3f m_velocity {}; ///< Velocity of the rigid body.
  Vec3f m_oldPosition {}; ///< Previous position of the rigid body.
};

} // namespace Raz

#endif // RAZ_RIGIDBODY_HPP
