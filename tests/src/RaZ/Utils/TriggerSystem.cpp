#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Utils/TriggerSystem.hpp"
#include "RaZ/Utils/TriggerVolume.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Raz::Literals;

TEST_CASE("TriggerSystem accepted components", "[utils]") {
  Raz::World world;

  const auto& triggerSystem = world.addSystem<Raz::TriggerSystem>();

  const Raz::Entity& triggerer     = world.addEntityWithComponent<Raz::Triggerer>();
  const Raz::Entity& triggerVolume = world.addEntityWithComponent<Raz::TriggerVolume>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

  world.update({});

  CHECK(triggerSystem.containsEntity(triggerer));
  CHECK(triggerSystem.containsEntity(triggerVolume));
}

TEST_CASE("TriggerSystem triggerer components", "[utils]") {
  Raz::World world;

  world.addSystem<Raz::TriggerSystem>();

  auto& triggerVolumeEntity = world.addEntityWithComponent<Raz::Transform>();
  auto& triggerVolume       = triggerVolumeEntity.addComponent<Raz::TriggerVolume>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

  int triggerCount = 0;
  triggerVolume.setEnterAction([&triggerCount] () noexcept { ++triggerCount; });
  triggerVolume.setStayAction([&triggerCount] () noexcept { ++triggerCount; });

  auto& triggerer = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Triggerer>();

  struct TestComponent : Raz::Component {};

  // A triggerer with no triggerable component can trigger nothing
  world.update({});
  CHECK(triggerCount == 0);

  // Adding a triggerable component to the triggerer, which the trigger volume entity doesn't have yet
  triggerer.registerComponents<TestComponent>();
  world.update({});
  CHECK(triggerCount == 0);

  // Adding the component to the trigger volume makes it triggerable
  triggerVolumeEntity.addComponent<TestComponent>();
  world.update({});
  world.update({});
  CHECK(triggerCount == 2);

  // Removing the component makes it untriggerable again
  triggerVolumeEntity.removeComponent<TestComponent>();
  world.update({});
  CHECK(triggerCount == 2);
}

TEST_CASE("TriggerSystem trigger actions", "[utils]") {
  Raz::World world;

  world.addSystem<Raz::TriggerSystem>();

  // Transforms and volumes are defined so that all objects are tested at the same location:
  //  - The AABB has its centroid at [0; 0; 0] and its transform to the destination;
  //  - The OBB has both its centroid and its transform halfway;
  //  - The sphere has its centroid at the destination and an identity transform.

  constexpr Raz::Vec3f testLocation(0.f, 0.f, -5.f);

  constexpr Raz::Transform triggererInitTransform(testLocation);
  const Raz::Transform boxInitTransform(Raz::Axis::Right * 5.f, Raz::Quaternionf(90_deg, Raz::Axis::Y));
  const Raz::Transform orientedBoxInitTransform(Raz::Axis::Up * 2.5f, Raz::Quaternionf(-90_deg, Raz::Axis::X));
  constexpr Raz::Transform sphereInitTransform;

  constexpr Raz::AABB aabb(Raz::Vec3f(-0.1f), Raz::Vec3f(0.1f));
  const Raz::OBB obb(Raz::Vec3f(-0.1f, -0.1f, -2.51f), Raz::Vec3f(0.1f, 0.1f, -2.49f), Raz::Quaternionf(45_deg, Raz::Axis::Y));
  constexpr Raz::Sphere sphere(testLocation, 0.1f);

  CHECK(boxInitTransform.getRotation() * boxInitTransform.getPosition() + aabb.computeCentroid() == testLocation);
  CHECK(orientedBoxInitTransform.getRotation() * orientedBoxInitTransform.getPosition() + obb.computeCentroid() == testLocation);
  CHECK(sphereInitTransform.getRotation() * sphereInitTransform.getPosition() + sphere.computeCentroid() == testLocation);

  Raz::Entity& triggererEntity = world.addEntityWithComponent<Raz::Transform>(triggererInitTransform);
  triggererEntity.addComponent<Raz::Triggerer>().registerComponents<Raz::TriggerVolume>(); // Standard case, triggers any TriggerVolume

  auto& triggerBox         = world.addEntityWithComponent<Raz::Transform>(boxInitTransform).addComponent<Raz::TriggerVolume>(aabb);
  auto& triggerOrientedBox = world.addEntityWithComponent<Raz::Transform>(orientedBoxInitTransform).addComponent<Raz::TriggerVolume>(obb);
  auto& triggerSphere      = world.addEntityWithComponent<Raz::Transform>(sphereInitTransform).addComponent<Raz::TriggerVolume>(sphere);

  int boxEnterCount = 0;
  int boxStayCount  = 0;
  int boxLeaveCount = 0;

  triggerBox.setEnterAction([&boxEnterCount] () noexcept { ++boxEnterCount; });
  triggerBox.setStayAction([&boxStayCount] () noexcept { ++boxStayCount; });
  triggerBox.setLeaveAction([&boxLeaveCount] () noexcept { ++boxLeaveCount; });

  int orientedBoxEnterCount = 0;
  int orientedBoxStayCount  = 0;
  int orientedBoxLeaveCount = 0;

  triggerOrientedBox.setEnterAction([&orientedBoxEnterCount] () noexcept { ++orientedBoxEnterCount; });
  triggerOrientedBox.setStayAction([&orientedBoxStayCount] () noexcept { ++orientedBoxStayCount; });
  triggerOrientedBox.setLeaveAction([&orientedBoxLeaveCount] () noexcept { ++orientedBoxLeaveCount; });

  int sphereEnterCount = 0;
  int sphereStayCount  = 0;
  int sphereLeaveCount = 0;

  triggerSphere.setEnterAction([&sphereEnterCount] () noexcept { ++sphereEnterCount; });
  triggerSphere.setStayAction([&sphereStayCount] () noexcept { ++sphereStayCount; });
  triggerSphere.setLeaveAction([&sphereLeaveCount] () noexcept { ++sphereLeaveCount; });

  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 0);
  CHECK(boxLeaveCount == 0);

  CHECK(orientedBoxEnterCount == 1);
  CHECK(orientedBoxStayCount == 0);
  CHECK(orientedBoxLeaveCount == 0);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 0);
  CHECK(sphereLeaveCount == 0);

  world.update({});
  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 2);
  CHECK(boxLeaveCount == 0);

  CHECK(orientedBoxEnterCount == 1);
  CHECK(orientedBoxStayCount == 2);
  CHECK(orientedBoxLeaveCount == 0);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 0);

  // Moving the triggerer out of the sphere and the oriented box, but still inside the axis-aligned box
  triggererEntity.getComponent<Raz::Transform>().setPosition(testLocation + Raz::Vec3f(0.075f));

  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 3);
  CHECK(boxLeaveCount == 0);

  CHECK(orientedBoxEnterCount == 1);
  CHECK(orientedBoxStayCount == 2);
  CHECK(orientedBoxLeaveCount == 1);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 1);

  // Moving the triggerer out of all volumes
  triggererEntity.getComponent<Raz::Transform>().setPosition(testLocation + Raz::Vec3f(0.15f));

  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 3);
  CHECK(boxLeaveCount == 1);

  CHECK(orientedBoxEnterCount == 1);
  CHECK(orientedBoxStayCount == 2);
  CHECK(orientedBoxLeaveCount == 1);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 1);

  // Moving the triggerer back inside all volumes and resetting all actions
  triggererEntity.getComponent<Raz::Transform>().setPosition(testLocation);
  triggerBox.resetEnterAction();
  triggerBox.resetStayAction();
  triggerBox.resetLeaveAction();
  triggerOrientedBox.resetEnterAction();
  triggerOrientedBox.resetStayAction();
  triggerOrientedBox.resetLeaveAction();
  triggerSphere.resetEnterAction();
  triggerSphere.resetStayAction();
  triggerSphere.resetLeaveAction();

  // Even though all volumes are triggered, nothing is done as no action is set
  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 3);
  CHECK(boxLeaveCount == 1);

  CHECK(orientedBoxEnterCount == 1);
  CHECK(orientedBoxStayCount == 2);
  CHECK(orientedBoxLeaveCount == 1);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 1);
}
