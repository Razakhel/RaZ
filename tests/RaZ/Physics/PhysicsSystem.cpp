#include "Catch.hpp"

#include "RaZ/World.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/Collider.hpp"
#include "RaZ/Physics/PhysicsSystem.hpp"
#include "RaZ/Physics/RigidBody.hpp"

constexpr float fixedTimeStep = 0.016666f;

TEST_CASE("PhysicsSystem basic") {
  Raz::PhysicsSystem physics;
  CHECK(physics.getGravity() == Raz::Vec3f(0.f, -9.80665f, 0.f));
  CHECK(physics.getFriction() == 0.95f);

  physics.setGravity(Raz::Vec3f(0.f));
  physics.setFriction(0.25f);
  CHECK(physics.getGravity() == Raz::Vec3f(0.f));
  CHECK(physics.getFriction() == 0.25f);
}

TEST_CASE("PhysicsSystem rigid bodies collision") {
  Raz::World world(4);

  auto& physics = world.addSystem<Raz::PhysicsSystem>();

  constexpr Raz::Vec3f initParticlePos(0.f, 1.f, 0.f);

  Raz::Entity& bouncyParticle   = world.addEntity();
  auto& bouncyParticleTrans     = bouncyParticle.addComponent<Raz::Transform>(initParticlePos);
  auto& bouncyParticleRigidBody = bouncyParticle.addComponent<Raz::RigidBody>(1.f, 0.95f);

  Raz::Entity& solidParticle   = world.addEntity();
  auto& solidParticleTrans     = solidParticle.addComponent<Raz::Transform>(initParticlePos);
  auto& solidParticleRigidBody = solidParticle.addComponent<Raz::RigidBody>(1.f, 0.05f); // The solid particle has a very low bounciness

  Raz::Entity& staticParticle   = world.addEntity();
  auto& staticParticleTrans     = staticParticle.addComponent<Raz::Transform>(initParticlePos);
  auto& staticParticleRigidBody = staticParticle.addComponent<Raz::RigidBody>(0.f, 0.95f); // A mass <= 0 represents an infinite mass: the rigid body won't move

  Raz::Entity& floor  = world.addEntity();
  floor.addComponent<Raz::Transform>();
  floor.addComponent<Raz::Collider>(Raz::Plane(0.f, Raz::Axis::Y));

  world.update(0.f);

  REQUIRE(physics.containsEntity(bouncyParticle));
  REQUIRE(physics.containsEntity(solidParticle));
  REQUIRE(physics.containsEntity(staticParticle));
  REQUIRE(physics.containsEntity(floor));

  // The delta time being 0, no particle has moved
  CHECK(bouncyParticleTrans.getPosition().strictlyEquals(initParticlePos));
  CHECK(bouncyParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));
  CHECK(solidParticleTrans.getPosition().strictlyEquals(initParticlePos));
  CHECK(solidParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));
  CHECK(staticParticleTrans.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  // The physics system must be updated with a fixed time step to guarantee numerical stability
  world.update(fixedTimeStep);

  // The particles have started moving downards
  CHECK(bouncyParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.998638f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -0.163437635f, 0.f));
  CHECK(solidParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.998638f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -0.163437635f, 0.f));
  CHECK(staticParticleTrans.getPosition().strictlyEquals(initParticlePos)); // The static particle still remains unchanged
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  // Updating to right before the collision
  for (int i = 0; i < 26; ++i)
    world.update(fixedTimeStep);

  CHECK(bouncyParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.0143348f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -4.3641448f, 0.f));
  CHECK(solidParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.0143348f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -4.3641448f, 0.f));
  CHECK(staticParticleTrans.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  // The collision with the floor happens on this step
  world.update(fixedTimeStep);

  // Particles having collided with the floor, they now have an upward velocity
  CHECK(bouncyParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.002f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, 4.29766f, 0.f)); // Having a high bounciness, the velocity has been almost fully reverted
  CHECK(solidParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.002f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, 0.226192668f, 0.f)); // Having a low bounciness, the velocity has decreased dramatically
  CHECK(staticParticleTrans.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  world.update(fixedTimeStep);
  world.update(fixedTimeStep);

  // After more steps, due to the velocity difference, the two particles' positions have been desynchronized
  CHECK(bouncyParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.139680699f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, 3.96358323f, 0.f));
  CHECK(solidParticleTrans.getPosition() == Raz::Vec3f(0.f, 0.00408647349f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -0.100929342f, 0.f)); // The solid particle goes back downards almost instantly
  CHECK(staticParticleTrans.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));
}
