#include "Catch.hpp"

#include "RaZ/Component.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"

TEST_CASE("Components IDs", "[core]") {
  // With the CRTP, every component gets a different constant ID with the first call
  // The ID is incremented with every distinct component call
  // No matter how many times one component is checked, it will always have the same ID

  const std::size_t transIndex  = Raz::Component::getId<Raz::Transform>();
  const std::size_t cameraIndex = Raz::Component::getId<Raz::Camera>();
  const std::size_t meshIndex   = Raz::Component::getId<Raz::Mesh>();
  const std::size_t lightIndex  = Raz::Component::getId<Raz::Light>();

  // Every component already have an ID attributed to each of them, so calling them again won't change it
  CHECK(transIndex == Raz::Component::getId<Raz::Transform>());
  CHECK(cameraIndex == Raz::Component::getId<Raz::Camera>());
  CHECK(meshIndex == Raz::Component::getId<Raz::Mesh>());
  CHECK(lightIndex == Raz::Component::getId<Raz::Light>());
}
