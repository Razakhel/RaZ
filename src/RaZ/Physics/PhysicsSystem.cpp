#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/RigidBody.hpp"
#include "RaZ/Physics/PhysicsSystem.hpp"

namespace Raz {

PhysicsSystem::PhysicsSystem() {
  m_acceptedComponents.setBit(Component::getId<RigidBody>());
}

bool PhysicsSystem::update(float deltaTime) {
  for (Entity* entity : m_entities) {
    if (entity->isEnabled()) {
      auto& rigidBody = entity->getComponent<RigidBody>();
      rigidBody.applyForces(m_gravity);

      const Vec3f acceleration = rigidBody.getForces() * (1.f / rigidBody.getMass());
      const Vec3f oldVelocity  = rigidBody.getVelocity();

      const Vec3f velocity = oldVelocity * rigidBody.getFriction() + acceleration * deltaTime;
      rigidBody.setVelocity(velocity);

      entity->getComponent<Transform>().translate((oldVelocity + velocity) * 0.5f * deltaTime);
    }
  }

  return true;
}

} // namespace Raz
