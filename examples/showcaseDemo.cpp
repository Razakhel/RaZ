#include "RaZ/RaZ.hpp"

using namespace std::literals;

inline void loadSponzaScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.enableGeometryPass(Raz::VertexShader(RAZ_ROOT + "shaders/vert.glsl"s), Raz::FragmentShader(RAZ_ROOT + "shaders/blinn-phong.glsl"s));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/crytek_sponza.obj"s);
  meshComp.load(render.getGeometryPass().getProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -1.f, 0.f);
  meshTrans.setRotation(Raz::Mat4f::identity());
  meshTrans.setScale(0.01f);
}

inline void loadBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.enableGeometryPass(Raz::VertexShader(RAZ_ROOT + "shaders/vert.glsl"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.glsl"s));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/ball.obj"s);
  meshComp.load(render.getGeometryPass().getProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Mat4f::identity());
  meshTrans.setScale(1.f);
}

inline void loadShieldScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.enableGeometryPass(Raz::VertexShader(RAZ_ROOT + "shaders/vert.glsl"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.glsl"s));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/shield.obj"s);
  meshComp.load(render.getGeometryPass().getProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(180.0_deg, Raz::Axis::Y);
  meshTrans.setScale(0.15f);
}

inline void loadCerberusScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.enableGeometryPass(Raz::VertexShader(RAZ_ROOT + "shaders/vert.glsl"s), Raz::FragmentShader(RAZ_ROOT + "shaders/cook-torrance.glsl"s));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/cerberus.obj"s);
  meshComp.load(render.getGeometryPass().getProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Mat4f::identity());
  meshTrans.setScale(2.5f);
}

#if defined(FBX_ENABLED)
inline void loadShaderBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.enableGeometryPass(Raz::VertexShader(RAZ_ROOT + "shaders/vert.glsl"s), Raz::FragmentShader(RAZ_ROOT + "shaders/blinn-phong.glsl"s));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import(RAZ_ROOT + "assets/meshes/shaderBall.fbx"s);
  meshComp.load(render.getGeometryPass().getProgram());

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
  Raz::Application app;
  Raz::World& world = app.addWorld(1);

  auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ - Showcase");

  Raz::Window& window = render.getWindow();
  window.enableVerticalSync();

  auto& light = world.addEntityWithComponent<Raz::Transform>();
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                 Raz::Vec3f(0.f, -0.2f, 1.f), // Direction
                                 1.f,                         // Energy
                                 Raz::Vec3f(1.f));            // Color (RGB)

  auto& mesh      = world.addEntity();
  auto& meshTrans = mesh.addComponent<Raz::Transform>();
  mesh.addComponent<Raz::Mesh>();

  auto& cameraTrans = render.getCameraEntity().getComponent<Raz::Transform>();
  cameraTrans.translate(0.f, 0.f, -5.f);

  loadBallScene(mesh, render);

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

  window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });
  window.addMouseScrollCallback([&cameraTrans] (double /* xOffset */, double yOffset) {
    cameraTrans.translate(0.f, 0.f, 0.5f * static_cast<float>(yOffset));
  });

  app.run([&] {
    meshTrans.rotate(45.0_deg * app.getDeltaTime(), Raz::Axis::Y);
  });

  return EXIT_SUCCESS;
}
