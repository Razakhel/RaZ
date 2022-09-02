#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(5);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    auto& offlineRender = world.addSystem<Raz::OfflineRenderSystem>(1280, 720, "RaZ");
    Raz::Window& window = offlineRender.getWindow();

    auto& bvh = world.addSystem<Raz::BvhSystem>();
    offlineRender.setBvh(bvh);

    Raz::Entity& camera = world.addEntity();
    auto& camTrans      = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 7.f));
    auto& camComp       = camera.addComponent<Raz::Camera>(offlineRender.getWindow().getWidth(), offlineRender.getWindow().getHeight());

    const Raz::Sphere sphere(Raz::Vec3f(0.f), 1.f);
    const Raz::Triangle triangle(Raz::Vec3f(-3.f, 1.f, 1.f), Raz::Vec3f(-1.f, 1.f, 0.f), Raz::Vec3f(-2.f, 3.f, 0.5f));
    const Raz::AABB box(Raz::Vec3f(1.f, 1.f, -1.f), Raz::Vec3f(3.f, 3.f, 0.f));

    Raz::Entity& sphereEntity   = world.addEntityWithComponent<Raz::Transform>();
    Raz::Entity& triangleEntity = world.addEntityWithComponent<Raz::Transform>();
    Raz::Entity& boxEntity      = world.addEntityWithComponent<Raz::Transform>();

    const auto& sphereMesh   = sphereEntity.addComponent<Raz::Mesh>(sphere, 20, Raz::SphereMeshType::UV);
    const auto& triangleMesh = triangleEntity.addComponent<Raz::Mesh>(triangle, Raz::Vec2f(), Raz::Vec2f(), Raz::Vec2f());
    const auto& boxMesh      = boxEntity.addComponent<Raz::Mesh>(box);

    Raz::Material& sphereMat   = sphereEntity.addComponent<Raz::MeshRenderer>(sphereMesh).getMaterials().front();
    Raz::Material& triangleMat = triangleEntity.addComponent<Raz::MeshRenderer>(triangleMesh).getMaterials().front();
    Raz::Material& boxMat      = boxEntity.addComponent<Raz::MeshRenderer>(boxMesh).getMaterials().front();

    sphereMat.setAttribute(Raz::ColorPreset::Blue, Raz::MaterialAttribute::BaseColor);
    triangleMat.setAttribute(Raz::ColorPreset::Green, Raz::MaterialAttribute::BaseColor);
    boxMat.setAttribute(Raz::ColorPreset::Red, Raz::MaterialAttribute::BaseColor);

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, 0.f, -1.f), 1.f, Raz::Vec3f(1.f));

    offlineRender.getWindow().addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    offlineRender.getWindow().setCloseCallback([&app] () noexcept { app.quit(); });

    /////////////////////
    // Camera controls //
    /////////////////////

    float cameraSpeed = 1.f;
    window.addKeyCallback(Raz::Keyboard::LEFT_SHIFT,
                          [&cameraSpeed] (float /* deltaTime */) noexcept { cameraSpeed = 2.f; },
                          Raz::Input::ONCE,
                          [&cameraSpeed] () noexcept { cameraSpeed = 1.f; });
    window.addKeyCallback(Raz::Keyboard::SPACE, [&camTrans, &cameraSpeed] (float deltaTime) {
      camTrans.move(0.f, (10.f * deltaTime) * cameraSpeed, 0.f);
    });
    window.addKeyCallback(Raz::Keyboard::V, [&camTrans, &cameraSpeed] (float deltaTime) {
      camTrans.move(0.f, (-10.f * deltaTime) * cameraSpeed, 0.f);
    });
    window.addKeyCallback(Raz::Keyboard::W, [&camTrans, &camComp, &cameraSpeed] (float deltaTime) {
      const float moveVal = (-10.f * deltaTime) * cameraSpeed;

      camTrans.move(0.f, 0.f, moveVal);
      camComp.setOrthoBoundX(camComp.getOrthoBoundX() + moveVal);
      camComp.setOrthoBoundY(camComp.getOrthoBoundY() + moveVal);
    });
    window.addKeyCallback(Raz::Keyboard::S, [&camTrans, &camComp, &cameraSpeed] (float deltaTime) {
      const float moveVal = (10.f * deltaTime) * cameraSpeed;

      camTrans.move(0.f, 0.f, moveVal);
      camComp.setOrthoBoundX(camComp.getOrthoBoundX() + moveVal);
      camComp.setOrthoBoundY(camComp.getOrthoBoundY() + moveVal);
    });
    window.addKeyCallback(Raz::Keyboard::A, [&camTrans, &cameraSpeed] (float deltaTime) {
      camTrans.move((-10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
    });
    window.addKeyCallback(Raz::Keyboard::D, [&camTrans, &cameraSpeed] (float deltaTime) {
      camTrans.move((10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
    });

    window.setMouseScrollCallback([&camComp] (double /* xOffset */, double yOffset) {
      const float newFovDeg = std::clamp(Raz::Degreesf(camComp.getFieldOfView()).value + static_cast<float>(-yOffset) * 2.f, 15.f, 90.f);
      camComp.setFieldOfView(Raz::Degreesf(newFovDeg));
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

    offlineRender.getWindow().setMouseMoveCallback([&] (double xMove, double yMove) {
      if (!isRightClicking)
        return;

      // Dividing movement by the window's size to scale between -1 and 1
      camTrans.rotate(-90_deg * static_cast<float>(yMove) / window.getHeight(),
                      -90_deg * static_cast<float>(xMove) / window.getWidth());
    });

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run();

    offlineRender.saveToImage("offlineTest.png");
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
