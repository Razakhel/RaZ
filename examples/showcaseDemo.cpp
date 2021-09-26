#include "RaZ/RaZ.hpp"

using namespace std::literals;

inline void loadSponzaScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/blinn-phong.frag"s));
  render.updateLights();

  auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT + "assets/meshes/crytek_sponza.obj"s);
  mesh.getComponent<Raz::Mesh>() = std::move(meshData);
  auto& meshRenderComp = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderComp = std::move(meshRenderData);
  meshRenderComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -1.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(0.01f);
}

inline void loadBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.frag"s));
  render.updateLights();

  auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT + "assets/meshes/ball.obj"s);
  mesh.getComponent<Raz::Mesh>() = std::move(meshData);
  auto& meshRenderComp = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderComp = std::move(meshRenderData);
  meshRenderComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(1.f);
}

inline void loadShieldScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.frag"s));
  render.updateLights();

  auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT + "assets/meshes/shield.obj"s);
  mesh.getComponent<Raz::Mesh>() = std::move(meshData);
  auto& meshRenderComp = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderComp = std::move(meshRenderData);
  meshRenderComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(180.0_deg, Raz::Axis::Y);
  meshTrans.setScale(0.15f);
}

inline void loadCerberusScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.frag"s));
  render.updateLights();

  auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT + "assets/meshes/cerberus.obj"s);
  mesh.getComponent<Raz::Mesh>() = std::move(meshData);
  auto& meshRenderComp = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderComp = std::move(meshRenderData);
  meshRenderComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(2.5f);
}

#if defined(FBX_ENABLED)
inline void loadShaderBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/blinn-phong.frag"s));
  render.updateLights();

  auto [meshData, meshRenderData] = Raz::FbxFormat::load(RAZ_ROOT + "assets/meshes/shaderBall.fbx"s);
  mesh.getComponent<Raz::Mesh>() = std::move(meshData);
  auto& meshRenderComp = mesh.getComponent<Raz::MeshRenderer>();
  meshRenderComp = std::move(meshRenderData);
  meshRenderComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -2.f, 5.f);
  meshTrans.setRotation(180.0_deg, Raz::Axis::Y);
  meshTrans.setScale(0.015f);
}
#endif

inline void loadCloudsSkybox(Raz::RenderSystem& render) {
  render.setCubemap(Raz::Cubemap(RAZ_ROOT + "assets/skyboxes/clouds_right.png"s,
                                 RAZ_ROOT + "assets/skyboxes/clouds_left.png"s,
                                 RAZ_ROOT + "assets/skyboxes/clouds_top.png"s,
                                 RAZ_ROOT + "assets/skyboxes/clouds_bottom.png"s,
                                 RAZ_ROOT + "assets/skyboxes/clouds_front.png"s,
                                 RAZ_ROOT + "assets/skyboxes/clouds_back.png"s));
}

inline void loadLakeSkybox(Raz::RenderSystem& render) {
  render.setCubemap(Raz::Cubemap(RAZ_ROOT + "assets/skyboxes/lake_right.png"s,
                                 RAZ_ROOT + "assets/skyboxes/lake_left.png"s,
                                 RAZ_ROOT + "assets/skyboxes/lake_top.png"s,
                                 RAZ_ROOT + "assets/skyboxes/lake_bottom.png"s,
                                 RAZ_ROOT + "assets/skyboxes/lake_front.png"s,
                                 RAZ_ROOT + "assets/skyboxes/lake_back.png"s));
}

int main() {
  ////////////////////
  // Initialization //
  ////////////////////

  Raz::Application app;
  Raz::World& world = app.addWorld(1);

  ///////////////
  // Rendering //
  ///////////////

  auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ - Showcase");

  Raz::Window& window = render.getWindow();
  window.enableVerticalSync();

  /////////////////
  // Mesh entity //
  /////////////////

  Raz::Entity& mesh = world.addEntity();
  auto& meshTrans   = mesh.addComponent<Raz::Transform>();
  mesh.addComponent<Raz::Mesh>();
  mesh.addComponent<Raz::MeshRenderer>();

  Raz::Entity& camera = world.addEntity();
  auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));
  camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());

  //////////////////
  // Light entity //
  //////////////////

  Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                 Raz::Vec3f(0.f, -0.2f, 1.f), // Direction
                                 1.f,                         // Energy
                                 Raz::Vec3f(1.f));            // Color (RGB)

  /////////////
  // Overlay //
  /////////////

#if !defined(RAZ_NO_OVERLAY)
  Raz::OverlayWindow& overlay = window.addOverlayWindow("RaZ - Showcase demo");

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

  window.addMouseScrollCallback([&cameraTrans] (double /* xOffset */, double yOffset) {
    cameraTrans.translate(0.f, 0.f, 0.5f * static_cast<float>(yOffset));
  });

  window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
  window.setCloseCallback([&app] () noexcept { app.quit(); });

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

  app.run([&] {
    meshTrans.rotate(-45.0_deg * app.getDeltaTime(), Raz::Axis::Y);
  });

  return EXIT_SUCCESS;
}
