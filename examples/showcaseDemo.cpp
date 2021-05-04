#include "RaZ/RaZ.hpp"

using namespace std::literals;

inline void loadSponzaScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/blinn-phong.frag"s));
  render.updateLights();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/crytek_sponza.obj"s);
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -1.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(0.01f);
}

inline void loadBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.frag"s));
  render.updateLights();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/ball.obj"s);
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(1.f);
}

inline void loadShieldScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.frag"s));
  render.updateLights();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/shield.obj"s);
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(180.0_deg, Raz::Axis::Y);
  meshTrans.setScale(0.15f);
}

inline void loadCerberusScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.frag"s));
  render.updateLights();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/cerberus.obj"s);
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Quaternionf::identity());
  meshTrans.setScale(2.5f);
}

#if defined(FBX_ENABLED)
inline void loadShaderBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s), Raz::FragmentShader(RAZ_ROOT + "shaders/blinn-phong.frag"s));
  render.updateLights();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/shaderBall.fbx"s);
  meshComp.load(render.getGeometryProgram());

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

#if defined(RAZ_USE_OVERLAY)
  window.enableOverlay();

  window.addOverlayButton("Load Sponza scene",   [&mesh, &render] () { loadSponzaScene(mesh, render); });
  window.addOverlayButton("Load Ball scene",     [&mesh, &render] () { loadBallScene(mesh, render); });
  window.addOverlayButton("Load Shield scene",   [&mesh, &render] () { loadShieldScene(mesh, render); });
  window.addOverlayButton("Load Cerberus scene", [&mesh, &render] () { loadCerberusScene(mesh, render); });
#if defined(FBX_ENABLED)
  window.addOverlayButton("Load Shader ball scene", [&mesh, &render] () { loadShaderBallScene(mesh, render); });
#endif

  window.addOverlaySeparator();

  window.addOverlayButton("Remove skybox",      [&render] () { render.removeCubemap(); });
  window.addOverlayButton("Load Clouds skybox", [&render] () { loadCloudsSkybox(render); });
  window.addOverlayButton("Load Lake skybox",   [&render] () { loadLakeSkybox(render); });

  window.addOverlaySeparator();

  window.addOverlayFpsCounter("FPS: %.1f");
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

  window.addKeyCallback(Raz::Keyboard::R, [&render] (float /* deltaTime */) { render.removeCubemap(); }, Raz::Input::ONCE);
  window.addKeyCallback(Raz::Keyboard::O, [&render] (float /* deltaTime */) { loadCloudsSkybox(render); }, Raz::Input::ONCE);
  window.addKeyCallback(Raz::Keyboard::L, [&render] (float /* deltaTime */) { loadLakeSkybox(render); }, Raz::Input::ONCE);

  window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });
  window.addMouseScrollCallback([&cameraTrans] (double /* xOffset */, double yOffset) {
    cameraTrans.translate(0.f, 0.f, 0.5f * static_cast<float>(yOffset));
  });
  window.setCloseCallback([&app] () { app.quit(); });

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
