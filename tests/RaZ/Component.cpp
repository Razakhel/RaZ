#include "catch/catch.hpp"
#include "RaZ/Component.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/Mesh.hpp"

TEST_CASE("Components IDs checks") {
  // With the CRTP, every component gets a different constant ID with the first call
  // The ID is incremented with every distinct component call
  // No matter how many times one component is checked, it will always have the same ID

  REQUIRE(Raz::Component::getId<Raz::Mesh>() == 0);      // Raz::Mesh's ID is fetched, gets 0; ID incremented
  REQUIRE(Raz::Component::getId<Raz::Transform>() == 1); // Raz::Transform's ID is fetched, gets 1; ID incremented
  REQUIRE(Raz::Component::getId<Raz::Light>() == 2);     // Raz::Light's ID is fetched, gets 2; ID incremented
  REQUIRE(Raz::Component::getId<Raz::Camera>() == 3);    // Raz::Camera's ID is fetched, gets 3; ID incremented

  REQUIRE(Raz::Component::getId<Raz::Transform>() == 1); // Raz::Transform already has the ID 1
  REQUIRE(Raz::Component::getId<Raz::Camera>() == 3);    // Raz::Camera already has the ID 3
  REQUIRE(Raz::Component::getId<Raz::Mesh>() == 0);      // Raz::Mesh already has the ID 0
  REQUIRE(Raz::Component::getId<Raz::Light>() == 2);     // Raz::Light already has the ID 2
}
