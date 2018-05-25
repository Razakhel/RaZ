#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::Window window(800, 600, "RaZ", 4);
  //window.disableVerticalSync();

  Raz::VertexShaderPtr vertShader   = std::make_unique<Raz::VertexShader>("../shaders/vert.glsl");
  Raz::FragmentShaderPtr fragShader = std::make_unique<Raz::FragmentShader>("../shaders/cook-torrance.glsl");

  Raz::Scene scene(std::move(vertShader), std::move(fragShader));

  Raz::CubemapPtr cubemap = std::make_unique<Raz::Cubemap>("../assets/skyboxes/lake_right.png", "../assets/skyboxes/lake_left.png",
                                                           "../assets/skyboxes/lake_top.png", "../assets/skyboxes/lake_bottom.png",
                                                           "../assets/skyboxes/lake_front.png", "../assets/skyboxes/lake_back.png");

  const auto startTime = std::chrono::system_clock::now();
  Raz::ModelPtr model  = Raz::ModelLoader::importModel("../assets/meshes/shield.obj");
  const auto endTime   = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  model->scale(0.2f);
  model->rotate(180.f, 0.f, 1.f, 0.f);

  /*Raz::LightPtr light = std::make_unique<Raz::PointLight>(Raz::Vec3f({ 0.f, 1.f, 0.f }),  // Position
                                                          10.f,                           // Energy
                                                          Raz::Vec3f({ 1.f, 1.f, 1.f })); // Color (R/G/B)*/
  Raz::LightPtr light = std::make_unique<Raz::DirectionalLight>(Raz::Vec3f({ 0.f, 1.f, 0.f }),  // Position
                                                                Raz::Vec3f({ 0.f, 0.f, 1.f }),  // Direction
                                                                1.f,                            // Energy
                                                                Raz::Vec3f({ 1.f, 1.f, 1.f })); // Color (R/G/B)

  Raz::CameraPtr camera = std::make_unique<Raz::Camera>(window.getWidth(),
                                                        window.getHeight(),
                                                        45.f,                            // Field of view
                                                        0.1f, 100.f,                     // Near plane, far plane
                                                        Raz::Vec3f({ 0.f, 0.f, -5.f })); // Initial position

  const auto cubemapPtr = cubemap.get();
  const auto modelPtr   = model.get();
  const auto lightPtr   = light.get();
  const auto cameraPtr  = camera.get();

  // Allow wireframe toggling
  bool isWireframe = false;
  window.addKeyCallback(Raz::Keyboard::Z, [&isWireframe] () {
    glPolygonMode(GL_FRONT_AND_BACK, ((isWireframe = !isWireframe) ? GL_LINE : GL_FILL));
  });

  // Camera controls
  window.addKeyCallback(Raz::Keyboard::SPACE, [&cameraPtr] () { cameraPtr->move( 0.f,  0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::V,     [&cameraPtr] () { cameraPtr->move( 0.f, -0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::W,     [&cameraPtr] () { cameraPtr->move( 0.f,  0.f,  0.5f); });
  window.addKeyCallback(Raz::Keyboard::S,     [&cameraPtr] () { cameraPtr->move( 0.f,  0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::A,     [&cameraPtr] () { cameraPtr->move(-0.5f, 0.f,  0.f); });
  window.addKeyCallback(Raz::Keyboard::D,     [&cameraPtr] () { cameraPtr->move( 0.5f, 0.f,  0.f); });
  window.addKeyCallback(Raz::Keyboard::NUM8,  [&cameraPtr] () { cameraPtr->rotate(-10.f, 1.f, 0.f, 0.f); }); // Looking up
  window.addKeyCallback(Raz::Keyboard::NUM2,  [&cameraPtr] () { cameraPtr->rotate( 10.f, 1.f, 0.f, 0.f); }); // Looking down
  window.addKeyCallback(Raz::Keyboard::NUM4,  [&cameraPtr] () { cameraPtr->rotate(-10.f, 0.f, 1.f, 0.f); }); // Looking left
  window.addKeyCallback(Raz::Keyboard::NUM6,  [&cameraPtr] () { cameraPtr->rotate( 10.f, 0.f, 1.f, 0.f); }); // Looking right

  // Mesh controls
  window.addKeyCallback(Raz::Keyboard::T,     [&modelPtr] () { modelPtr->move( 0.f,  0.f,  0.5f); });
  window.addKeyCallback(Raz::Keyboard::G,     [&modelPtr] () { modelPtr->move( 0.f,  0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::F,     [&modelPtr] () { modelPtr->move(-0.5f, 0.f,  0.f); });
  window.addKeyCallback(Raz::Keyboard::H,     [&modelPtr] () { modelPtr->move( 0.5f, 0.f,  0.f); });
  window.addKeyCallback(Raz::Keyboard::X,     [&modelPtr] () { modelPtr->scale(0.5f); });
  window.addKeyCallback(Raz::Keyboard::C,     [&modelPtr] () { modelPtr->scale(2.f); });
  window.addKeyCallback(Raz::Keyboard::UP,    [&modelPtr] () { modelPtr->rotate(-10.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::DOWN,  [&modelPtr] () { modelPtr->rotate( 10.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::LEFT,  [&modelPtr] () { modelPtr->rotate(-10.f, 0.f, 1.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] () { modelPtr->rotate( 10.f, 0.f, 1.f, 0.f); });

  // Light controls
  window.addKeyCallback(Raz::Keyboard::I, [&lightPtr, &scene] () { lightPtr->translate( 0.f,  0.f,  0.5f); scene.updateLights(); });
  window.addKeyCallback(Raz::Keyboard::K, [&lightPtr, &scene] () { lightPtr->translate( 0.f,  0.f, -0.5f); scene.updateLights(); });
  window.addKeyCallback(Raz::Keyboard::J, [&lightPtr, &scene] () { lightPtr->translate(-0.5f, 0.f,  0.f);  scene.updateLights(); });
  window.addKeyCallback(Raz::Keyboard::L, [&lightPtr, &scene] () { lightPtr->translate( 0.5f, 0.f,  0.f);  scene.updateLights(); });

  window.addKeyCallback(Raz::Keyboard::F5, [&scene] () {
    scene.getProgram().updateShaders();
    scene.load();
    scene.updateLights();
  });

  scene.setCubemap(std::move(cubemap));
  scene.addModel(std::move(model));
  scene.addLight(std::move(light));
  scene.updateLights();
  //scene.setCamera(std::move(camera));
  scene.load();

  const Raz::Mat4f projectionMat = cameraPtr->computePerspectiveMatrix();

  cubemapPtr->getProgram().use();
  cubemapPtr->getProgram().sendUniform("uniProjectionMatrix", projectionMat);

  const auto uniCameraPosLocation = scene.getProgram().recoverUniformLocation("uniCameraPos");
  const auto uniViewProjLocation  = scene.getProgram().recoverUniformLocation("uniViewProjMatrix");

  const auto uniCubemapViewLocation = cubemapPtr->getProgram().recoverUniformLocation("uniViewMatrix");

  auto lastTime = std::chrono::system_clock::now();
  uint16_t nbFrames = 0;

  while (window.run()) {
    const auto currentTime = std::chrono::system_clock::now();
    ++nbFrames;

    if (std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime).count() >= 1.f) {
      window.setTitle("RaZ - " + std::to_string(nbFrames) + " FPS");

      nbFrames = 0;
      lastTime = currentTime;
    }

    const Raz::Mat4f viewMat     = cameraPtr->computeViewMatrix();
    const Raz::Mat4f viewProjMat = projectionMat * viewMat;

    scene.getProgram().use();
    scene.getProgram().sendUniform(uniCameraPosLocation, camera->getPosition());
    scene.getProgram().sendUniform(uniViewProjLocation, viewProjMat);

    cubemapPtr->getProgram().use();
    cubemapPtr->getProgram().sendUniform(uniCubemapViewLocation, Raz::Mat4f(Raz::Mat3f(viewMat)));

    scene.render(viewProjMat);
  }

  return EXIT_SUCCESS;
}
