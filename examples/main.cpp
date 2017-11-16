#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::Window window(800, 600, "Test");

  Raz::Scene scene;

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/frag.glsl");

  Raz::Material defaultMaterial(vertShader, fragShader);
  const Raz::TexturePtr defaultTexture = std::make_shared<Raz::Texture>("../assets/textures/lena.png");
  defaultMaterial.setTexture(defaultTexture);

  const auto startTime = std::chrono::system_clock::now();

  const Raz::MeshPtr mesh = std::make_shared<Raz::Mesh>("../assets/meshes/bigguy.obj");

  const auto endTime = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  Raz::ModelPtr model = std::make_unique<Raz::Model>(mesh);
  model->setMaterial(defaultMaterial);

  const auto modelPtr = model.get();
  modelPtr->scale(0.05f);

  window.addKeyCallback(Raz::Keyboard::W, [&modelPtr] () { modelPtr->translate(0.f, 0.1f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::S, [&modelPtr] () { modelPtr->translate(0.f, -0.1f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::A, [&modelPtr] () { modelPtr->translate(-0.1f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::D, [&modelPtr] () { modelPtr->translate(0.1f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] () { modelPtr->rotate(15.f, 0.f, 1.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::LEFT, [&modelPtr] () { modelPtr->rotate(-15.f, 0.f, 1.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::UP, [&modelPtr] () { modelPtr->rotate(15.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::DOWN, [&modelPtr] () { modelPtr->rotate(-15.f, 1.f, 0.f, 0.f); });

  scene.addModel(std::move(model));

  scene.addLight(Raz::DirectionalLight(Raz::Vec4f({ 0.f, 1.f, 0.f, 0.f }),
                                       Raz::Vec3f({ 0.f, 1.f, 0.f}),
                                       Raz::Vec3f({ 1.f, 1.f, 1.f })));

  while (window.run())
    scene.render();

  return EXIT_SUCCESS;
}
