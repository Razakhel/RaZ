#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::Window window(800, 600, "RaZ");
  window.disableVerticalSync();

  Raz::Framebuffer framebuffer(window.getWidth(), window.getHeight());

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/blinn-phong.glsl");

  Raz::Scene scene(vertShader, fragShader);

  const auto startTime = std::chrono::system_clock::now();
  Raz::ModelPtr model = Raz::ModelLoader::importModel("../assets/meshes/crytek_sponza.obj");
  const auto endTime = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  model->scale(0.075f);

  Raz::LightPtr light = std::make_unique<Raz::PointLight>(Raz::Vec3f({ 0.f, 1.f, 0.f }),  // Position
                                                          Raz::Vec3f({ 1.f, 1.f, 1.f })); // Color (R/G/B)

  Raz::CameraPtr camera = std::make_unique<Raz::Camera>(window.getWidth(),
                                                        window.getHeight(),
                                                        45.f,                              // Field of view
                                                        0.1f, 100.f,                       // Near plane, far plane
                                                        Raz::Vec3f({ 50.f, 1.f, -5.f }));  // Initial position

  const auto modelPtr = model.get();
  const auto lightPtr = light.get();
  const auto cameraPtr = camera.get();

  // Allow wireframe toggling
  bool isWireframe = false;
  window.addKeyCallback(Raz::Keyboard::Z, [&isWireframe] () {
    glPolygonMode(GL_FRONT_AND_BACK, ((isWireframe = !isWireframe) ? GL_LINE : GL_FILL));
  });

  // Allow framebuffer toggling
  bool renderFramebuffer = false;
  window.addKeyCallback(Raz::Keyboard::B, [&renderFramebuffer] () {
    renderFramebuffer = !renderFramebuffer;
    std::cout << "Framebuffer " << (renderFramebuffer ? "ON" : "OFF") << std::endl;
  });

  // Camera controls
  window.addKeyCallback(Raz::Keyboard::SPACE, [&cameraPtr] () { cameraPtr->translate(0.f, 0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::V, [&cameraPtr] () { cameraPtr->translate(0.f, -0.5f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::W, [&cameraPtr] () { cameraPtr->translate(0.f, 0.f, 0.5f); });
  window.addKeyCallback(Raz::Keyboard::S, [&cameraPtr] () { cameraPtr->translate(0.f, 0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::A, [&cameraPtr] () { cameraPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::D, [&cameraPtr] () { cameraPtr->translate(0.5f, 0.f, 0.f); });

  // Mesh controls
  window.addKeyCallback(Raz::Keyboard::T, [&modelPtr] () { modelPtr->translate(0.f, 0.f, 0.5f); });
  window.addKeyCallback(Raz::Keyboard::G, [&modelPtr] () { modelPtr->translate(0.f, 0.f, -0.5f); });
  window.addKeyCallback(Raz::Keyboard::F, [&modelPtr] () { modelPtr->translate(-0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::H, [&modelPtr] () { modelPtr->translate(0.5f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::X, [&modelPtr] () { modelPtr->scale(0.5f); });
  window.addKeyCallback(Raz::Keyboard::C, [&modelPtr] () { modelPtr->scale(2.f); });
  window.addKeyCallback(Raz::Keyboard::UP, [&modelPtr] () { modelPtr->rotate(10.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::DOWN, [&modelPtr] () { modelPtr->rotate(-10.f, 1.f, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::LEFT, [&modelPtr] () { modelPtr->rotate(-10.f, 0.f, 1.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&modelPtr] () { modelPtr->rotate(10.f, 0.f, 1.f, 0.f); });

  // Light controls
  window.addKeyCallback(Raz::Keyboard::I, [&lightPtr, &scene] () { lightPtr->translate(0.f, 0.f, 0.5f); scene.updateLights(); });
  window.addKeyCallback(Raz::Keyboard::K, [&lightPtr, &scene] () { lightPtr->translate(0.f, 0.f, -0.5f); scene.updateLights(); });
  window.addKeyCallback(Raz::Keyboard::J, [&lightPtr, &scene] () { lightPtr->translate(-0.5f, 0.f, 0.f); scene.updateLights(); });
  window.addKeyCallback(Raz::Keyboard::L, [&lightPtr, &scene] () { lightPtr->translate(0.5f, 0.f, 0.f); scene.updateLights(); });

  scene.addModel(std::move(model));
  scene.addLight(std::move(light));
  scene.updateLights();
  //scene.setCamera(std::move(camera));

  const int uniProjectionLocation = framebuffer.getProgram().recoverUniformLocation("uniProjectionMatrix");
  const int uniInvProjLocation = framebuffer.getProgram().recoverUniformLocation("uniInvProjMatrix");
  const int uniViewLocation = framebuffer.getProgram().recoverUniformLocation("uniViewMatrix");
  const int uniInvViewLocation = framebuffer.getProgram().recoverUniformLocation("uniInvViewMatrix");

  const int uniViewProjLocation = scene.getProgram().recoverUniformLocation("uniViewProjMatrix");

  framebuffer.getProgram().use();
  framebuffer.getProgram().sendUniform("uniSceneDepthBuffer", 0);
  framebuffer.getProgram().sendUniform("uniSceneColorBuffer", 1);
  framebuffer.getProgram().sendUniform("uniSceneNormalBuffer", 2);

  auto lastTime = std::chrono::system_clock::now();
  uint16_t nbFrames = 0;

  while (window.run()) {
    const auto currentTime = std::chrono::system_clock::now();
    ++nbFrames;

    if (std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime).count() >= 1.f) {
      std::cout << nbFrames << " FPS\r" << std::flush;

      nbFrames = 0;
      lastTime = currentTime;
    }

    const Raz::Mat4f projectionMat = cameraPtr->computePerspectiveMatrix();
    const Raz::Mat4f viewMat = cameraPtr->lookAt(modelPtr->getPosition());
    const Raz::Mat4f viewProjMat = projectionMat * viewMat;

    if (renderFramebuffer) {
      framebuffer.bind();

      framebuffer.getProgram().sendUniform(uniProjectionLocation, projectionMat);
      framebuffer.getProgram().sendUniform(uniInvProjLocation, projectionMat.inverse());
      framebuffer.getProgram().sendUniform(uniViewLocation, viewMat);
      framebuffer.getProgram().sendUniform(uniInvViewLocation, viewMat.inverse());

      scene.render(viewProjMat);
      framebuffer.unbind();

      glActiveTexture(GL_TEXTURE0);
      framebuffer.getDepthBuffer()->bind();
      glActiveTexture(GL_TEXTURE1);
      framebuffer.getColorBuffer()->bind();
      glActiveTexture(GL_TEXTURE2);
      framebuffer.getNormalBuffer()->bind();

      framebuffer.getProgram().use();
      framebuffer.display();
    } else {
      scene.getProgram().sendUniform(uniViewProjLocation, viewProjMat);

      scene.render(viewProjMat);
    }
  }

  return EXIT_SUCCESS;
}
