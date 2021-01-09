#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/Collider.hpp"
#include "RaZ/Physics/RigidBody.hpp"
#include "RaZ/Physics/PhysicsSystem.hpp"

namespace Raz {

PhysicsSystem::PhysicsSystem() {
  m_acceptedComponents.setBit(Component::getId<Collider>());
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

  solveConstraints();

  return true;
}

void PhysicsSystem::solveConstraints() {
  for (Entity* entity : m_entities) {
    if (!entity->isEnabled() || !entity->hasComponent<RigidBody>())
      continue;

    auto& rigidBody = entity->getComponent<RigidBody>();
    auto& transform = entity->getComponent<Transform>();

    const Vec3f velocity    = rigidBody.getVelocity();
    const Vec3f velocityDir = velocity.normalize();

    for (Entity* collidableEntity : m_entities) {
      if (collidableEntity == entity || !collidableEntity->isEnabled() || !collidableEntity->hasComponent<Collider>())
        continue;

      assert("Error: A collidable entity must have a Transform component." && collidableEntity->hasComponent<Transform>());

      auto& collider = collidableEntity->getComponent<Collider>();

      // The collision detection is made in the collider's local space
      // The test shapes/rays must thus be translated into that space
      const Vec3f colliderPos   = collidableEntity->getComponent<Transform>().getPosition();
      const Vec3f localStartPos = rigidBody.m_oldPosition - colliderPos;

      // We first try to determine if the last movement gave an intersection
      // This is necessary in case our object has travelled too fast right through the collider,
      //  ending behind it
      const Line movementLine(localStartPos, transform.getPosition() - colliderPos);
      if (!collider.intersects(movementLine))
        continue;

      const Ray ray(localStartPos, velocityDir);

      RayHit hit;
      if (!collider.intersects(ray, &hit))
        continue;

      // Setting the entity's new position a little above the collision point
      const Vec3f newPos = hit.position + hit.normal * 0.002f + colliderPos;

      rigidBody.m_oldPosition = newPos;
      transform.setPosition(newPos);

      //                                     Vt/paraVec
      //  Vel  N  Refl                  \---->
      //    \  ^  ^                     | \          Vn is the velocity's perpendicular component to the surface
      //     \ | /        ->            |   \        Vt is the velocity's parallel component to the surface
      // _____v|/______      Vn/perpVec v    v Vel

      const Vec3f paraVec = hit.normal * velocity.dot(hit.normal);
      const Vec3f perpVec = velocity - paraVec;

      rigidBody.setVelocity(perpVec - paraVec * rigidBody.getBounciness());

      break;
    }
  }
}

} // namespace Raz
