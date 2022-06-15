#include "RaZ/Physics/RigidBody.hpp"

namespace Raz {

RigidBody::RigidBody(float mass, float bounciness) noexcept {
  setMass(mass);
  setBounciness(bounciness);
}

void RigidBody::setMass(float mass) noexcept {
  m_mass    = mass;
  m_invMass = (mass > 0.f ? 1.f / mass : 0.f);
}

void RigidBody::setBounciness(float bounciness) noexcept {
  assert("Error: The bounciness value must be between 0 & 1." && (bounciness >= 0.f && bounciness <= 1.f));
  m_bounciness = bounciness;
}

} // namespace Raz
