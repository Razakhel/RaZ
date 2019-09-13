#include "RaZ/RaZ.hpp"

void loadSponzaScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.setGeometryProgram(Raz::ShaderProgram(Raz::VertexShader("../../shaders/vert.glsl"),
                                               Raz::FragmentShader("../../shaders/blinn-phong.glsl")));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import("../../assets/meshes/crytek_sponza.obj");
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -1.f, 0.f);
  meshTrans.setRotation(Raz::Mat4f::identity());
  meshTrans.setScale(0.01f);
}

void loadBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.setGeometryProgram(Raz::ShaderProgram(Raz::VertexShader("../../shaders/vert.glsl"),
                                               Raz::FragmentShader("../../shaders/cook-torrance.glsl")));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import("../../assets/meshes/ball.obj");
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Mat4f::identity());
  meshTrans.setScale(1.f);
}

void loadShieldScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.setGeometryProgram(Raz::ShaderProgram(Raz::VertexShader("../../shaders/vert.glsl"),
                                               Raz::FragmentShader("../../shaders/cook-torrance.glsl")));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import("../../assets/meshes/shield.obj");
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(180.0_deg, Raz::Axis::Y);
  meshTrans.setScale(0.15f);
}

void loadCerberusScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.setGeometryProgram(Raz::ShaderProgram(Raz::VertexShader("../../shaders/vert.glsl"),
                                               Raz::FragmentShader("../../shaders/cook-torrance.glsl")));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import("../../assets/meshes/cerberus.obj");
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, 0.f, 0.f);
  meshTrans.setRotation(Raz::Mat4f::identity());
  meshTrans.setScale(2.5f);
}

#if defined(FBX_ENABLED)
void loadShaderBallScene(Raz::Entity& mesh, Raz::RenderSystem& render) {
  render.setGeometryProgram(Raz::ShaderProgram(Raz::VertexShader("../../shaders/vert.glsl"),
                                               Raz::FragmentShader("../../shaders/blinn-phong.glsl")));
  render.updateLights();
  render.sendCameraMatrices();

  auto& meshComp = mesh.getComponent<Raz::Mesh>();
  meshComp.import("../../assets/meshes/shaderBall.fbx");
  meshComp.load(render.getGeometryProgram());

  auto& meshTrans = mesh.getComponent<Raz::Transform>();
  meshTrans.setPosition(0.f, -2.f, 5.f);
  meshTrans.setRotation(180.0_deg, Raz::Axis::Y);
  meshTrans.setScale(0.015f);
}
#endif

void loadCloudsSkybox(Raz::RenderSystem& render) {
  render.setCubemap(Raz::Cubemap::create("../../assets/skyboxes/clouds_right.png",
                                         "../../assets/skyboxes/clouds_left.png",
                                         "../../assets/skyboxes/clouds_top.png",
                                         "../../assets/skyboxes/clouds_bottom.png",
                                         "../../assets/skyboxes/clouds_front.png",
                                         "../../assets/skyboxes/clouds_back.png"));
}

void loadLakeSkybox(Raz::RenderSystem& render) {
  render.setCubemap(Raz::Cubemap::create("../../assets/skyboxes/lake_right.png",
                                         "../../assets/skyboxes/lake_left.png",
                                         "../../assets/skyboxes/lake_top.png",
                                         "../../assets/skyboxes/lake_bottom.png",
                                         "../../assets/skyboxes/lake_front.png",
                                         "../../assets/skyboxes/lake_back.png"));
}

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(Raz::World(1));

  auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ - Showcase");

  Raz::Window& window = render.getWindow();
  window.enableVerticalSync();

  auto& light = world.addEntityWithComponent<Raz::Transform>();
  light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL,     // Type
                                 Raz::Vec3f({ 0.f, -0.2f, 1.f }), // Direction
                                 1.f,                             // Energy
                                 Raz::Vec3f(1.f));                // Color (RGB)

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

  while (app.run())
    meshTrans.rotate(45.0_deg * app.getDeltaTime(), Raz::Axis::Y);

  return EXIT_SUCCESS;
}
