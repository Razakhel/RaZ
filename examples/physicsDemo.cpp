#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  try {
    Raz::Application app;
    Raz::World& world = app.addWorld(10);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");

    Raz::Window& window = render.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    Raz::Entity& camera = world.addEntity();
    auto& cameraComp    = camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());
    auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 10.f, 25.f));
    cameraComp.setCameraType(Raz::CameraType::LOOK_AT);

    auto& physics = world.addSystem<Raz::PhysicsSystem>();

    {
      const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/ball.obj");

      Raz::Entity& ball1 = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(-3.f, 3.f, 0.f));
      ball1.addComponent<Raz::MeshRenderer>(meshRenderer.clone()).load(mesh);
      ball1.addComponent<Raz::RigidBody>(3.f, 0.75f);
      // The collider's position is always relative to the object
      // We will set a position to 0 in all the following colliders, since we want their shape centered on the object
      ball1.addComponent<Raz::Collider>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

      Raz::Entity& ball2 = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(-3.f, 0.f, 3.f));
      ball2.addComponent<Raz::MeshRenderer>(meshRenderer.clone()).load(mesh);
      ball2.addComponent<Raz::RigidBody>(3.f, 0.75f);
      ball2.addComponent<Raz::Collider>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

      Raz::Entity& ball3 = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, -3.f));
      ball3.addComponent<Raz::MeshRenderer>(meshRenderer.clone()).load(mesh);
      ball3.addComponent<Raz::RigidBody>(3.f, 0.75f);
      ball3.addComponent<Raz::Collider>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

      Raz::Entity& ball4 = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(3.f, -3.f, 1.f));
      ball4.addComponent<Raz::MeshRenderer>(meshRenderer.clone()).load(mesh);
      ball4.addComponent<Raz::RigidBody>(3.f, 0.75f);
      ball4.addComponent<Raz::Collider>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));

      Raz::Entity& ball5 = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(3.f, 1.f, 0.f));
      ball5.addComponent<Raz::MeshRenderer>(meshRenderer.clone()).load(mesh);
      ball5.addComponent<Raz::RigidBody>(3.f, 0.75f);
      ball5.addComponent<Raz::Collider>(Raz::Sphere(Raz::Vec3f(0.f), 1.f));
    }

    Raz::Entity& floor = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, -5.f, 0.f));
    auto& floorMesh    = floor.addComponent<Raz::Mesh>(Raz::Plane(0.f), 10.f, 10.f);
    floor.addComponent<Raz::MeshRenderer>(floorMesh);
    floor.addComponent<Raz::Collider>(Raz::Plane(0.f));

    Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
    light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, -1.f, -1.f), 1.f, Raz::Vec3f(1.f));

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
