#include "RaZ/RaZ.hpp"

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(Raz::World(2));

  auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");
  render.setGeometryProgram(Raz::ShaderProgram(Raz::VertexShader("../../shaders/vert.glsl"),
                                               Raz::FragmentShader("../../shaders/cook-torrance.glsl")));

  render.getCameraEntity().getComponent<Raz::Transform>().setPosition(Raz::Vec3f({ 0.f, 0.f, -5.f }));

  Raz::Entity& mesh = world.addEntityWithComponent<Raz::Transform>();
  mesh.addComponent<Raz::Mesh>("../../assets/meshes/ball.obj");

  Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL,   // Type
                                 Raz::Vec3f({ 0.f, 0.f, 1.f }), // Direction
                                 1.f,                           // Energy
                                 Raz::Vec3f(1.f));              // Color (R/G/B)

  render.getWindow().addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });

  app.run();

  return EXIT_SUCCESS;
}
