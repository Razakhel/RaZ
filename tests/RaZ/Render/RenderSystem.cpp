#include "Catch.hpp"

#include "RaZ/World.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"

TEST_CASE("RenderSystem accepted components") {
  Raz::World world(3);

  auto& render = world.addSystem<Raz::RenderSystem>(0, 0);

  Raz::Entity& camera       = world.addEntityWithComponents<Raz::Camera, Raz::Transform>(); // RenderSystem::update() needs a Camera with a Transform component
  Raz::Entity& meshRenderer = world.addEntityWithComponent<Raz::MeshRenderer>();
  Raz::Entity& light        = world.addEntityWithComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::X, 1.f);

  world.update(0.f);

  CHECK(render.containsEntity(camera));
  CHECK(render.containsEntity(meshRenderer));
  CHECK(render.containsEntity(light));
}
