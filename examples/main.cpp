#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::Window window(800, 600, "RaZ");

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/frag.glsl");

  Raz::Scene scene(vertShader, fragShader);

  Raz::Material defaultMaterial;
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
  model->scale(0.075f);

  Raz::LightPtr light = std::make_unique<Raz::PointLight>(Raz::Vec3f({ 0.f, 3.f, 0.f }),  // Position
                                                          Raz::Vec3f({ 1.f, 1.f, 1.f })); // Color

  Raz::CameraPtr camera = std::make_unique<Raz::Camera>(window.getWidth(),
                                                        window.getHeight(),
                                                        45.f,                             // Field of view
                                                        0.1f, 100.f,                      // Near plane, far plane
                                                        Raz::Vec3f({ 0.f, 0.f, -3.f }));  // Initial position

  const auto modelPtr = model.get();
  const auto lightPtr = light.get();
  const auto cameraPtr = camera.get();

  // Camera controls
  window.addKeyCallback(Raz::Keyboard::W, [&cameraPtr] () { cameraPtr->translate(0.f, 0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::S, [&cameraPtr] () { cameraPtr->translate(0.f, -0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::A, [&cameraPtr] () { cameraPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::D, [&cameraPtr] () { cameraPtr->translate(0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::F, [&cameraPtr, &modelPtr] () { cameraPtr->lookAt(modelPtr->getPosition()); });

  // Mesh control
  window.addKeyCallback(Raz::Keyboard::Q, [&modelPtr] () { modelPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::E, [&modelPtr] () { modelPtr->translate(0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::X, [&modelPtr] () { modelPtr->scale(0.5f); });
  window.addKeyCallback(Raz::Keyboard::C, [&modelPtr] () { modelPtr->scale(2.f); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] () { modelPtr->rotate(10.f, 0.f, 1.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::LEFT, [&modelPtr] () { modelPtr->rotate(-10.f, 0.f, 1.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::UP, [&modelPtr] () { modelPtr->rotate(10.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::DOWN, [&modelPtr] () { modelPtr->rotate(-10.f, 1.f, 0.f, 0.f); });

  // Light control
  window.addKeyCallback(Raz::Keyboard::I, [&lightPtr] () { lightPtr->translate(0.f, 0.f, 0.5f); });
  window.addKeyCallback(Raz::Keyboard::K, [&lightPtr] () { lightPtr->translate(0.f, 0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::J, [&lightPtr] () { lightPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::L, [&lightPtr] () { lightPtr->translate(0.5f, 0.f, 0.f); });

  scene.addModel(std::move(model));
  scene.addLight(std::move(light));
  scene.setCamera(std::move(camera));

  while (window.run())
    scene.render();

  return EXIT_SUCCESS;
}
