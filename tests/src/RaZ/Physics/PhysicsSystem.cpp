#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/Collider.hpp"
#include "RaZ/Physics/PhysicsSystem.hpp"
#include "RaZ/Physics/RigidBody.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("PhysicsSystem basic", "[physics]") {
  Raz::PhysicsSystem physics;
  CHECK(physics.getGravity() == Raz::Vec3f(0.f, -9.80665f, 0.f));
  CHECK(physics.getFriction() == 0.95f);

  physics.setGravity(Raz::Vec3f(0.f));
  physics.setFriction(0.25f);
  CHECK(physics.getGravity() == Raz::Vec3f(0.f));
  CHECK(physics.getFriction() == 0.25f);
}

TEST_CASE("PhysicsSystem accepted components", "[physics]") {
  Raz::World world(2);

  const auto& physics = world.addSystem<Raz::PhysicsSystem>();

  const Raz::Entity& rigidBody = world.addEntityWithComponent<Raz::RigidBody>(1.f, 1.f);
  const Raz::Entity& collider  = world.addEntityWithComponent<Raz::Collider>(Raz::Plane(0.f));

  world.update({});

  CHECK(physics.containsEntity(rigidBody));
  CHECK(physics.containsEntity(collider));
}

TEST_CASE("PhysicsSystem rigid bodies collision", "[physics]") {
  Raz::World world(4);

  float remainingTime {};
  const auto updateWorld = [&world, &remainingTime] (float duration, float substepTime = 0.016666f) {
    world.update(Raz::FrameTimeInfo{ duration, 0.f, static_cast<int>((duration + remainingTime) / substepTime), substepTime });
    remainingTime = std::fmod(duration, substepTime);
  };

  world.addSystem<Raz::PhysicsSystem>();

  constexpr Raz::Vec3f initParticlePos(0.f, 1.f, 0.f);

  Raz::Entity& bouncyParticle   = world.addEntity();
  auto& bouncyParticleTransform = bouncyParticle.addComponent<Raz::Transform>(initParticlePos);
  auto& bouncyParticleRigidBody = bouncyParticle.addComponent<Raz::RigidBody>(1.f, 0.95f);

  Raz::Entity& solidParticle   = world.addEntity();
  auto& solidParticleTransform = solidParticle.addComponent<Raz::Transform>(initParticlePos);
  auto& solidParticleRigidBody = solidParticle.addComponent<Raz::RigidBody>(1.f, 0.05f); // The solid particle has a very low bounciness

  Raz::Entity& staticParticle   = world.addEntity();
  auto& staticParticleTransform = staticParticle.addComponent<Raz::Transform>(initParticlePos);
  auto& staticParticleRigidBody = staticParticle.addComponent<Raz::RigidBody>(0.f, 0.95f); // A mass <= 0 represents an infinite mass: the rigid body won't move

  // Adding a plane collider as the floor
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Collider>(Raz::Plane(0.f, Raz::Axis::Y));

  updateWorld(0.f);

  // The delta time being 0, no particle has moved
  CHECK(bouncyParticleTransform.getPosition().strictlyEquals(initParticlePos));
  CHECK(bouncyParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));
  CHECK(solidParticleTransform.getPosition().strictlyEquals(initParticlePos));
  CHECK(solidParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));
  CHECK(staticParticleTransform.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  // The system should internally update itself in several fixed time steps to guarantee its numerical stability
  updateWorld(0.02f);

  // The particles have started moving downards
  CHECK(bouncyParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.998638f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -0.163437635f, 0.f));
  CHECK(solidParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.998638f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -0.163437635f, 0.f));
  CHECK(staticParticleTransform.getPosition().strictlyEquals(initParticlePos)); // The static particle still remains unchanged
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  // Updating to right before the collision
  updateWorld(0.44f);

  CHECK(bouncyParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.0143348f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -4.3641448f, 0.f));
  CHECK(solidParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.0143348f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -4.3641448f, 0.f));
  CHECK(staticParticleTransform.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  // Collision with the floor happens on the following step
  updateWorld(0.01f);

  // Particles having collided with the floor, they now have an upward velocity
  CHECK(bouncyParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.002f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, 4.29766f, 0.f)); // Having a high bounciness, the velocity has been almost fully reverted
  CHECK(solidParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.002f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, 0.226192668f, 0.f)); // Having a low bounciness, the velocity has decreased dramatically
  CHECK(staticParticleTransform.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));

  updateWorld(0.03f);

  // After more steps, due to the velocity difference, the two particles' positions have been desynchronized
  CHECK(bouncyParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.139680699f, 0.f));
  CHECK(bouncyParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, 3.96358323f, 0.f));
  CHECK(solidParticleTransform.getPosition() == Raz::Vec3f(0.f, 0.00408647349f, 0.f));
  CHECK(solidParticleRigidBody.getVelocity() == Raz::Vec3f(0.f, -0.100929342f, 0.f)); // The solid particle goes back downards almost instantly
  CHECK(staticParticleTransform.getPosition().strictlyEquals(initParticlePos));
  CHECK(staticParticleRigidBody.getVelocity().strictlyEquals(Raz::Vec3f(0.f)));
}
