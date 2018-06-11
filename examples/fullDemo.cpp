#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::WindowPtr window = std::make_unique<Raz::Window>(1280, 720, "RaZ", 4);
  window->enableOverlay();

  Raz::VertexShaderPtr vertShader   = std::make_unique<Raz::VertexShader>("../../shaders/vert.glsl");
  Raz::FragmentShaderPtr fragShader = std::make_unique<Raz::FragmentShader>("../../shaders/cook-torrance.glsl");

  Raz::ScenePtr scene = std::make_unique<Raz::Scene>(std::move(vertShader), std::move(fragShader));

  Raz::CubemapPtr cubemap = std::make_unique<Raz::Cubemap>("../../assets/skyboxes/clouds_right.png",
                                                           "../../assets/skyboxes/clouds_left.png",
                                                           "../../assets/skyboxes/clouds_top.png",
                                                           "../../assets/skyboxes/clouds_bottom.png",
                                                           "../../assets/skyboxes/clouds_front.png",
                                                           "../../assets/skyboxes/clouds_back.png");

  const auto startTime = std::chrono::system_clock::now();
  Raz::ModelPtr model  = Raz::ModelLoader::importModel("../../assets/meshes/shield.obj");
  const auto endTime   = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  model->scale(0.2f);
  model->rotate(180.f, 0.f, 1.f, 0.f);

  /*Raz::LightPtr light = std::make_unique<Raz::PointLight>(Raz::Vec3f({ 0.f, 1.f, 0.f }),  // Position
                                                          10.f,                           // Energy
                                                          Raz::Vec3f({ 1.f, 1.f, 1.f })); // Color (R/G/B)*/
  Raz::LightPtr light = std::make_unique<Raz::DirectionalLight>(Raz::Vec3f({ 0.f, 0.f, 1.f }),  // Direction
                                                                1.f,                            // Energy
                                                                Raz::Vec3f({ 1.f, 1.f, 1.f }),  // Color (R/G/B)
                                                                Raz::Vec3f({ 0.f, 1.f, 0.f })); // Position

  Raz::CameraPtr camera = std::make_unique<Raz::Camera>(window->getWidth(),
                                                        window->getHeight(),
                                                        45.f,                            // Field of view
                                                        0.1f, 100.f,                     // Near plane, far plane
                                                        Raz::Vec3f({ 0.f, 0.f, -5.f })); // Initial position

  const auto modelPtr  = model.get();
  const auto lightPtr  = light.get();
  const auto cameraPtr = camera.get();

  scene->setCubemap(std::move(cubemap));
  scene->addModel(std::move(model));
  scene->addLight(std::move(light));
  scene->updateLights();
  scene->load();

  Raz::Application app(std::move(window), std::move(scene), std::move(camera));

  auto& windowPtr = app.getWindow();
  auto& scenePtr  = app.getScene();

  // Allow wireframe toggling
  bool isWireframe = false;
  windowPtr->addKeyCallback(Raz::Keyboard::Z, [&isWireframe] () {
    glPolygonMode(GL_FRONT_AND_BACK, ((isWireframe = !isWireframe) ? GL_LINE : GL_FILL));
  });

  // Allow face culling toggling
  bool hasFaceCulling = true;
  windowPtr->addKeyCallback(Raz::Keyboard::N, [&hasFaceCulling, &windowPtr] () {
    windowPtr->enableFaceCulling((hasFaceCulling = !hasFaceCulling));
  });

  // Camera controls
  windowPtr->addKeyCallback(Raz::Keyboard::SPACE, [&cameraPtr] () { cameraPtr->move( 0.f,  0.5f, 0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::V,     [&cameraPtr] () { cameraPtr->move( 0.f, -0.5f, 0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::W,     [&cameraPtr] () { cameraPtr->move( 0.f,  0.f,  0.5f); });
  windowPtr->addKeyCallback(Raz::Keyboard::S,     [&cameraPtr] () { cameraPtr->move( 0.f,  0.f, -0.5f); });
  windowPtr->addKeyCallback(Raz::Keyboard::A,     [&cameraPtr] () { cameraPtr->move(-0.5f, 0.f,  0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::D,     [&cameraPtr] () { cameraPtr->move( 0.5f, 0.f,  0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::NUM8,  [&cameraPtr] () { cameraPtr->rotate(-10.f, 1.f, 0.f, 0.f); }); // Looking up
  windowPtr->addKeyCallback(Raz::Keyboard::NUM2,  [&cameraPtr] () { cameraPtr->rotate( 10.f, 1.f, 0.f, 0.f); }); // Looking down
  windowPtr->addKeyCallback(Raz::Keyboard::NUM4,  [&cameraPtr] () { cameraPtr->rotate(-10.f, 0.f, 1.f, 0.f); }); // Looking left
  windowPtr->addKeyCallback(Raz::Keyboard::NUM6,  [&cameraPtr] () { cameraPtr->rotate( 10.f, 0.f, 1.f, 0.f); }); // Looking right

  // Mesh controls
  windowPtr->addKeyCallback(Raz::Keyboard::T,     [&modelPtr] () { modelPtr->move( 0.f,  0.f,  0.5f); });
  windowPtr->addKeyCallback(Raz::Keyboard::G,     [&modelPtr] () { modelPtr->move( 0.f,  0.f, -0.5f); });
  windowPtr->addKeyCallback(Raz::Keyboard::F,     [&modelPtr] () { modelPtr->move(-0.5f, 0.f,  0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::H,     [&modelPtr] () { modelPtr->move( 0.5f, 0.f,  0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::X,     [&modelPtr] () { modelPtr->scale(0.5f); });
  windowPtr->addKeyCallback(Raz::Keyboard::C,     [&modelPtr] () { modelPtr->scale(2.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::UP,    [&modelPtr] () { modelPtr->rotate(-10.f, 1.f, 0.f, 0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::DOWN,  [&modelPtr] () { modelPtr->rotate( 10.f, 1.f, 0.f, 0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::LEFT,  [&modelPtr] () { modelPtr->rotate(-10.f, 0.f, 1.f, 0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] () { modelPtr->rotate( 10.f, 0.f, 1.f, 0.f); });

  // Light controls
  windowPtr->addKeyCallback(Raz::Keyboard::I, [&lightPtr, &scenePtr] () {
    lightPtr->translate(0.f, 0.f, 0.5f);
    scenePtr->updateLights();
  });
  windowPtr->addKeyCallback(Raz::Keyboard::K, [&lightPtr, &scenePtr] () {
    lightPtr->translate(0.f, 0.f, -0.5f);
    scenePtr->updateLights();
  });
  windowPtr->addKeyCallback(Raz::Keyboard::J, [&lightPtr, &scenePtr] () {
    lightPtr->translate(-0.5f, 0.f, 0.f);
    scenePtr->updateLights();
  });
  windowPtr->addKeyCallback(Raz::Keyboard::L, [&lightPtr, &scenePtr] () {
    lightPtr->translate(0.5f, 0.f, 0.f);
    scenePtr->updateLights();
  });

  windowPtr->addKeyCallback(Raz::Keyboard::F5, [&app] () { app.updateShaders(); });

  // Overlay features
  windowPtr->addOverlayText("RaZ - Full demo");
  windowPtr->addOverlayButton("Scale down mesh", [&modelPtr] () { modelPtr->scale(0.5f); });
  windowPtr->addOverlayButton("Scale up mesh", [&modelPtr] () { modelPtr->scale(2.f); });
  windowPtr->addOverlayCheckbox("Enable vertical sync",                                 // Text
                                windowPtr->recoverVerticalSyncState(),                  // Initial state
                                [&windowPtr] () { windowPtr->enableVerticalSync(); },   // Action if toggled on
                                [&windowPtr] () { windowPtr->disableVerticalSync(); }); // Action if toggled off
  windowPtr->addOverlayFrameTime("Frame time: %.3f ms/frame"); // Frame time's & FPS counter's texts must be formatted
  windowPtr->addOverlayFpsCounter("FPS: %.1f");

  auto lastTime = std::chrono::system_clock::now();
  uint16_t nbFrames = 0;

  while (app.run()) {
    const auto currentTime = std::chrono::system_clock::now();
    ++nbFrames;

    if (std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime).count() >= 1.f) {
      app.getWindow()->setTitle("RaZ - " + std::to_string(nbFrames) + " FPS");

      nbFrames = 0;
      lastTime = currentTime;
    }
  }

  return EXIT_SUCCESS;
}
