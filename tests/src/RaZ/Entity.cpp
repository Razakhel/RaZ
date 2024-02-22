#include "Catch.hpp"

#include "RaZ/Entity.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Light.hpp"

namespace {

// Declaring entities to be tested
Raz::Entity entity0(0);
Raz::Entity entity1(1);
Raz::Entity entity2(2);

} // namespace

TEST_CASE("Entity basic", "[core]") {
  CHECK(entity0.getId() == 0);
  CHECK(entity1.getId() == 1);
  CHECK(entity2.getId() == 2);

  CHECK(entity0.isEnabled());

  entity0.disable();
  CHECK_FALSE(entity0.isEnabled());

  entity0.enable();
  CHECK(entity0.isEnabled());
}

TEST_CASE("Entity-component manipulations", "[core]") {
  CHECK(entity0.getComponents().empty());

  CHECK_FALSE(entity0.hasComponent<Raz::Mesh>());
  CHECK_FALSE(entity0.hasComponent<Raz::Transform>());

  entity0.addComponent<Raz::Transform>();
  CHECK(entity0.hasComponent<Raz::Transform>());
  CHECK_FALSE(entity0.hasComponent<Raz::Mesh>());

  CHECK(std::is_same_v<decltype(entity0.getComponent<Raz::Transform>()), Raz::Transform&>);
  CHECK_THROWS(entity0.getComponent<Raz::Mesh>());

  entity0.removeComponent<Raz::Transform>();
  CHECK_FALSE(entity0.hasComponent<Raz::Transform>());
  CHECK_THROWS(entity0.getComponent<Raz::Transform>());
}

TEST_CASE("Entity bitset", "[core]") {
  CHECK(entity1.getEnabledComponents().isEmpty());
  entity1.addComponent<Raz::Transform>();
  CHECK_FALSE(entity1.getEnabledComponents().isEmpty());

  CHECK(entity1.getEnabledComponents().getEnabledBitCount() == 1);
  CHECK(entity1.getEnabledComponents()[Raz::Component::getId<Raz::Transform>()]);
  CHECK(entity1.getEnabledComponents().getSize() == Raz::Component::getId<Raz::Transform>() + 1);

  CHECK(entity2.getEnabledComponents().isEmpty());
  entity2.addComponent<Raz::Light>(Raz::LightType::POINT, 10.f);
  CHECK_FALSE(entity2.getEnabledComponents().isEmpty());

  CHECK(entity2.getEnabledComponents().getEnabledBitCount() == 1);
  CHECK(entity2.getEnabledComponents()[Raz::Component::getId<Raz::Light>()]);
  CHECK(entity2.getEnabledComponents().getSize() == Raz::Component::getId<Raz::Light>() + 1);

  //  Match test
  //     ---
  //    0 0 1     -> Transform
  // &  0 1 0     -> Light
  //   _______
  // =  0 0 0
  CHECK((entity1.getEnabledComponents() & entity2.getEnabledComponents()) == Raz::Bitset(entity1.getEnabledComponents().getSize()));
}
