#include "TestUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaPhysics Collider", "[script][lua][physics]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local collider = Collider.new()

    assert(not collider:hasShape())
    collider:setShape(Sphere.new(Vec3f.new(), 1))
    assert(collider:hasShape())
    assert(collider:getShapeType() == collider:getShape():getType())
    assert(collider:getShape():computeBoundingBox() == AABB.new(Vec3f.new(-1), Vec3f.new(1)))

    assert(collider:intersects(collider))
    assert(collider:intersects(collider:getShape():computeBoundingBox()))
    local rayHit = RayHit.new()
    assert(collider:intersects(Ray.new(Vec3f.new(), Axis.Z)))
    assert(collider:intersects(Ray.new(Vec3f.new(), Axis.Z), rayHit))
  )"));
}

TEST_CASE("LuaPhysics PhysicsSystem", "[script][lua][physics]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local physicsSystem = PhysicsSystem.new()

    physicsSystem.gravity  = Axis.Y
    physicsSystem.friction = 0.25
    assert(physicsSystem.gravity == Axis.Y)
    assert(physicsSystem.friction == 0.25)

    assert(physicsSystem:getAcceptedComponents() ~= nil)
    assert(not physicsSystem:containsEntity(Entity.new(0)))
    assert(physicsSystem:update(FrameTimeInfo.new()))
    physicsSystem:destroy()
  )"));
}

TEST_CASE("LuaPhysics RigidBody", "[script][lua][physics]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local rigidBody = RigidBody.new(0, 0)

    rigidBody.mass       = 10
    rigidBody.bounciness = 0.25
    rigidBody.velocity   = Vec3f.new(1, 2, 3)
    rigidBody.forces     = Vec3f.new(1) + Vec3f.new(4, 5, 6)
    assert(rigidBody.mass == 10)
    assert(FloatUtils.areNearlyEqual(rigidBody:getInvMass(), 0.1))
    assert(rigidBody.bounciness == 0.25)
    assert(rigidBody.velocity == Vec3f.new(1, 2, 3))
    assert(rigidBody.forces == Vec3f.new(5, 6, 7))
  )"));
}
