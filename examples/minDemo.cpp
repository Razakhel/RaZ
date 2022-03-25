#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  try {
    Raz::Application app;
    Raz::World& world = app.addWorld(2);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");

    Raz::Entity& camera = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));
    camera.addComponent<Raz::Camera>(render.getWindow().getWidth(), render.getWindow().getHeight());

    Raz::Entity& mesh = world.addEntityWithComponent<Raz::Transform>();

    auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/ball.obj");
    mesh.addComponent<Raz::Mesh>(std::move(meshData));
    mesh.addComponent<Raz::MeshRenderer>(std::move(meshRenderData));

    Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
    light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                   Raz::Vec3f(0.f, 0.f, -1.f),  // Direction
                                   1.f,                         // Energy
                                   Raz::Vec3f(1.f));            // Color (R/G/B)

    render.getWindow().addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    render.getWindow().setCloseCallback([&app] () noexcept { app.quit(); });

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
