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
                                       Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.frag"s));

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

  ///////////////
  // Blur pass //
  ///////////////

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  Raz::RenderGraph& renderGraph = renderSystem.getRenderGraph();

  const Raz::Texture& depthBuffer = renderGraph.addTextureBuffer(window.getWidth(), window.getHeight(), Raz::ImageColorspace::DEPTH);
  const Raz::Texture& colorBuffer = renderGraph.addTextureBuffer(window.getWidth(), window.getHeight(), Raz::ImageColorspace::RGBA);
  geometryPass.addWriteTexture(depthBuffer); // A depth buffer is always needed
  geometryPass.addWriteTexture(colorBuffer);

  Raz::RenderPass& blurPass = renderGraph.addNode(Raz::FragmentShader(RAZ_ROOT + "shaders/blur.frag"s));
  blurPass.addReadTexture(colorBuffer, "uniBuffer");
  blurPass.getProgram().sendUniform("uniKernelSize", 1); // Neutralizing the blur at first

  geometryPass.addChildren(blurPass);
#endif

  ///////////////////
  // Camera entity //
  ///////////////////

  Raz::Entity& camera = world.addEntity();
  auto& cameraComp    = camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());
  auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));

  /////////////////
  // Mesh entity //
  /////////////////

  Raz::Entity& mesh = world.addEntity();

  auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT + "assets/meshes/shield.obj"s);
  auto& meshComp       = mesh.addComponent<Raz::Mesh>(std::move(meshData));
  auto& meshRenderComp = mesh.addComponent<Raz::MeshRenderer>(std::move(meshRenderData));

  auto& meshTrans = mesh.addComponent<Raz::Transform>();
  meshTrans.scale(0.2f);
  meshTrans.rotate(180_deg, Raz::Axis::Y);

  window.addKeyCallback(Raz::Keyboard::R, [&mesh] (float /* deltaTime */) noexcept { mesh.disable(); },
                        Raz::Input::ONCE,
                        [&mesh] () noexcept { mesh.enable(); });

  ///////////
  // Audio //
  ///////////

  auto& audio = world.addSystem<Raz::AudioSystem>();

  auto& meshSound = mesh.addComponent<Raz::Sound>(Raz::WavFormat::load(RAZ_ROOT + "assets/sounds/wave_seagulls.wav"s));
  meshSound.repeat(true);
  meshSound.play();

  // Adding a Listener component, so that the sound's volume decays over the camera's distance to the source
  // Passing its initial position and/or orientation is optional, since they will be sent every time the camera moves,
  //  but recommended so that the sound instantly takes that information into account
  camera.addComponent<Raz::Listener>(cameraTrans.getPosition(), Raz::Mat3f(cameraTrans.computeTransformMatrix()));

  //////////////////
  // Light entity //
  //////////////////

  Raz::Entity& light = world.addEntity();
  /*auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::POINT, // Type
                                                   1.f,                   // Energy
                                                   Raz::Vec3f(1.f));      // Color(RGB)*/
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

  // Toggling play/pause
  window.addKeyCallback(Raz::Keyboard::NUM0, [&meshSound] (float /* deltaTime */) {
    if (meshSound.isPlaying()) {
      meshSound.pause();
    } else {
      meshSound.play();
      std::cout << "Sound time: " << meshSound.recoverElapsedTime() << " minutes" << std::endl;
    }
  }, Raz::Input::ONCE);

  // Stopping the sound
  window.addKeyCallback(Raz::Keyboard::DECIMAL, [&meshSound] (float /* deltaTime */) noexcept { meshSound.stop(); }, Raz::Input::ONCE);

  // Adding a new sound on the camera's position
  window.addKeyCallback(Raz::Keyboard::ADD, [&world, &cameraTrans] (float /* deltaTime */) {
    Raz::Entity& newSound = world.addEntityWithComponent<Raz::Sound>(Raz::WavFormat::load(RAZ_ROOT + "assets/sounds/wave_seagulls.wav"s));
    newSound.addComponent<Raz::Transform>(cameraTrans.getPosition());
  }, Raz::Input::ONCE);

  ////////////////////
  // Light controls //
  ////////////////////

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
  Raz::OverlayWindow& overlay = window.addOverlayWindow("RaZ - Full demo");

  overlay.addCheckbox("Enable face culling",
                      [&window] () { window.enableFaceCulling(); },
                      [&window] () { window.disableFaceCulling(); },
                      true);

#if !defined(USE_OPENGL_ES)
  overlay.addCheckbox("Enable vertical sync",
                      [&window] () { window.enableVerticalSync(); },
                      [&window] () { window.disableVerticalSync(); },
                      window.recoverVerticalSyncState());
#endif

  overlay.addSeparator();

  overlay.addSlider("Sound volume", [&meshSound] (float value) noexcept { meshSound.setGain(value); }, 0.f, 1.f, 1.f);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  overlay.addSlider("Blur strength",
                          [&blurPass] (float value) { blurPass.getProgram().sendUniform("uniKernelSize", static_cast<int>(value)); },
                          1.f, 16.f, 1.f);
#endif

  overlay.addSeparator();

  overlay.addTexture(*meshRenderComp.getMaterials().front()->getBaseColorMap(), 256, 256);

  overlay.addSeparator();

  overlay.addFrameTime("Frame time: %.3f ms/frame"); // Frame time's & FPS counter's texts must be formatted
  overlay.addFpsCounter("FPS: %.1f");
#endif

  //////////////////////////
  // Starting application //
  //////////////////////////

  app.run();

  return EXIT_SUCCESS;
}
