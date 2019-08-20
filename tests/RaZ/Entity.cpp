#include "catch/catch.hpp"
#include "RaZ/Entity.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/Mesh.hpp"

namespace {

// Declaring entities to be tested
Raz::Entity entity0(0);
Raz::Entity entity1(1);
Raz::Entity entity2(2);

} // namespace

TEST_CASE("Entity basic") {
  REQUIRE(entity0.getId() == 0);
  REQUIRE(entity1.getId() == 1);
  REQUIRE(entity2.getId() == 2);

  REQUIRE(entity0.isEnabled());

  entity0.disable();
  REQUIRE_FALSE(entity0.isEnabled());

  entity0.enable();
  REQUIRE(entity0.isEnabled());
}

TEST_CASE("Entity-component manipulations") {
  REQUIRE(entity0.getComponents().empty());

  REQUIRE_FALSE(entity0.hasComponent<Raz::Mesh>());
  REQUIRE_FALSE(entity0.hasComponent<Raz::Transform>());

  entity0.addComponent<Raz::Transform>();
  REQUIRE(entity0.hasComponent<Raz::Transform>());
  REQUIRE_FALSE(entity0.hasComponent<Raz::Mesh>());

  REQUIRE(std::is_same_v<decltype(entity0.getComponent<Raz::Transform>()), Raz::Transform&>);
  REQUIRE_THROWS(entity0.getComponent<Raz::Mesh>());

  entity0.removeComponent<Raz::Transform>();
  REQUIRE_FALSE(entity0.hasComponent<Raz::Transform>());
  REQUIRE_THROWS(entity0.getComponent<Raz::Transform>());
}

TEST_CASE("Entity bitset") {
  REQUIRE(entity1.getEnabledComponents().isEmpty());
  entity1.addComponent<Raz::Transform>();
  REQUIRE_FALSE(entity1.getEnabledComponents().isEmpty());

  REQUIRE(entity1.getEnabledComponents().getEnabledBitCount() == 1);
  REQUIRE(entity1.getEnabledComponents()[Raz::Component::getId<Raz::Transform>()]);
  REQUIRE(entity1.getEnabledComponents().getSize() == Raz::Component::getId<Raz::Transform>() + 1);

  REQUIRE(entity2.getEnabledComponents().isEmpty());
  entity2.addComponent<Raz::Light>(Raz::LightType::POINT, 10.f);
  REQUIRE_FALSE(entity2.getEnabledComponents().isEmpty());

  REQUIRE(entity2.getEnabledComponents().getEnabledBitCount() == 1);
  REQUIRE(entity2.getEnabledComponents()[Raz::Component::getId<Raz::Light>()]);
  REQUIRE(entity2.getEnabledComponents().getSize() == Raz::Component::getId<Raz::Light>() + 1);

  //  Match test
  //     ---
  //    0 0 1     -> Transform
  // &  0 1 0     -> Light
  //   _______
  // =  0 0 0
  REQUIRE((entity1.getEnabledComponents() & entity2.getEnabledComponents()) == Raz::Bitset(entity1.getEnabledComponents().getSize()));
}
