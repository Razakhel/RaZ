#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::WindowPtr window = std::make_unique<Raz::Window>(1280, 720, "RaZ - Full demo", 4);
  window->enableOverlay();

  Raz::VertexShaderPtr vertShader = std::make_unique<Raz::VertexShader>("../../shaders/vert.glsl");
  Raz::FragmentShaderPtr fragShader;

  const auto img = std::make_shared<Raz::Texture>("../../assets/textures/lava.tga");

  const auto startTime = std::chrono::system_clock::now();
  // If FBX SDK linked, load an FBX mesh
#if defined(FBX_ENABLED)
  fragShader = std::make_unique<Raz::FragmentShader>("../../shaders/blinn-phong.glsl");
  Raz::ModelPtr model = Raz::Model::import("../../assets/meshes/shaderBall.fbx");

  dynamic_cast<Raz::MaterialStandard*>(model->getMesh()->getMaterials().front().get())->setDiffuseMap(img);

  model->translate(0.f, -1.f, 0.f);
  model->scale(0.01f);
  model->rotate(180.f, 0.f, 1.f, 0.f);
#else
  fragShader = std::make_unique<Raz::FragmentShader>("../../shaders/cook-torrance.glsl");
  Raz::ModelPtr model = Raz::Model::import("../../assets/meshes/shield.obj");

  dynamic_cast<Raz::MaterialCookTorrance*>(model->getMesh()->getMaterials().front().get())->setAlbedoMap(img);

  model->scale(0.2f);
  model->rotate(180.f, 0.f, 1.f, 0.f);
#endif
  const auto endTime = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  Raz::ScenePtr scene = std::make_unique<Raz::Scene>(std::move(vertShader), std::move(fragShader));

  Raz::CubemapPtr cubemap = std::make_unique<Raz::Cubemap>("../../assets/skyboxes/clouds_right.png",
                                                           "../../assets/skyboxes/clouds_left.png",
                                                           "../../assets/skyboxes/clouds_top.png",
                                                           "../../assets/skyboxes/clouds_bottom.png",
                                                           "../../assets/skyboxes/clouds_front.png",
                                                           "../../assets/skyboxes/clouds_back.png");

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
  windowPtr->addKeyCallback(Raz::Keyboard::Z, [&isWireframe] (float /* deltaTime */) {
    isWireframe = !isWireframe;
    glPolygonMode(GL_FRONT_AND_BACK, (isWireframe ? GL_LINE : GL_FILL));
  });

  // Allow face culling toggling
  bool hasFaceCulling = true;
  windowPtr->addKeyCallback(Raz::Keyboard::N, [&hasFaceCulling, &windowPtr] (float /* deltaTime */) {
    windowPtr->enableFaceCulling((hasFaceCulling = !hasFaceCulling));
  });

  windowPtr->addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });

  // Camera controls
  float cameraSpeed = 1.f;
  windowPtr->addKeyCallback(Raz::Keyboard::LEFT_SHIFT,
                            [&cameraSpeed] (float /* deltaTime */) { cameraSpeed = 2.f; },
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
    cameraPtr->rotate(-1.f, 90.f * deltaTime, 0.f, 0.f); // Looking up
  });
  windowPtr->addKeyCallback(Raz::Keyboard::NUM2, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(1.f, 90.f * deltaTime, 0.f, 0.f); // Looking down
  });
  windowPtr->addKeyCallback(Raz::Keyboard::NUM4, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(-1.f, 0.f, 90.f * deltaTime, 0.f); // Looking left
  });
  windowPtr->addKeyCallback(Raz::Keyboard::NUM6, [&cameraPtr] (float deltaTime) {
    cameraPtr->rotate(1.f, 0.f, 90.f * deltaTime, 0.f); // Looking right
  });

  // Mesh controls
  windowPtr->addKeyCallback(Raz::Keyboard::T, [&modelPtr] (float deltaTime) { modelPtr->move(0.f, 0.f,  10.f * deltaTime); });
  windowPtr->addKeyCallback(Raz::Keyboard::G, [&modelPtr] (float deltaTime) { modelPtr->move(0.f, 0.f, -10.f * deltaTime); });
  windowPtr->addKeyCallback(Raz::Keyboard::F, [&modelPtr] (float deltaTime) { modelPtr->move(-10.f * deltaTime, 0.f, 0.f); });
  windowPtr->addKeyCallback(Raz::Keyboard::H, [&modelPtr] (float deltaTime) { modelPtr->move( 10.f * deltaTime, 0.f, 0.f); });

  windowPtr->addKeyCallback(Raz::Keyboard::UP, [&modelPtr] (float deltaTime) {
    modelPtr->rotate(-1.f, 90.f * deltaTime, 0.f, 0.f);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::DOWN, [&modelPtr] (float deltaTime) {
    modelPtr->rotate( 1.f, 90.f * deltaTime, 0.f, 0.f);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::LEFT, [&modelPtr] (float deltaTime) {
    modelPtr->rotate(-1.f, 0.f, 90.f * deltaTime, 0.f);
  });
  windowPtr->addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] (float deltaTime) {
    modelPtr->rotate( 1.f, 0.f, 90.f * deltaTime, 0.f);
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

  windowPtr->addMouseButtonCallback(Raz::Mouse::LEFT_CLICK, [&scenePtr, &cameraPtr] (float /* deltaTime */) {
    scenePtr->addLight(std::make_unique<Raz::PointLight>(cameraPtr->getPosition(), 10.f, Raz::Vec3f({ 1.f, 1.f, 1.f })));
    scenePtr->updateLights();
  });

  windowPtr->addKeyCallback(Raz::Keyboard::F5, [&app] (float /* deltaTime */) { app.updateShaders(); });

  // Mouse callbacks
  windowPtr->addMouseScrollCallback([&cameraPtr] (double /* xOffset */, double yOffset) {
    cameraPtr->setFieldOfView(std::max(1.f, std::min(90.f, cameraPtr->getFieldOfViewDegrees() + static_cast<float>(-yOffset) * 2.f)));
  });

  windowPtr->disableCursor(); // Disabling mouse cursor to allow continous rotations
  windowPtr->addMouseMoveCallback([&cameraPtr, &windowPtr] (double xMove, double yMove) {
    cameraPtr->rotate(1.f,
                      (static_cast<float>(yMove) / windowPtr->getHeight() * 90.f), // X & Y moves are inverted, unsure of why for now
                      (static_cast<float>(xMove) / windowPtr->getWidth() * 90.f),  // Dividing by window size to scale between -1 and 1
                      0.f);
  });

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
