#pragma once

#ifndef RAZ_PHYSICSSYSTEM_HPP
#define RAZ_PHYSICSSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class PhysicsSystem final : public System {
public:
  PhysicsSystem();

  constexpr const Vec3f& getGravity() const noexcept { return m_gravity; }
  constexpr float getFriction() const noexcept { return m_friction; }

  void setGravity(const Vec3f& gravity) { m_gravity = gravity; }
  void setFriction(float friction) {
    assert("Error: Friction coefficient must be between 0 & 1." && (friction >= 0.f && friction <= 1.f));
    m_friction = friction;
  }

  bool step(float deltaTime) override;

private:
  void solveConstraints();

  Vec3f m_gravity  = Vec3f(0.f, -9.80665f, 0.f); ///< Gravity acceleration.
  float m_friction = 0.95f; ///< Friction coefficient.
};

} // namespace Raz

#endif // RAZ_PHYSICSSYSTEM_HPP
