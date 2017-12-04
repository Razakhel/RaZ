#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::Window window(800, 600, "RaZ");

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/blinn-phong.glsl");

  Raz::Scene scene(vertShader, fragShader);

  Raz::Material floorMaterial;
  const Raz::TexturePtr floorTexture = std::make_shared<Raz::Texture>("../assets/textures/brick_wall.png");
  floorMaterial.setTexture(floorTexture);
  Raz::Material defaultMaterial;
  const Raz::TexturePtr defaultTexture = std::make_shared<Raz::Texture>("../assets/textures/lava.png");
  defaultMaterial.setTexture(defaultTexture);

  const auto startTime = std::chrono::system_clock::now();

  const Raz::MeshPtr floor = std::make_shared<Raz::Mesh>(Raz::Vec3f({ -10.f, -0.5f, 10.f }),    // Top left
                                                         Raz::Vec3f({ 10.f, -0.5f, 10.f }),     // Top right
                                                         Raz::Vec3f({ 10.f, -0.5f, -10.f }),    // Bottom right
                                                         Raz::Vec3f({ -10.f, -0.5f, -10.f }));  // Bottom left
  const Raz::MeshPtr mesh = std::make_shared<Raz::Mesh>("../assets/meshes/bigguy.obj");

  const auto endTime = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  Raz::ModelPtr floorModel = std::make_unique<Raz::Model>(floor);
  floorModel->setMaterial(floorMaterial);

  Raz::ModelPtr model = std::make_unique<Raz::Model>(mesh);
  model->setMaterial(defaultMaterial);
  model->scale(0.075f);

  Raz::ModelPtr model2 = std::make_unique<Raz::Model>(mesh);
  model2->setMaterial(defaultMaterial);
  model2->scale(0.075f);
  model2->translate(-1.5f, 0.f, 0.f);

  Raz::ModelPtr model3 = std::make_unique<Raz::Model>(mesh);
  model3->setMaterial(defaultMaterial);
  model3->scale(0.075f);
  model3->translate(1.5f, 0.f, 0.f);

  Raz::LightPtr light = std::make_unique<Raz::PointLight>(Raz::Vec3f({ 0.f, 1.f, 0.f }),  // Position
                                                          Raz::Vec3f({ 1.f, 1.f, 1.f })); // Color (R/G/B)

  Raz::CameraPtr camera = std::make_unique<Raz::Camera>(window.getWidth(),
                                                        window.getHeight(),
                                                        45.f,                             // Field of view
                                                        0.1f, 100.f,                      // Near plane, far plane
                                                        Raz::Vec3f({ 0.f, 0.f, -3.f }));  // Initial position

  const auto modelPtr = model.get();
  const auto lightPtr = light.get();
  const auto cameraPtr = camera.get();

  // Allow wireframe toggling
  bool isWireframe = false;
  window.addKeyCallback(Raz::Keyboard::Z, [&isWireframe] () {
    glPolygonMode(GL_FRONT_AND_BACK, ((isWireframe = !isWireframe) ? GL_LINE : GL_FILL));
  });

  // Camera controls
  window.addKeyCallback(Raz::Keyboard::SPACE, [&cameraPtr] () { cameraPtr->translate(0.f, 0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::V, [&cameraPtr] () { cameraPtr->translate(0.f, -0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::W, [&cameraPtr] () { cameraPtr->translate(0.f, 0.f, 0.5f); });
  window.addKeyCallback(Raz::Keyboard::S, [&cameraPtr] () { cameraPtr->translate(0.f, 0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::A, [&cameraPtr] () { cameraPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::D, [&cameraPtr] () { cameraPtr->translate(0.5f, 0.f, 0.f); });

  // Mesh control
  window.addKeyCallback(Raz::Keyboard::T, [&modelPtr] () { modelPtr->translate(0.f, 0.f, 0.5f); });
  window.addKeyCallback(Raz::Keyboard::G, [&modelPtr] () { modelPtr->translate(0.f, 0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::F, [&modelPtr] () { modelPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::H, [&modelPtr] () { modelPtr->translate(0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::X, [&modelPtr] () { modelPtr->scale(0.5f); });
  window.addKeyCallback(Raz::Keyboard::C, [&modelPtr] () { modelPtr->scale(2.f); });
  window.addKeyCallback(Raz::Keyboard::UP, [&modelPtr] () { modelPtr->rotate(10.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::DOWN, [&modelPtr] () { modelPtr->rotate(-10.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::LEFT, [&modelPtr] () { modelPtr->rotate(-10.f, 0.f, 1.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] () { modelPtr->rotate(10.f, 0.f, 1.f, 0.f); });

  // Light control
  window.addKeyCallback(Raz::Keyboard::I, [&lightPtr] () { lightPtr->translate(0.f, 0.f, 0.5f); });
  window.addKeyCallback(Raz::Keyboard::K, [&lightPtr] () { lightPtr->translate(0.f, 0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::J, [&lightPtr] () { lightPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::L, [&lightPtr] () { lightPtr->translate(0.5f, 0.f, 0.f); });

  scene.addModel(std::move(model));
  scene.addModel(std::move(model2));
  scene.addModel(std::move(model3));
  scene.addModel(std::move(floorModel));
  scene.addLight(std::move(light));
  scene.setCamera(std::move(camera));

  auto lastTime = std::chrono::system_clock::now();
  uint16_t nbFrames = 0;

  while (window.run()) {
    const auto currentTime = std::chrono::system_clock::now();
    ++nbFrames;

    if (std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime).count() >= 1.f) {
      std::cout << nbFrames << " FPS\r" << std::flush;

      nbFrames = 0;
      lastTime = currentTime;
    }

    scene.render();
  }

  return EXIT_SUCCESS;
}
