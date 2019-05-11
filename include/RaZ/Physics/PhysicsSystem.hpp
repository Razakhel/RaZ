#pragma once

#ifndef RAZ_PHYSICSSYSTEM_HPP
#define RAZ_PHYSICSSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class PhysicsSystem : public System {
public:
  PhysicsSystem();

  void setGravity(const Vec3f& gravity) { m_gravity = gravity; }

  bool update(float deltaTime) override;
  void destroy() override {}

private:
  Vec3f m_gravity = Vec3f({ 0.f, -9.80665f, 0.f });
};

} // namespace Raz

#endif // RAZ_PHYSICSSYSTEM_HPP

