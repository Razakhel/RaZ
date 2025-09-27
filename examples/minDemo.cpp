#include "RaZ/RaZ.hpp"

int main() {
  try {
    Raz::Application app;
    Raz::World& world = app.addWorld(3);

    auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");

    Raz::Window& window = render.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    Raz::Entity& camera = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));
    camera.addComponent<Raz::Camera>(render.getSceneWidth(), render.getSceneHeight());

    Raz::Entity& mesh = world.addEntityWithComponent<Raz::Transform>();
    mesh.addComponent<Raz::MeshRenderer>(Raz::MeshFormat::load(RAZ_ROOT "assets/meshes/ball.obj").second);

    Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
    light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                   Raz::Axis::Forward,          // Direction
                                   1.f,                         // Energy
                                   Raz::ColorPreset::White);    // Color

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
