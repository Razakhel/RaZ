#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Utils/TriggerSystem.hpp"
#include "RaZ/Utils/TriggerVolume.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("TriggerSystem accepted components", "[utils]") {
  Raz::World world;

  const auto& triggerSystem = world.addSystem<Raz::TriggerSystem>();

  const Raz::Entity& triggerer     = world.addEntityWithComponent<Raz::Triggerer>();
  const Raz::Entity& triggerVolume = world.addEntityWithComponent<Raz::TriggerVolume>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

  world.update({});

  CHECK(triggerSystem.containsEntity(triggerer));
  CHECK(triggerSystem.containsEntity(triggerVolume));
}

TEST_CASE("TriggerSystem trigger actions", "[utils]") {
  Raz::World world;

  world.addSystem<Raz::TriggerSystem>();

  Raz::Entity& triggererEntity = world.addEntityWithComponent<Raz::Transform>();
  triggererEntity.addComponent<Raz::Triggerer>();

  auto& triggerBox    = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::TriggerVolume>(Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)));
  auto& triggerSphere = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::TriggerVolume>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

  int boxEnterCount = 0;
  int boxStayCount  = 0;
  int boxLeaveCount = 0;

  triggerBox.setEnterAction([&boxEnterCount] () noexcept { ++boxEnterCount; });
  triggerBox.setStayAction([&boxStayCount] () noexcept { ++boxStayCount; });
  triggerBox.setLeaveAction([&boxLeaveCount] () noexcept { ++boxLeaveCount; });

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

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 0);
  CHECK(sphereLeaveCount == 0);

  world.update({});
  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 2);
  CHECK(boxLeaveCount == 0);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 0);

  // Moving the triggerer out of the sphere, but still inside the box
  triggererEntity.getComponent<Raz::Transform>().setPosition(Raz::Vec3f(0.75f));

  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 3);
  CHECK(boxLeaveCount == 0);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 1);

  // Moving the triggerer out of both volumes
  triggererEntity.getComponent<Raz::Transform>().setPosition(Raz::Vec3f(1.5f));

  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 3);
  CHECK(boxLeaveCount == 1);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 1);

  // Moving the triggerer inside both volumes and resetting all actions
  triggererEntity.getComponent<Raz::Transform>().setPosition(Raz::Vec3f(0.f));
  triggerBox.resetEnterAction();
  triggerBox.resetStayAction();
  triggerBox.resetLeaveAction();
  triggerSphere.resetEnterAction();
  triggerSphere.resetStayAction();
  triggerSphere.resetLeaveAction();

  // Even though both volumes are triggered, nothing is done as no action is set
  world.update({});

  CHECK(boxEnterCount == 1);
  CHECK(boxStayCount == 3);
  CHECK(boxLeaveCount == 1);

  CHECK(sphereEnterCount == 1);
  CHECK(sphereStayCount == 2);
  CHECK(sphereLeaveCount == 1);
}
