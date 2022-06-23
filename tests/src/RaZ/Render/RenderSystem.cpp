#include "Catch.hpp"
#include "TestUtils.hpp"

#include "RaZ/World.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace {

Raz::Image renderFrame(Raz::World& world, const Raz::FilePath& renderedImgPath = {}) {
  Raz::Window& window = TestUtils::getWindow();

  // Rendering a frame of the scene by updating the World's RenderSystem & running the Window
  world.update(0.f);
  window.run(0.f);

  // Recovering the rendered frame into an image
  Raz::Image renderedImg(window.getWidth(), window.getHeight(), Raz::ImageColorspace::RGB, Raz::ImageDataType::BYTE);
  Raz::Renderer::recoverFrame(window.getWidth(), window.getHeight(), Raz::TextureFormat::RGB, Raz::TextureDataType::UBYTE, renderedImg.getDataPtr());

  if (!renderedImgPath.isEmpty())
    Raz::ImageFormat::save(renderedImgPath, renderedImg, true);

  return renderedImg;
}

} // namespace

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

TEST_CASE("RenderSystem Cook-Torrance ball") {
  Raz::World world(7);

  Raz::Window& window = TestUtils::getWindow();

  world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 3.f)).addComponent<Raz::Camera>(window.getWidth(), window.getHeight());
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::MeshRenderer>(Raz::ObjFormat::load(RAZ_TESTS_ROOT "../assets/meshes/ball.obj").second);

  world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 1.5f)).addComponent<Raz::Light>(Raz::LightType::POINT, 1.5f, Raz::Vec3f(1.f));
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(5.f, 0.f, -1.f).normalize(),
                                                                          1.f, Raz::Vec3f(1.f, 1.f, 0.f));
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(-5.f, 0.f, -1.f).normalize(),
                                                                          1.f, Raz::Vec3f(1.f, 0.f, 1.f));
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, 5.f, -1.f).normalize(),
                                                                          1.f, Raz::Vec3f(0.f, 1.f, 1.f));
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, -5.f, -1.f).normalize(),
                                                                          1.f, Raz::Vec3f(1.f, 0.f, 0.f));

  CHECK_THAT(renderFrame(world), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_base.png", true)));
}
