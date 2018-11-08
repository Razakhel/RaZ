#include "RaZ/RaZ.hpp"

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(Raz::World(10));

  auto& renderSystem = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");
  renderSystem.setProgram(Raz::ShaderProgram(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                             Raz::FragmentShader::create("../../shaders/cook-torrance.glsl")));

  Raz::Window& window = renderSystem.getWindow();
  window.enableOverlay();

  Raz::Entity& camera = renderSystem.getCameraEntity();
  auto& cameraComp    = camera.getComponent<Raz::Camera>();
  auto& cameraTrans   = camera.getComponent<Raz::Transform>();
  cameraTrans.setPosition(Raz::Vec3f({0.f, 0.f, -5.f}));

  Raz::Entity& mesh = world.addEntity();
  auto& meshTrans   = mesh.addComponent<Raz::Transform>();
  mesh.addComponent<Raz::Mesh>("../../assets/meshes/shield.obj");

  meshTrans.scale(0.2f);
  meshTrans.rotate(180.f, Raz::Axis::Y);

  renderSystem.setCubemap(Raz::Cubemap::create("../../assets/skyboxes/clouds_right.png", "../../assets/skyboxes/clouds_left.png",
                                               "../../assets/skyboxes/clouds_top.png", "../../assets/skyboxes/clouds_bottom.png",
                                               "../../assets/skyboxes/clouds_front.png", "../../assets/skyboxes/clouds_back.png"));

  Raz::Entity& light = world.addEntity();
  /*auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::POINT, // Type
                                                   1.f,                   // Energy
                                                   Raz::Vec3f(1.f));      // Color(RGB)*/
  auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL,   // Type
                                                   Raz::Vec3f({ 0.f, 0.f, 1.f }), // Direction
                                                   1.f,                           // Energy
                                                   Raz::Vec3f(1.f));              // Color (RGB)
  auto& lightTrans = light.addComponent<Raz::Transform>(Raz::Vec3f({ 0.f, 1.f, 0.f }));

  window.addKeyCallback(Raz::Keyboard::R, [&mesh] (float /* deltaTime */) { mesh.disable(); },
                        Raz::Input::ONCE,
                        [&mesh] () { mesh.enable(); });

  // Allow wireframe toggling
  bool isWireframe = false;
  window.addKeyCallback(Raz::Keyboard::Z, [&isWireframe] (float /* deltaTime */) {
    isWireframe = !isWireframe;
    glPolygonMode(GL_FRONT_AND_BACK, (isWireframe ? GL_LINE : GL_FILL));
  }, Raz::Input::ONCE);

  // Allow face culling toggling
  bool hasFaceCulling = true;
  window.addKeyCallback(Raz::Keyboard::N, [&hasFaceCulling, &window] (float /* deltaTime */) {
    window.enableFaceCulling((hasFaceCulling = !hasFaceCulling));
  }, Raz::Input::ONCE);

  window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });

  // Camera controls
  float cameraSpeed = 1.f;
  window.addKeyCallback(Raz::Keyboard::LEFT_SHIFT,
                        [&cameraSpeed] (float /* deltaTime */) { cameraSpeed = 2.f; },
                        Raz::Input::ONCE,
                        [&cameraSpeed] () { cameraSpeed = 1.f; });
  window.addKeyCallback(Raz::Keyboard::SPACE, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move(0.f, (10.f * deltaTime) * cameraSpeed, 0.f);
  });
  window.addKeyCallback(Raz::Keyboard::V, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move(0.f, (-10.f * deltaTime) * cameraSpeed, 0.f);
  });
  window.addKeyCallback(Raz::Keyboard::W, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move(0.f, 0.f, (10.f * deltaTime) * cameraSpeed);
  });
  window.addKeyCallback(Raz::Keyboard::S, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move(0.f,  0.f, (-10.f * deltaTime) * cameraSpeed);
  });
  window.addKeyCallback(Raz::Keyboard::A, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move((-10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });
  window.addKeyCallback(Raz::Keyboard::D, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move((10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });

  window.addKeyCallback(Raz::Keyboard::NUM8, [&cameraTrans] (float deltaTime) {
    cameraTrans.rotate(-90.f * deltaTime, Raz::Axis::X); // Looking up
  });
  window.addKeyCallback(Raz::Keyboard::NUM2, [&cameraTrans] (float deltaTime) {
    cameraTrans.rotate(90.f * deltaTime, Raz::Axis::X); // Looking down
  });
  window.addKeyCallback(Raz::Keyboard::NUM4, [&cameraTrans] (float deltaTime) {
    cameraTrans.rotate(-90.f * deltaTime, Raz::Axis::Y); // Looking left
  });
  window.addKeyCallback(Raz::Keyboard::NUM6, [&cameraTrans] (float deltaTime) {
    cameraTrans.rotate(90.f * deltaTime, Raz::Axis::Y); // Looking right
  });
  // DO A BARREL ROLL
  window.addKeyCallback(Raz::Keyboard::Q, [&cameraTrans] (float deltaTime) {
    cameraTrans.rotate(90.f * deltaTime, Raz::Axis::Z); // Roll to the left
  });
  window.addKeyCallback(Raz::Keyboard::E, [&cameraTrans] (float deltaTime) {
    cameraTrans.rotate(-90.f * deltaTime, Raz::Axis::Z); // Roll to the right
  });

  // Mesh controls
  window.addKeyCallback(Raz::Keyboard::T, [&meshTrans] (float deltaTime) { meshTrans.move(0.f, 0.f,  10.f * deltaTime); });
  window.addKeyCallback(Raz::Keyboard::G, [&meshTrans] (float deltaTime) { meshTrans.move(0.f, 0.f, -10.f * deltaTime); });
  window.addKeyCallback(Raz::Keyboard::F, [&meshTrans] (float deltaTime) { meshTrans.move(-10.f * deltaTime, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::H, [&meshTrans] (float deltaTime) { meshTrans.move( 10.f * deltaTime, 0.f, 0.f); });

  window.addKeyCallback(Raz::Keyboard::X, [&meshTrans] (float /* deltaTime */) { meshTrans.scale(0.5f); }, Raz::Input::ONCE);
  window.addKeyCallback(Raz::Keyboard::C, [&meshTrans] (float /* deltaTime */) { meshTrans.scale(2.f); }, Raz::Input::ONCE);

  window.addKeyCallback(Raz::Keyboard::UP,    [&meshTrans] (float deltaTime) { meshTrans.rotate(-90.f * deltaTime, Raz::Axis::X); });
  window.addKeyCallback(Raz::Keyboard::DOWN,  [&meshTrans] (float deltaTime) { meshTrans.rotate( 90.f * deltaTime, Raz::Axis::X); });
  window.addKeyCallback(Raz::Keyboard::LEFT,  [&meshTrans] (float deltaTime) { meshTrans.rotate(-90.f * deltaTime, Raz::Axis::Y); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&meshTrans] (float deltaTime) { meshTrans.rotate( 90.f * deltaTime, Raz::Axis::Y); });

  // Light controls
  window.addKeyCallback(Raz::Keyboard::I, [&lightTrans, &renderSystem] (float deltaTime) {
    lightTrans.translate(0.f, 0.f, 10.f * deltaTime);
    renderSystem.updateLights();
  });
  window.addKeyCallback(Raz::Keyboard::K, [&lightTrans, &renderSystem] (float deltaTime) {
    lightTrans.translate(0.f, 0.f, -10.f * deltaTime);
    renderSystem.updateLights();
  });
  window.addKeyCallback(Raz::Keyboard::J, [&lightTrans, &renderSystem] (float deltaTime) {
    lightTrans.translate(-10.f * deltaTime, 0.f, 0.f);
    renderSystem.updateLights();
  });
  window.addKeyCallback(Raz::Keyboard::L, [&lightTrans, &renderSystem] (float deltaTime) {
    lightTrans.translate(10.f * deltaTime, 0.f, 0.f);
    renderSystem.updateLights();
  });
  window.addKeyCallback(Raz::Keyboard::PAGEUP, [&lightComp, &renderSystem] (float deltaTime) {
    lightComp.setEnergy(lightComp.getEnergy() + 1.f * deltaTime);
    renderSystem.updateLights();
  });
  window.addKeyCallback(Raz::Keyboard::PAGEDOWN, [&lightComp, &renderSystem] (float deltaTime) {
    lightComp.setEnergy(std::max(0.f, lightComp.getEnergy() - 1.f * deltaTime));
    renderSystem.updateLights();
  });

  window.addMouseButtonCallback(Raz::Mouse::RIGHT_CLICK, [&world, &cameraTrans] (float /* deltaTime */) {
    auto& newLight = world.addEntityWithComponent<Raz::Light>(true,                  // Enabled
                                                              Raz::LightType::POINT, // Type (point light)
                                                              10.f);                 // Energy
    newLight.addComponent<Raz::Transform>(cameraTrans.getPosition());
  }, Raz::Input::ONCE);

  window.addKeyCallback(Raz::Keyboard::F5, [&renderSystem] (float /* deltaTime */) { renderSystem.updateShaders(); });

  // Mouse callbacks
  window.addMouseScrollCallback([&cameraComp] (double /* xOffset */, double yOffset) {
    cameraComp.setFieldOfView(std::max(15.f, std::min(90.f, cameraComp.getFieldOfViewDegrees() + static_cast<float>(-yOffset) * 2.f)));
  });

  window.addMouseMoveCallback([&cameraTrans, &window] (double xMove, double yMove) {
    // Dividing move by window size to scale between -1 and 1
    cameraTrans.rotate(90.f * static_cast<float>(yMove) / window.getHeight(),
                       90.f * static_cast<float>(xMove) / window.getWidth(),
                       0.f);
  });

  window.disableCursor(); // Disabling mouse cursor to allow continuous rotations
  window.addKeyCallback(Raz::Keyboard::LEFT_ALT,
                        [&window] (float /* deltaTime */) { window.showCursor(); },
                        Raz::Input::ONCE,
                        [&window] () { window.disableCursor(); });

  // Overlay features
  window.addOverlayText("RaZ - Full demo");
  window.addOverlaySeparator();
  window.addOverlayCheckbox("Enable face culling",
                            true,
                            [&window] () { window.enableFaceCulling(); },
                            [&window] () { window.disableFaceCulling(); });
  window.addOverlayCheckbox("Enable vertical sync",
                            window.recoverVerticalSyncState(),
                            [&window] () { window.enableVerticalSync(); },
                            [&window] () { window.disableVerticalSync(); });
  window.addOverlaySeparator();
  window.addOverlayFrameTime("Frame time: %.3f ms/frame"); // Frame time's & FPS counter's texts must be formatted
  window.addOverlayFpsCounter("FPS: %.1f");

  while (app.run());

  return EXIT_SUCCESS;
}
