#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::WindowPtr window = Raz::Window::create(1280, 720, "RaZ - Full demo", 4);
  window->enableOverlay();

  const auto startTime = std::chrono::system_clock::now();
  Raz::ModelPtr model = Raz::Model::import("../../assets/meshes/shield.obj");
  const auto endTime = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  model->scale(0.2f);
  model->rotate(180.f, Raz::Axis::Y);

  Raz::ScenePtr scene = Raz::Scene::create(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                           Raz::FragmentShader::create("../../shaders/cook-torrance.glsl"));

  Raz::CubemapPtr cubemap = Raz::Cubemap::create("../../assets/skyboxes/clouds_right.png",
                                                 "../../assets/skyboxes/clouds_left.png",
                                                 "../../assets/skyboxes/clouds_top.png",
                                                 "../../assets/skyboxes/clouds_bottom.png",
                                                 "../../assets/skyboxes/clouds_front.png",
                                                 "../../assets/skyboxes/clouds_back.png");

  /*Raz::LightPtr light = Raz::PointLight::create(Raz::Vec3f({ 0.f, 1.f, 0.f }),  // Position
                                                  10.f,                           // Energy
                                                  Raz::Vec3f({ 1.f, 1.f, 1.f })); // Color (R/G/B)*/
  Raz::LightPtr light = Raz::DirectionalLight::create(Raz::Vec3f({ 0.f, 0.f, 1.f }),  // Direction
                                                      1.f,                            // Energy
                                                      Raz::Vec3f({ 1.f, 1.f, 1.f }),  // Color (R/G/B)
                                                      Raz::Vec3f({ 0.f, 1.f, 0.f })); // Position

  Raz::CameraPtr camera = Raz::Camera::create(window->getWidth(),
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
  windowPtr->addKeyCallback(Raz::Keyboard::Z, [&isWireframe] (float /* deltaTime */) {
    isWireframe = !isWireframe;
    glPolygonMode(GL_FRONT_AND_BACK, (isWireframe ? GL_LINE : GL_FILL));
  }, Raz::Input::ONCE);

  // Allow face culling toggling
  bool hasFaceCulling = true;
  windowPtr->addKeyCallback(Raz::Keyboard::N, [&hasFaceCulling, &windowPtr] (float /* deltaTime */) {
    windowPtr->enableFaceCulling((hasFaceCulling = !hasFaceCulling));
  }, Raz::Input::ONCE);

  windowPtr->addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });

  // Camera controls
  float cameraSpeed = 1.f;
  windowPtr->addKeyCallback(Raz::Keyboard::LEFT_SHIFT,
                            [&cameraSpeed] (float /* deltaTime */) { cameraSpeed = 2.f; },
                            Raz::Input::ONCE,
                            [&cameraSpeed] () { cameraSpeed = 1.f; });
  windowPtr->addKeyCallback(Raz::Keyboard::SPACE, [&cameraPtr, &cameraSpeed] (float deltaTime) {
    cameraPtr->move(0.f, (10.f * deltaTime) * cameraSpeed, 0.f);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::V, [&cameraPtr, &cameraSpeed] (float deltaTime) {
    cameraPtr->move(0.f, (-10.f * deltaTime) * cameraSpeed, 0.f);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::W, [&cameraPtr, &cameraSpeed] (float deltaTime) {
    cameraPtr->move(0.f, 0.f, (10.f * deltaTime) * cameraSpeed);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::S, [&cameraPtr, &cameraSpeed] (float deltaTime) {
    cameraPtr->move(0.f,  0.f, (-10.f * deltaTime) * cameraSpeed);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::A, [&cameraPtr, &cameraSpeed] (float deltaTime) {
    cameraPtr->move((-10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::D, [&cameraPtr, &cameraSpeed] (float deltaTime) {
    cameraPtr->move((10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });

  windowPtr->addKeyCallback(Raz::Keyboard::NUM8, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(-90.f * deltaTime, Raz::Axis::X); // Looking up
  });
  windowPtr->addKeyCallback(Raz::Keyboard::NUM2, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(90.f * deltaTime, Raz::Axis::X); // Looking down
  });
  windowPtr->addKeyCallback(Raz::Keyboard::NUM4, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(-90.f * deltaTime, Raz::Axis::Y); // Looking left
  });
  windowPtr->addKeyCallback(Raz::Keyboard::NUM6, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(90.f * deltaTime, Raz::Axis::Y); // Looking right
  });
  // DO A BARREL ROLL
  windowPtr->addKeyCallback(Raz::Keyboard::Q, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(90.f * deltaTime, Raz::Axis::Z); // Roll to the left
  });
  windowPtr->addKeyCallback(Raz::Keyboard::E, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(-90.f * deltaTime, Raz::Axis::Z); // Roll to the right
  });

  // Mesh controls
  windowPtr->addKeyCallback(Raz::Keyboard::T, [&modelPtr] (float deltaTime) { modelPtr->move(0.f, 0.f,  10.f * deltaTime); });
  windowPtr->addKeyCallback(Raz::Keyboard::G, [&modelPtr] (float deltaTime) { modelPtr->move(0.f, 0.f, -10.f * deltaTime); });
  windowPtr->addKeyCallback(Raz::Keyboard::F, [&modelPtr] (float deltaTime) { modelPtr->move(-10.f * deltaTime, 0.f, 0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::H, [&modelPtr] (float deltaTime) { modelPtr->move( 10.f * deltaTime, 0.f, 0.f); });

  windowPtr->addKeyCallback(Raz::Keyboard::X, [&modelPtr] (float /* deltaTime */) { modelPtr->scale(0.5f); }, Raz::Input::ONCE);
  windowPtr->addKeyCallback(Raz::Keyboard::C, [&modelPtr] (float /* deltaTime */) { modelPtr->scale(2.f); }, Raz::Input::ONCE);

  windowPtr->addKeyCallback(Raz::Keyboard::UP, [&modelPtr] (float deltaTime) {
    modelPtr->rotate(-90.f * deltaTime, Raz::Axis::X);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::DOWN, [&modelPtr] (float deltaTime) {
    modelPtr->rotate(90.f * deltaTime, Raz::Axis::X);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::LEFT, [&modelPtr] (float deltaTime) {
    modelPtr->rotate(-90.f * deltaTime, Raz::Axis::Y);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] (float deltaTime) {
    modelPtr->rotate(90.f * deltaTime, Raz::Axis::Y);
  });

  // Light controls
  windowPtr->addKeyCallback(Raz::Keyboard::I, [&lightPtr, &scenePtr] (float deltaTime) {
    lightPtr->translate(0.f, 0.f, 10.f * deltaTime);
    scenePtr->updateLights();
  });
  windowPtr->addKeyCallback(Raz::Keyboard::K, [&lightPtr, &scenePtr] (float deltaTime) {
    lightPtr->translate(0.f, 0.f, -10.f * deltaTime);
    scenePtr->updateLights();
  });
  windowPtr->addKeyCallback(Raz::Keyboard::J, [&lightPtr, &scenePtr] (float deltaTime) {
    lightPtr->translate(-10.f * deltaTime, 0.f, 0.f);
    scenePtr->updateLights();
  });
  windowPtr->addKeyCallback(Raz::Keyboard::L, [&lightPtr, &scenePtr] (float deltaTime) {
    lightPtr->translate(10.f * deltaTime, 0.f, 0.f);
    scenePtr->updateLights();
  });

  windowPtr->addMouseButtonCallback(Raz::Mouse::RIGHT_CLICK, [&scenePtr, &cameraPtr] (float /* deltaTime */) {
    scenePtr->addLight(Raz::PointLight::create(cameraPtr->getPosition(), 10.f, Raz::Vec3f({ 1.f, 1.f, 1.f })));
    scenePtr->updateLights();
  }, Raz::Input::ONCE);

  windowPtr->addKeyCallback(Raz::Keyboard::F5, [&app] (float /* deltaTime */) { app.updateShaders(); });

  // Mouse callbacks
  windowPtr->addMouseScrollCallback([&cameraPtr] (double /* xOffset */, double yOffset) {
    cameraPtr->setFieldOfView(std::max(1.f, std::min(90.f, cameraPtr->getFieldOfViewDegrees() + static_cast<float>(-yOffset) * 2.f)));
  });

  windowPtr->addMouseMoveCallback([&cameraPtr, &windowPtr] (double xMove, double yMove) {
    // Dividing move by window size to scale between -1 and 1
    // X & Y moves are inverted, unsure of why for now
    cameraPtr->rotate(90.f * static_cast<float>(yMove) / windowPtr->getHeight(),
                      90.f * static_cast<float>(xMove) / windowPtr->getWidth(),
                      0.f);
  });

  windowPtr->disableCursor(); // Disabling mouse cursor to allow continous rotations
  windowPtr->addKeyCallback(Raz::Keyboard::LEFT_ALT,
                            [&windowPtr] (float /* deltaTime */) { windowPtr->showCursor(); },
                            Raz::Input::ONCE,
                            [&windowPtr] () { windowPtr->disableCursor(); });

  // Overlay features
  windowPtr->addOverlayText("RaZ - Full demo");
  windowPtr->addOverlayText("Scene's informations:");
  windowPtr->addOverlayText(std::to_string(scenePtr->recoverVertexCount()) + " vertices, "
                          + std::to_string(scenePtr->recoverTriangleCount()) + " triangles");
  windowPtr->addOverlayCheckbox("Enable face culling",
                                true,
                                [&windowPtr] () { windowPtr->enableFaceCulling(); },
                                [&windowPtr] () { windowPtr->disableFaceCulling(); });
  windowPtr->addOverlayCheckbox("Enable vertical sync",
                                windowPtr->recoverVerticalSyncState(),
                                [&windowPtr] () { windowPtr->enableVerticalSync(); },
                                [&windowPtr] () { windowPtr->disableVerticalSync(); });
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
