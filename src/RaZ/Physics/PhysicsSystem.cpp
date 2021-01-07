#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/RigidBody.hpp"
#include "RaZ/Physics/PhysicsSystem.hpp"

namespace Raz {

PhysicsSystem::PhysicsSystem() {
  m_acceptedComponents.setBit(Component::getId<RigidBody>());
}

bool PhysicsSystem::step(float deltaTime) {
  for (Entity* entity : m_entities) {
    if (!entity->isEnabled() || !entity->hasComponent<RigidBody>())
      continue;

    auto& rigidBody = entity->getComponent<RigidBody>();
    auto& transform = entity->getComponent<Transform>();

    rigidBody.m_oldPosition = transform.getPosition();
    rigidBody.applyForces(m_gravity);

    const Vec3f acceleration = rigidBody.getForces() * rigidBody.getInvMass();
    const Vec3f oldVelocity  = rigidBody.getVelocity();

    const Vec3f velocity = oldVelocity * m_friction + acceleration * deltaTime;
    rigidBody.setVelocity(velocity);

    transform.translate((oldVelocity + velocity) * 0.5f * deltaTime);
  }

  return true;
}

} // namespace Raz
