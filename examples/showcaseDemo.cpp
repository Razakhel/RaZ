#include "RaZ/RaZ.hpp"

#include <iostream>

using namespace std::literals;

inline void loadSponzaScene(Raz::Entity& mesh, const Raz::RenderSystem& render) {
  auto& meshRenderer = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderer       = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/crytek_sponza.obj").second;
  render.updateMaterials(meshRenderer);

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -1.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(0.01f);
}

inline void loadBallScene(Raz::Entity& mesh, const Raz::RenderSystem& render) {
  auto& meshRenderer = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderer       = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/ball.obj").second;
  render.updateMaterials(meshRenderer);

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(1.f);
}

inline void loadShieldScene(Raz::Entity& mesh, const Raz::RenderSystem& render) {
  auto& meshRenderer = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderer       = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/shield.obj").second;
  render.updateMaterials(meshRenderer);

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(0.15f);
}

inline void loadCerberusScene(Raz::Entity& mesh, const Raz::RenderSystem& render) {
  auto& meshRenderer = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderer       = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/cerberus.obj").second;
  render.updateMaterials(meshRenderer);

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(2.5f);
}

#if defined(FBX_ENABLED)
inline void loadShaderBallScene(Raz::Entity& mesh, const Raz::RenderSystem& render) {
  auto& meshRenderer = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderer       = Raz::FbxFormat::load(RAZ_ROOT "assets/meshes/shaderBall.fbx").second;
  render.updateMaterials(meshRenderer);

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -2.f, -5.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(5.f);
}
#endif

inline void loadCloudsSkybox(Raz::RenderSystem& render) {
  static const Raz::Image right  = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_right.png");
  static const Raz::Image left   = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_left.png");
  static const Raz::Image top    = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_top.png");
  static const Raz::Image bottom = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_bottom.png");
  static const Raz::Image front  = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_front.png");
  static const Raz::Image back   = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_back.png");

  render.setCubemap(Raz::Cubemap(right, left, top, bottom, front, back));
}

inline void loadLakeSkybox(Raz::RenderSystem& render) {
  static const Raz::Image right  = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/lake_right.png");
  static const Raz::Image left   = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/lake_left.png");
  static const Raz::Image top    = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/lake_top.png");
  static const Raz::Image bottom = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/lake_bottom.png");
  static const Raz::Image front  = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/lake_front.png");
  static const Raz::Image back   = Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/lake_back.png");

  render.setCubemap(Raz::Cubemap(right, left, top, bottom, front, back));
}

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(1);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ - Showcase");

    Raz::Window& window = render.getWindow();
    window.enableVerticalSync();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    /////////////////
    // Mesh entity //
    /////////////////

    Raz::Entity& mesh = world.addEntityWithComponent<Raz::MeshRenderer>();
    auto& meshTrans   = mesh.addComponent<Raz::Transform>();

    Raz::Entity& camera = world.addEntity();
    auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));
    camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());

    //////////////////
    // Light entity //
    //////////////////

    Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
    light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL,  // Type
                                   Raz::Vec3f(0.f, -0.2f, -1.f), // Direction
                                   1.f,                          // Energy
                                   Raz::ColorPreset::White);     // Color

    /////////////
    // Overlay //
    /////////////

#if !defined(RAZ_NO_OVERLAY)
    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Showcase demo", Raz::Vec2f(-1.f));

    std::vector<std::string> scenes = { "Sponza", "Ball", "Shield", "Cerberus" };
#if defined(FBX_ENABLED)
    scenes.emplace_back("Shader ball");
#endif

    overlay.addDropdown("Scene", std::move(scenes), [&mesh, &render] (const std::string&, std::size_t index) {
      switch (index) {
        case 0: loadSponzaScene(mesh, render); break;
        case 1: loadBallScene(mesh, render); break;
        case 2: loadShieldScene(mesh, render); break;
        case 3: loadCerberusScene(mesh, render); break;
#if defined(FBX_ENABLED)
        case 4: loadShaderBallScene(mesh, render); break;
#endif
        default: break;
      }
    }, 1);

    overlay.addSeparator();

    overlay.addDropdown("Skybox", { "None", "Clouds", "Lake" }, [&render] (const std::string&, std::size_t index) {
      switch (index) {
        case 0: render.removeCubemap(); break;
        case 1: loadCloudsSkybox(render); break;
        case 2: loadLakeSkybox(render); break;
        default: break;
      }
    });

    overlay.addSeparator();

    overlay.addFpsCounter("FPS: %.1f");
#endif

    ///////////////////
    // Key callbacks //
    ///////////////////

    window.addKeyCallback(Raz::Keyboard::P, [&mesh, &render] (float /* deltaTime */) { loadSponzaScene(mesh, render); }, Raz::Input::ONCE);
    window.addKeyCallback(Raz::Keyboard::B, [&mesh, &render] (float /* deltaTime */) { loadBallScene(mesh, render); }, Raz::Input::ONCE);
    window.addKeyCallback(Raz::Keyboard::S, [&mesh, &render] (float /* deltaTime */) { loadShieldScene(mesh, render); }, Raz::Input::ONCE);
    window.addKeyCallback(Raz::Keyboard::C, [&mesh, &render] (float /* deltaTime */) { loadCerberusScene(mesh, render); }, Raz::Input::ONCE);
#if defined(FBX_ENABLED)
    window.addKeyCallback(Raz::Keyboard::D, [&mesh, &render] (float /* deltaTime */) { loadShaderBallScene(mesh, render); });
#endif

    window.addKeyCallback(Raz::Keyboard::R, [&render] (float /* deltaTime */) noexcept { render.removeCubemap(); }, Raz::Input::ONCE);
    window.addKeyCallback(Raz::Keyboard::O, [&render] (float /* deltaTime */) { loadCloudsSkybox(render); }, Raz::Input::ONCE);
    window.addKeyCallback(Raz::Keyboard::L, [&render] (float /* deltaTime */) { loadLakeSkybox(render); }, Raz::Input::ONCE);

    window.setMouseScrollCallback([&cameraTrans] (double /* xOffset */, double yOffset) {
      cameraTrans.translate(0.f, 0.f, -0.5f * static_cast<float>(yOffset));
    });

    //////////////////
    // Display help //
    //////////////////

    std::cout << "#######################\n";
    std::cout << "# RaZ - Showcase demo #\n";
    std::cout << "#######################\n\n";

    std::cout << "Keyboard shortcuts:\n";
    std::cout << "\tP: Load Sponza scene\n";
    std::cout << "\tB: Load Ball scene\n";
    std::cout << "\tS: Load Shield scene\n";
    std::cout << "\tC: Load Cerberus scene\n";
#if defined(FBX_ENABLED)
    std::cout << "\tD: Load ShaderBall scene\n";
#endif
    std::cout << '\n';

    std::cout << "\tR: Remove skybox\n";
    std::cout << "\tO: Load cloud skybox\n";
    std::cout << "\tL: Load lake skybox\n\n";

    std::cout << "\tEsc: Exit the demo\n";

    //////////////////////////
    // Starting application //
    //////////////////////////

    loadBallScene(mesh, render);

    app.run([&] (float deltaTime) {
      meshTrans.rotate(-45.0_deg * deltaTime, Raz::Axis::Y);
    });
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
