#include "RaZ/RaZ.hpp"

int main() {
  Raz::WindowPtr window = Raz::Window::create(800, 600);

  const Raz::Application app(std::move(window),
                             Raz::Scene::create(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                                Raz::FragmentShader::create("../../shaders/cook-torrance.glsl")));

  app.getScene()->addModel(Raz::Model::import("../../assets/meshes/ball.obj"));
  app.getScene()->addLight(Raz::DirectionalLight::create(Raz::Vec3f({ 0.f, 0.f, 1.f })));
  app.getScene()->load();

  app.getCamera()->translate(0.f, 0.f, -5.f);

  while (app.run());

  return EXIT_SUCCESS;
}
