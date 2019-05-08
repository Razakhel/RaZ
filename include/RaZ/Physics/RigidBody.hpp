#pragma once

#ifndef RAZ_RIGIDBODY_HPP
#define RAZ_RIGIDBODY_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class RigidBody : public Component {
public:
  explicit RigidBody(float mass, float friction) : m_mass{ mass }, m_friction{ friction } {}

  float getMass() const { return m_mass; }
  float getFriction() const { return m_friction; }
  const Vec3f& getForces() const { return m_forces; }
  const Vec3f& getVelocity() const { return m_velocity; }

  void setVelocity(const Vec3f& velocity) { m_velocity = velocity; }

  void applyForces(const Vec3f& gravity) { m_forces = gravity; }

private:
  float m_mass {};
  float m_friction {};
  Vec3f m_forces {};
  Vec3f m_velocity {};
};

} // namespace Raz

#endif // RAZ_RIGIDBODY_HPP
