#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  try {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    Raz::Application app;
    Raz::World& world = app.addWorld();

    auto& render        = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");
    Raz::Window& window = render.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    world.addSystem<Raz::XrSystem>("RaZ - XR demo");

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Camera>(window.getWidth(), window.getHeight());
    world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f))
      .addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f))));

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
