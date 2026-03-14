#include "RaZ/Utils/TriggerVolume.hpp"

#include <catch2/catch_test_macros.hpp>

struct TestComponent1 : Raz::Component {};
struct TestComponent2 : Raz::Component {};

TEST_CASE("Triggerer triggerable components", "[utils]") {
  Raz::Triggerer triggerer;
  CHECK(triggerer.getTriggerableComponents().isEmpty());

  triggerer.registerComponents<Raz::Triggerer>(); // Probably a degenerate case, may be considered invalid later
  CHECK(triggerer.getTriggerableComponents().getEnabledBitCount() == 1);
  REQUIRE(triggerer.getTriggerableComponents().getSize() > Raz::Component::getId<Raz::Triggerer>());
  CHECK(triggerer.getTriggerableComponents()[Raz::Component::getId<Raz::Triggerer>()]);

  triggerer.registerComponents<TestComponent1, TestComponent2>();
  CHECK(triggerer.getTriggerableComponents().getEnabledBitCount() == 3);
  REQUIRE(triggerer.getTriggerableComponents().getSize() > std::max(Raz::Component::getId<TestComponent1>(), Raz::Component::getId<TestComponent2>()));
  CHECK(triggerer.getTriggerableComponents()[Raz::Component::getId<TestComponent1>()]);
  CHECK(triggerer.getTriggerableComponents()[Raz::Component::getId<TestComponent2>()]);

  triggerer.unregisterComponents<TestComponent1, TestComponent2>();
  CHECK(triggerer.getTriggerableComponents().getEnabledBitCount() == 1);
  REQUIRE(triggerer.getTriggerableComponents().getSize() > std::max(Raz::Component::getId<TestComponent1>(), Raz::Component::getId<TestComponent2>()));
  CHECK_FALSE(triggerer.getTriggerableComponents()[Raz::Component::getId<TestComponent1>()]);
  CHECK_FALSE(triggerer.getTriggerableComponents()[Raz::Component::getId<TestComponent2>()]);

  triggerer.unregisterComponents<Raz::Triggerer>();
  CHECK(triggerer.getTriggerableComponents().isEmpty());
}
