#include "RaZ/Entity.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

struct FirstTestComponent : public Raz::Component {};
struct SecondTestComponent : public Raz::Component {};

} // namespace

TEST_CASE("Entity basic", "[core]") {
  Raz::Entity entity0(0);
  const Raz::Entity entity1(1);
  const Raz::Entity entity2(2, false);

  CHECK(entity0.getId() == 0);
  CHECK(entity1.getId() == 1);
  CHECK(entity2.getId() == 2);

  CHECK(entity0.isEnabled());
  CHECK(entity1.isEnabled());
  CHECK_FALSE(entity2.isEnabled());

  entity0.disable();
  CHECK_FALSE(entity0.isEnabled());

  entity0.enable();
  CHECK(entity0.isEnabled());
}

TEST_CASE("Entity-component manipulation", "[core]") {
  Raz::Entity entity(0);

  CHECK(entity.getComponents().empty());
  CHECK_FALSE(entity.hasComponent<FirstTestComponent>());
  CHECK_FALSE(entity.hasComponent<SecondTestComponent>());

  entity.addComponent<FirstTestComponent>();
  CHECK(entity.hasComponent<FirstTestComponent>());
  CHECK_NOTHROW(entity.getComponent<FirstTestComponent>());
  CHECK_FALSE(entity.hasComponent<SecondTestComponent>());
  CHECK_THROWS(entity.getComponent<SecondTestComponent>());

  CHECK(std::is_same_v<decltype(entity.getComponent<FirstTestComponent>()), FirstTestComponent&>);

  entity.removeComponent<FirstTestComponent>();
  CHECK_FALSE(entity.hasComponent<FirstTestComponent>());
  CHECK_THROWS(entity.getComponent<FirstTestComponent>());
}

TEST_CASE("Entity bitset", "[core]") {
  Raz::Entity entity0(0);
  Raz::Entity entity1(1);

  CHECK(entity0.getEnabledComponents().isEmpty());
  entity0.addComponent<FirstTestComponent>();
  CHECK_FALSE(entity0.getEnabledComponents().isEmpty());

  CHECK(entity0.getEnabledComponents().getEnabledBitCount() == 1);
  CHECK(entity0.getEnabledComponents()[Raz::Component::getId<FirstTestComponent>()]);
  CHECK(entity0.getEnabledComponents().getSize() == Raz::Component::getId<FirstTestComponent>() + 1);

  CHECK(entity1.getEnabledComponents().isEmpty());
  entity1.addComponent<SecondTestComponent>();
  CHECK_FALSE(entity1.getEnabledComponents().isEmpty());

  CHECK(entity1.getEnabledComponents().getEnabledBitCount() == 1);
  CHECK(entity1.getEnabledComponents()[Raz::Component::getId<SecondTestComponent>()]);
  CHECK(entity1.getEnabledComponents().getSize() == Raz::Component::getId<SecondTestComponent>() + 1);

  //  Match test
  //     ---
  //    0 0 1     -> FirstTestComponent
  // &  0 1 0     -> SecondTestComponent
  //   _______
  // =  0 0 0
  CHECK((entity0.getEnabledComponents() & entity1.getEnabledComponents()) == Raz::Bitset(entity1.getEnabledComponents().getSize(), false));
}
