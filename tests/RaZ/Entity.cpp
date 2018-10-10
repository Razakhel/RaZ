#include "catch/catch.hpp"
#include "RaZ/Entity.hpp"

namespace {

// Declaring entities to be tested
Raz::Entity entity0(0);
Raz::Entity entity1(1);
Raz::Entity entity2(2);

} // namespace

TEST_CASE("Entity base tests") {
  REQUIRE(entity0.getId() == 0);
  REQUIRE(entity1.getId() == 1);
  REQUIRE(entity2.getId() == 2);

  REQUIRE(entity0.isEnabled());

  entity0.disable();
  REQUIRE_FALSE(entity0.isEnabled());

  entity0.enable();
  REQUIRE(entity0.isEnabled());
}
