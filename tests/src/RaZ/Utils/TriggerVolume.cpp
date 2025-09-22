#include "RaZ/Utils/TriggerVolume.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("TriggerVolume basic", "[utils]") {
  CHECK_THROWS(Raz::TriggerVolume(Raz::AABB(Raz::Vec3f(1.f), Raz::Vec3f(-1.f)))); // Max > min
  CHECK_THROWS(Raz::TriggerVolume(Raz::AABB(Raz::Vec3f(1.f), Raz::Vec3f(1.f)))); // Max == min
  CHECK_NOTHROW(Raz::TriggerVolume(Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f))));

  CHECK_THROWS(Raz::TriggerVolume(Raz::OBB(Raz::Vec3f(1.f), Raz::Vec3f(-1.f), Raz::Quaternionf::identity()))); // Max > min
  CHECK_THROWS(Raz::TriggerVolume(Raz::OBB(Raz::Vec3f(1.f), Raz::Vec3f(1.f), Raz::Quaternionf::identity()))); // Max == min
  CHECK_NOTHROW(Raz::TriggerVolume(Raz::OBB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f), Raz::Quaternionf::identity())));

  CHECK_THROWS(Raz::TriggerVolume(Raz::Sphere({}, -1.f))); // Negative radius
  CHECK_THROWS(Raz::TriggerVolume(Raz::Sphere({}, 0.f))); // Null radius
  CHECK_NOTHROW(Raz::TriggerVolume(Raz::Sphere({}, 1.f)));
}
