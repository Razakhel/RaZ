#pragma once

#ifndef RAZ_PHYSICSSYSTEM_HPP
#define RAZ_PHYSICSSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class PhysicsSystem final : public System {
public:
  PhysicsSystem();

  void setGravity(const Vec3f& gravity) { m_gravity = gravity; }
  void setFriction(float friction) {
    assert("Error: Friction coefficient must be between 0 & 1." && (friction >= 0.f && friction <= 1.f));
    m_friction = friction;
  }

  bool update(float deltaTime) override;
  void destroy() override {}

private:
  Vec3f m_gravity  = Vec3f(0.f, -9.80665f, 0.f); ///< Gravity force.
  float m_friction = 0.95f; ///< Friction coefficient.
};

} // namespace Raz

#endif // RAZ_PHYSICSSYSTEM_HPP

