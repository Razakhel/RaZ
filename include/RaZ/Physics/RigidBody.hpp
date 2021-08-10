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
  /// \param mass Mass of the rigid body. 0 represents an infinite mass.
  /// \param bounciness Coefficient of restitution (must be between 0 & 1).
  constexpr RigidBody(float mass, float bounciness) noexcept : m_mass{ mass }, m_invMass{ (mass != 0.f ? 1.f / mass : 0.f) } { setBounciness(bounciness); }

  constexpr float getMass() const noexcept { return m_mass; }
  constexpr float getInvMass() const noexcept { return m_invMass; }
  constexpr float getBounciness() const noexcept { return m_bounciness; }
  constexpr const Vec3f& getForces() const noexcept { return m_forces; }
  constexpr const Vec3f& getVelocity() const noexcept { return m_velocity; }

  constexpr void setMass(float mass) noexcept { m_mass = mass; }
  constexpr void setBounciness(float bounciness) noexcept {
    assert("Error: The bounciness value must be between 0 & 1." && (bounciness >= 0.f && bounciness <= 1.f));
    m_bounciness = bounciness;
  }
  constexpr void setVelocity(const Vec3f& velocity) noexcept { m_velocity = velocity; }

  constexpr void applyForces(const Vec3f& gravity) noexcept { m_forces = m_mass * gravity; }

private:
  float m_mass {}; ///< Mass of the rigid body.
  float m_invMass {}; ///< Inverse mass of the rigid body.
  float m_bounciness {}; ///< Coefficient of restitution, determining the amount of energy kept by the rigid body when bouncing off.

  Vec3f m_forces {}; ///< Forces applied to the rigid body.
  Vec3f m_velocity {}; ///< Velocity of the rigid body.
  Vec3f m_oldPosition {}; ///< Previous position of the rigid body.
};

} // namespace Raz

#endif // RAZ_RIGIDBODY_HPP
