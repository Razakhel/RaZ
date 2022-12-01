#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(10);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    auto& renderSystem = world.addSystem<Raz::RenderSystem>(1280u, 720u, "RaZ", Raz::WindowSetting::DEFAULT, 2);
    renderSystem.setCubemap(Raz::Cubemap(Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_right.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_left.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_top.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_bottom.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_front.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_back.png")));

    Raz::Window& window = renderSystem.getWindow();
    window.setIcon(Raz::ImageFormat::load(RAZ_ROOT "assets/icons/RaZ_logo_128.png"));

    window.addKeyCallback(Raz::Keyboard::F5, [&renderSystem] (float /* deltaTime */) { renderSystem.updateShaders(); });

    window.addKeyCallback(Raz::Keyboard::F11, [&window] (float) noexcept {
      static bool isFullScreen = false;

      if (isFullScreen)
        window.makeWindowed();
      else
        window.makeFullscreen();

      isFullScreen = !isFullScreen;
    }, Raz::Input::ONCE);

    // Allowing to quit the application by pressing the Escape key
    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    // Quitting the application when the close button is clicked
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    ///////////////
    // Blur pass //
    ///////////////

    Raz::RenderGraph& renderGraph = renderSystem.getRenderGraph();
    Raz::RenderPass& geometryPass = renderSystem.getGeometryPass();

    const auto depthBuffer = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::DEPTH);
    const auto colorBuffer = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB);

#if !defined(USE_OPENGL_ES)
    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, depthBuffer->getIndex(), "Depth buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, colorBuffer->getIndex(), "Color buffer");
    }
#endif

    geometryPass.setWriteDepthTexture(depthBuffer); // A depth buffer is always needed
    geometryPass.addWriteColorTexture(colorBuffer, 0);

    // Blur

    auto& boxBlur = renderGraph.addRenderProcess<Raz::BoxBlurRenderProcess>();
    boxBlur.setInputBuffer(colorBuffer);
    boxBlur.addParent(geometryPass);

    ////////////
    // Camera //
    ////////////

    Raz::Entity& camera = world.addEntity();
    auto& cameraComp    = camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());
    auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));

    DemoUtils::setupCameraControls(camera, window);

    //////////
    // Mesh //
    //////////

    Raz::Entity& mesh = world.addEntity();

    auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/shield.obj");
    auto& meshComp       = mesh.addComponent<Raz::Mesh>(std::move(meshData)); // Useful if you need to keep the geometry
    auto& meshRenderComp = mesh.addComponent<Raz::MeshRenderer>(std::move(meshRenderData)); // Useful if you need to render the mesh

    auto& meshTrans = mesh.addComponent<Raz::Transform>();
    meshTrans.scale(0.2f);

    window.addKeyCallback(Raz::Keyboard::R, [&meshRenderComp] (float /* deltaTime */) noexcept { meshRenderComp.disable(); },
                          Raz::Input::ONCE,
                          [&meshRenderComp] () noexcept { meshRenderComp.enable(); });

    DemoUtils::setupMeshControls(mesh, window);

    ///////////
    // Light //
    ///////////

    Raz::Entity& light = world.addEntity();
    /*auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::POINT,    // Type
                                                     1.f,                      // Energy
                                                     Raz::ColorPreset::White); // Color*/
    auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                                     -Raz::Axis::Z,               // Direction
                                                     1.f,                         // Energy
                                                     Raz::ColorPreset::White);    // Color
    auto& lightTrans = light.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 0.f));

    DemoUtils::setupLightControls(light, renderSystem, window);
    DemoUtils::setupAddLight(cameraTrans, world, window);

    ///////////
    // Audio //
    ///////////

    auto& audio = world.addSystem<Raz::AudioSystem>();

    auto& meshSound = mesh.addComponent<Raz::Sound>(Raz::WavFormat::load(RAZ_ROOT "assets/sounds/wave_seagulls.wav"));
    meshSound.repeat(true);
    meshSound.setGain(0.f);
    meshSound.play();

    // Adding a Listener component, so that the sound's volume decays over the camera's distance to the source
    // Passing its initial position and/or orientation is optional, since they will be sent every time the camera moves,
    //  but recommended so that the sound instantly takes that information into account
    camera.addComponent<Raz::Listener>(cameraTrans.getPosition(), Raz::Mat3f(cameraTrans.computeTransformMatrix()));

    DemoUtils::setupSoundControls(meshSound, window);
    DemoUtils::setupAddSound(cameraTrans, RAZ_ROOT "assets/sounds/wave_seagulls.wav", world, window);

    /////////////
    // Overlay //
    /////////////

#if !defined(RAZ_NO_OVERLAY)
    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Full demo", Raz::Vec2f(window.getWidth() / 4, window.getHeight()));

    DemoUtils::insertOverlayCameraControlsHelp(overlay);
    overlay.addLabel("Press F11 to toggle fullscreen.");

    overlay.addSeparator();

    DemoUtils::insertOverlayCullingOption(window, overlay);
    DemoUtils::insertOverlayVerticalSyncOption(window, overlay);

    overlay.addSeparator();

    overlay.addSlider("Sound volume", [&meshSound] (float value) noexcept { meshSound.setGain(value); }, 0.f, 1.f, 0.f);

    overlay.addSlider("Blur strength",
                      [&boxBlur] (float value) { boxBlur.setStrength(static_cast<unsigned int>(value)); },
                      1.f, 16.f, 1.f);

    overlay.addSeparator();

    overlay.addTexture(static_cast<const Raz::Texture2D&>(meshRenderComp.getMaterials().front().getProgram().getTexture(0)), 256, 256);

    overlay.addSeparator();

    DemoUtils::insertOverlayFrameSpeed(overlay);
#endif

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
