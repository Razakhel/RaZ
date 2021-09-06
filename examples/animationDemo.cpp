#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  ////////////////////
  // Initialization //
  ////////////////////

  Raz::Application app;
  Raz::World& world = app.addWorld(10);

  ///////////////
  // Rendering //
  ///////////////

  auto& renderSystem = world.addSystem<Raz::RenderSystem>(1280u, 720u, "RaZ", Raz::WindowSetting::DEFAULT, 2);

  Raz::RenderPass& geometryPass = renderSystem.getGeometryPass();
  geometryPass.getProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s),
                                       Raz::FragmentShader(RAZ_ROOT + "shaders/blinn-phong.frag"s));

  renderSystem.setCubemap(Raz::Cubemap(RAZ_ROOT + "assets/skyboxes/clouds_right.png"s, RAZ_ROOT + "assets/skyboxes/clouds_left.png"s,
                                       RAZ_ROOT + "assets/skyboxes/clouds_top.png"s,   RAZ_ROOT + "assets/skyboxes/clouds_bottom.png"s,
                                       RAZ_ROOT + "assets/skyboxes/clouds_front.png"s, RAZ_ROOT + "assets/skyboxes/clouds_back.png"s));

  Raz::Window& window = renderSystem.getWindow();
  window.setIcon(RAZ_ROOT + "assets/icons/RaZ_logo_128.png"s);

#if !defined(USE_OPENGL_ES)
  // Allow wireframe toggling
  bool isWireframe = false;
  window.addKeyCallback(Raz::Keyboard::Z, [&isWireframe] (float /* deltaTime */) {
    isWireframe = !isWireframe;
    Raz::Renderer::setPolygonMode(Raz::FaceOrientation::FRONT_BACK, (isWireframe ? Raz::PolygonMode::LINE : Raz::PolygonMode::FILL));
  }, Raz::Input::ONCE);
#endif

  // Allowing to quit the application with the Escape key
  window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
  // Allowing to quit the application when the close button is clicked
  window.setCloseCallback([&app] () noexcept { app.quit(); });

  ///////////////////
  // Camera entity //
  ///////////////////

  Raz::Entity& camera = world.addEntity();
  auto& cameraComp    = camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());
  auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, -5.f));

  /////////////////
  // Mesh entity //
  /////////////////

  Raz::Entity& mesh = world.addEntity();
  auto& meshComp    = mesh.addComponent<Raz::Mesh>(RAZ_ROOT + "assets/meshes/character.obj"s);

  auto& meshTrans = mesh.addComponent<Raz::Transform>();
  meshTrans.rotate(180_deg, Raz::Axis::Y);

  //////////////////////////
  // Skeleton & animation //
  //////////////////////////

  world.addSystem<Raz::AnimationSystem>();

  Raz::BvhFormat bvh(RAZ_ROOT + "assets/animations/character.bvh"s);
  meshComp.setSkeleton(std::move(bvh.skeleton));

  auto& animation = mesh.addComponent<Raz::Animation>(std::move(bvh.animation));

  //////////////////
  // Light entity //
  //////////////////

  Raz::Entity& light = world.addEntity();
  auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                                   Raz::Vec3f(0.f, 0.f, 1.f),   // Direction
                                                   1.f,                         // Energy
                                                   Raz::Vec3f(1.f));            // Color (RGB)
  auto& lightTrans = light.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 0.f));

  /////////////////////
  // Camera controls //
  /////////////////////

  float cameraSpeed = 1.f;
  window.addKeyCallback(Raz::Keyboard::LEFT_SHIFT,
                        [&cameraSpeed] (float /* deltaTime */) noexcept { cameraSpeed = 2.f; },
                        Raz::Input::ONCE,
                        [&cameraSpeed] () noexcept { cameraSpeed = 1.f; });
  window.addKeyCallback(Raz::Keyboard::SPACE, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move(0.f, (10.f * deltaTime) * cameraSpeed, 0.f);
  });
  window.addKeyCallback(Raz::Keyboard::V, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move(0.f, (-10.f * deltaTime) * cameraSpeed, 0.f);
  });
  window.addKeyCallback(Raz::Keyboard::W, [&cameraTrans, &cameraComp, &cameraSpeed] (float deltaTime) {
    const float moveVal = (10.f * deltaTime) * cameraSpeed;

    cameraTrans.move(0.f, 0.f, moveVal);
    cameraComp.setOrthoBoundX(cameraComp.getOrthoBoundX() - moveVal);
    cameraComp.setOrthoBoundY(cameraComp.getOrthoBoundY() - moveVal);
  });
  window.addKeyCallback(Raz::Keyboard::S, [&cameraTrans, &cameraComp, &cameraSpeed] (float deltaTime) {
    const float moveVal = (-10.f * deltaTime) * cameraSpeed;

    cameraTrans.move(0.f, 0.f, moveVal);
    cameraComp.setOrthoBoundX(cameraComp.getOrthoBoundX() - moveVal);
    cameraComp.setOrthoBoundY(cameraComp.getOrthoBoundY() - moveVal);
  });
  window.addKeyCallback(Raz::Keyboard::A, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move((-10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });
  window.addKeyCallback(Raz::Keyboard::D, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move((10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });

  window.addKeyCallback(Raz::Keyboard::P, [&cameraComp] (float /* deltaTime */) {
    cameraComp.setProjectionType(Raz::ProjectionType::ORTHOGRAPHIC);
  }, Raz::Input::ONCE);

  window.addMouseScrollCallback([&cameraComp] (double /* xOffset */, double yOffset) {
    const float newFovDeg = std::clamp(Raz::Degreesf(cameraComp.getFieldOfView()).value + static_cast<float>(-yOffset) * 2.f, 15.f, 90.f);
    cameraComp.setFieldOfView(Raz::Degreesf(newFovDeg));
  });

  // The camera can be rotated while holding the mouse right click
  bool isRightClicking = false;

  window.addMouseButtonCallback(Raz::Mouse::RIGHT_CLICK, [&isRightClicking, &window] (float /* deltaTime */) {
    isRightClicking = true;
    window.disableCursor();
  }, Raz::Input::ONCE, [&isRightClicking, &window] () {
    isRightClicking = false;
    window.showCursor();
  });

  window.addMouseMoveCallback([&isRightClicking, &cameraTrans, &window] (double xMove, double yMove) {
    if (!isRightClicking)
      return;

    // Dividing movement by the window's size to scale between -1 and 1
    cameraTrans.rotate(-90_deg * static_cast<float>(yMove) / window.getHeight(),
                       -90_deg * static_cast<float>(xMove) / window.getWidth());
  });

  ///////////////////
  // Mesh controls //
  ///////////////////

  window.addKeyCallback(Raz::Keyboard::T, [&meshTrans] (float deltaTime) { meshTrans.move(0.f, 0.f,  10.f * deltaTime); });
  window.addKeyCallback(Raz::Keyboard::G, [&meshTrans] (float deltaTime) { meshTrans.move(0.f, 0.f, -10.f * deltaTime); });
  window.addKeyCallback(Raz::Keyboard::F, [&meshTrans] (float deltaTime) { meshTrans.move(-10.f * deltaTime, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::H, [&meshTrans] (float deltaTime) { meshTrans.move( 10.f * deltaTime, 0.f, 0.f); });

  window.addKeyCallback(Raz::Keyboard::X, [&meshTrans] (float /* deltaTime */) { meshTrans.scale(0.5f); }, Raz::Input::ONCE);
  window.addKeyCallback(Raz::Keyboard::C, [&meshTrans] (float /* deltaTime */) { meshTrans.scale(2.f); }, Raz::Input::ONCE);

  window.addKeyCallback(Raz::Keyboard::UP,    [&meshTrans] (float deltaTime) { meshTrans.rotate(-90_deg * deltaTime, Raz::Axis::X); });
  window.addKeyCallback(Raz::Keyboard::DOWN,  [&meshTrans] (float deltaTime) { meshTrans.rotate(90_deg * deltaTime, Raz::Axis::X); });
  window.addKeyCallback(Raz::Keyboard::LEFT,  [&meshTrans] (float deltaTime) { meshTrans.rotate(-90_deg * deltaTime, Raz::Axis::Y); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&meshTrans] (float deltaTime) { meshTrans.rotate(90_deg * deltaTime, Raz::Axis::Y); });

  ////////////////////
  // Light controls //
  ////////////////////

  window.addKeyCallback(Raz::Keyboard::PAGEUP, [&lightComp, &renderSystem] (float deltaTime) {
    lightComp.setEnergy(lightComp.getEnergy() + 1.f * deltaTime);
    renderSystem.updateLights();
  });
  window.addKeyCallback(Raz::Keyboard::PAGEDOWN, [&lightComp, &renderSystem] (float deltaTime) {
    lightComp.setEnergy(std::max(0.f, lightComp.getEnergy() - 1.f * deltaTime));
    renderSystem.updateLights();
  });

  window.addMouseButtonCallback(Raz::Mouse::MIDDLE_CLICK, [&world, &cameraTrans] (float /* deltaTime */) {
    auto& newLight = world.addEntityWithComponent<Raz::Light>(Raz::LightType::POINT, // Type (point light)
                                                              10.f);                 // Energy
    newLight.addComponent<Raz::Transform>(cameraTrans.getPosition());
  }, Raz::Input::ONCE);

  window.addKeyCallback(Raz::Keyboard::F5, [&renderSystem] (float /* deltaTime */) { renderSystem.updateShaders(); });

  /////////////
  // Overlay //
  /////////////

#if !defined(RAZ_NO_OVERLAY)
  window.enableOverlay();

  window.addOverlayLabel("RaZ - Animation demo");

  window.addOverlaySeparator();

  window.addOverlayCheckbox("Play animation",
                            [&animation] () noexcept { animation.play(); },
                            [&animation] () noexcept { animation.pause(); },
                            animation.isPlaying());

  window.addOverlaySeparator();

  window.addOverlayFrameTime("Frame time: %.3f ms/frame"); // Frame time's & FPS counter's texts must be formatted
  window.addOverlayFpsCounter("FPS: %.1f");
#endif

  //////////////////////////
  // Starting application //
  //////////////////////////

  app.run();

  return EXIT_SUCCESS;
}
