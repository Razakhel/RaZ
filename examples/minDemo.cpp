#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(2);

  auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/vert.glsl"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.glsl"s));

  Raz::Entity& camera = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));
  camera.addComponent<Raz::Camera>(render.getWindow().getWidth(), render.getWindow().getHeight());

  Raz::Entity& mesh = world.addEntityWithComponent<Raz::Transform>();
  mesh.addComponent<Raz::Mesh>(RAZ_ROOT + "assets/meshes/ball.obj"s);

  Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                 Raz::Vec3f(0.f, 0.f, 1.f),   // Direction
                                 1.f,                         // Energy
                                 Raz::Vec3f(1.f));            // Color (R/G/B)

  render.getWindow().addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });

  app.run();

  return EXIT_SUCCESS;
}
