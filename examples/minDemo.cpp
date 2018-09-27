#include "RaZ/RaZ.hpp"

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(Raz::World(2));

  auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");
  render.setProgram(Raz::ShaderProgram(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                       Raz::FragmentShader::create("../../shaders/cook-torrance.glsl")));

  render.getCameraEntity().getComponent<Raz::Transform>().setPosition(Raz::Vec3f({0.f, 0.f, -5.f}));

  auto& mesh = world.addEntityWithComponent<Raz::Transform>();
  mesh.addComponent<Raz::Mesh>("../../assets/meshes/ball.obj");

  auto& light = world.addEntityWithComponent<Raz::Transform>();
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL,   // Type
                                 Raz::Vec3f({ 0.f, 0.f, 1.f }), // Direction
                                 1.f,                           // Energy
                                 Raz::Vec3f(1.f));              // Color (R/G/B)

  while (app.run());

  return EXIT_SUCCESS;
}
