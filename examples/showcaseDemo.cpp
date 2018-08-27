#include "RaZ/RaZ.hpp"

void loadSponzaScene(Raz::Application& app) {
  Raz::ScenePtr scene = Raz::Scene::create(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                           Raz::FragmentShader::create("../../shaders/blinn-phong.glsl"));
  scene->addModel(Raz::Model::import("../../assets/meshes/crytek_sponza.obj"));
  scene->load();
  app.setScene(std::move(scene));

  app.getScene()->getModels().front()->scale(0.01f);
  app.getScene()->getModels().front()->translate(0.f, -1.f, 0.f);

  app.getScene()->addLight(Raz::DirectionalLight::create(Raz::Vec3f({ 0.f, -1.f, 1.f }),
                                                         0.75f,
                                                         Raz::Vec3f({ 1.f, 1.f, 1.f })));
  app.getScene()->updateLights();
}

void loadBallScene(Raz::Application& app) {
  Raz::ScenePtr scene = Raz::Scene::create(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                           Raz::FragmentShader::create("../../shaders/cook-torrance.glsl"));
  scene->addModel(Raz::Model::import("../../assets/meshes/ball.obj"));
  scene->load();
  app.setScene(std::move(scene));

  app.getScene()->addLight(Raz::DirectionalLight::create(Raz::Vec3f({ 0.f, 0.f, 1.f }),
                                                         0.75f,
                                                         Raz::Vec3f({ 1.f, 1.f, 1.f })));
  app.getScene()->updateLights();
}

void loadShieldScene(Raz::Application& app) {
  Raz::ScenePtr scene = Raz::Scene::create(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                           Raz::FragmentShader::create("../../shaders/cook-torrance.glsl"));
  scene->addModel(Raz::Model::import("../../assets/meshes/shield.obj"));
  scene->load();
  app.setScene(std::move(scene));

  app.getScene()->getModels().front()->scale(0.15f);
  app.getScene()->getModels().front()->rotate(180.f, Raz::Axis::Y);

  app.getScene()->addLight(Raz::DirectionalLight::create(Raz::Vec3f({ 0.f, 0.f, 1.f }),
                                                         1.f,
                                                         Raz::Vec3f({ 1.f, 1.f, 1.f })));
  app.getScene()->updateLights();
}

void loadCerberusScene(Raz::Application& app) {
  Raz::ScenePtr scene = Raz::Scene::create(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                           Raz::FragmentShader::create("../../shaders/cook-torrance.glsl"));
  scene->addModel(Raz::Model::import("../../assets/meshes/cerberus.obj"));
  scene->load();
  app.setScene(std::move(scene));

  app.getScene()->getModels().front()->scale(2.5f);

  app.getScene()->addLight(Raz::DirectionalLight::create(Raz::Vec3f({ 0.f, 0.f, 1.f }),
                                                         0.5f,
                                                         Raz::Vec3f({ 1.f, 1.f, 1.f })));
  app.getScene()->updateLights();
}

#if defined(FBX_ENABLED)
void loadShaderBallScene(Raz::Application& app) {
  Raz::ScenePtr scene = Raz::Scene::create(Raz::VertexShader::create("../../shaders/vert.glsl"),
                                           Raz::FragmentShader::create("../../shaders/blinn-phong.glsl"));
  scene->addModel(Raz::Model::import("../../assets/meshes/shaderBall.fbx"));
  scene->load();
  app.setScene(std::move(scene));

  app.getScene()->getModels().front()->translate(0.f, -2.f, 5.f);
  app.getScene()->getModels().front()->scale(0.015f);
  app.getScene()->getModels().front()->rotate(180.f, Raz::Axis::Y);

  app.getScene()->addLight(Raz::DirectionalLight::create(Raz::Vec3f({ 0.f, 0.f, 1.f }),
                                                         0.75f,
                                                         Raz::Vec3f({ 1.f, 1.f, 1.f })));
  app.getScene()->updateLights();
}
#endif

void loadCloudsSkybox(Raz::Application& app) {
  app.getScene()->setCubemap(Raz::Cubemap::create("../../assets/skyboxes/clouds_right.png",
                                                  "../../assets/skyboxes/clouds_left.png",
                                                  "../../assets/skyboxes/clouds_top.png",
                                                  "../../assets/skyboxes/clouds_bottom.png",
                                                  "../../assets/skyboxes/clouds_front.png",
                                                  "../../assets/skyboxes/clouds_back.png"));
}

void loadLakeSkybox(Raz::Application& app) {
  app.getScene()->setCubemap(Raz::Cubemap::create("../../assets/skyboxes/lake_right.png",
                                                  "../../assets/skyboxes/lake_left.png",
                                                  "../../assets/skyboxes/lake_top.png",
                                                  "../../assets/skyboxes/lake_bottom.png",
                                                  "../../assets/skyboxes/lake_front.png",
                                                  "../../assets/skyboxes/lake_back.png"));
}

int main() {
  Raz::WindowPtr window = Raz::Window::create(1280, 720, "RaZ - Showcase", 4);
  window->enableVerticalSync();
  auto windowPtr = window.get();

  Raz::Application app(std::move(window), Raz::Camera::create(windowPtr->getWidth(), windowPtr->getHeight(),
                                                              45.f,
                                                              0.1f, 100.f,
                                                              Raz::Vec3f({ 0.f, 0.f, -5.f })));

  auto& cameraPtr = app.getCamera();

  loadBallScene(app);

  windowPtr->enableOverlay();
  windowPtr->addOverlayButton("Load Sponza scene", [&app] () { loadSponzaScene(app); });
  windowPtr->addOverlayButton("Load Ball scene", [&app] () { loadBallScene(app); });
  windowPtr->addOverlayButton("Load Shield scene", [&app] () { loadShieldScene(app); });
  windowPtr->addOverlayButton("Load Cerberus scene", [&app] () { loadCerberusScene(app); });
#if defined(FBX_ENABLED)
  windowPtr->addOverlayButton("Load Shader ball scene", [&app] () { loadShaderBallScene(app); });
#endif
  windowPtr->addOverlaySeparator();
  windowPtr->addOverlayButton("Remove skybox", [&app] () { app.getScene()->setCubemap(nullptr); });
  windowPtr->addOverlayButton("Load Clouds skybox", [&app] () { loadCloudsSkybox(app); });
  windowPtr->addOverlayButton("Load Lake skybox", [&app] () { loadLakeSkybox(app); });
  windowPtr->addOverlaySeparator();
  windowPtr->addOverlayFpsCounter("FPS: %.1f");

  windowPtr->addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) { app.quit(); });
  windowPtr->addMouseScrollCallback([&cameraPtr] (double /* xOffset */, double yOffset) {
    cameraPtr->translate(0.f, 0.f, 0.5f * static_cast<float>(yOffset));
  });

  while (app.run())
    app.getScene()->getModels().front()->rotate(45.f * windowPtr->getDeltaTime(), Raz::Axis::Y);

  return EXIT_SUCCESS;
}
