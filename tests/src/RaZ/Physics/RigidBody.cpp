#include "Catch.hpp"

#include "RaZ/Physics/RigidBody.hpp"

TEST_CASE("RigidBody basic") {
  Raz::RigidBody rigidBody(1.1f, 0.25f);
  CHECK(rigidBody.getMass() == 1.1f);
  CHECK(rigidBody.getInvMass() == 0.909090877f);
  CHECK(rigidBody.getBounciness() == 0.25f);
  CHECK(rigidBody.getForces() == Raz::Vec3f(0.f)); // No forces for now; gravity is computed when calculating the rigid body's acceleration
  CHECK(rigidBody.getVelocity() == Raz::Vec3f(0.f));

  rigidBody.setMass(10.1f);
  CHECK(rigidBody.getMass() == 10.1f);
  CHECK(rigidBody.getInvMass() == 0.099009894f);

  rigidBody.setMass(-1.f);
  CHECK(rigidBody.getMass() == -1.f);
  CHECK(rigidBody.getInvMass() == 0.f); // A mass <= 0 is an infinite mass; the inverse is set to 0

  rigidBody.setMass(0.f);
  CHECK(rigidBody.getMass() == 0.f);
  CHECK(rigidBody.getInvMass() == 0.f); // Same as above

  rigidBody.setBounciness(0.75f);
  CHECK(rigidBody.getBounciness() == 0.75f);

  rigidBody.setForces(Raz::Vec3f(0.f, 1.f, 2.f), Raz::Vec3f(1.f, 2.f, 3.f));
  CHECK(rigidBody.getForces() == Raz::Vec3f(1.f, 3.f, 5.f)); // All given forces are added together

  rigidBody.setVelocity(Raz::Vec3f(0.f, 1.12f, -3.f));
  CHECK(rigidBody.getVelocity() == Raz::Vec3f(0.f, 1.12f, -3.f));
}
