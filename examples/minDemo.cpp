#include "RaZ/RaZ.hpp"

int main() {
  Raz::WindowPtr window = std::make_unique<Raz::Window>(800, 600);

  const Raz::Application app(std::move(window),
                             std::make_unique<Raz::Scene>(std::make_unique<Raz::VertexShader>("../../shaders/vert.glsl"),
                                                          std::make_unique<Raz::FragmentShader>("../../shaders/cook-torrance.glsl")));

  app.getScene()->addModel(Raz::ModelLoader::importModel("../../assets/meshes/ball.obj"));
  app.getScene()->addLight(std::make_unique<Raz::DirectionalLight>(Raz::Vec3f({ 0.f, 0.f, 1.f })));
  app.getScene()->load();

  app.getCamera()->translate(0.f, 0.f, -5.f);

  while (app.run());

  return EXIT_SUCCESS;
}
