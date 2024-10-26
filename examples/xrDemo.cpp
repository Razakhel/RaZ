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

    auto& xr = world.addSystem<Raz::XrSystem>("RaZ - XR demo");
    render.enableXr(xr);

    // The camera parameters aren't technically used for XR rendering, but its transform may be later
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Camera>(window.getWidth(), window.getHeight());

    world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, -1.f, -5.f), Raz::Quaternionf(90_deg, Raz::Axis::Y), Raz::Vec3f(0.01f))
      .addComponent<Raz::MeshRenderer>(Raz::MeshFormat::load(RAZ_ROOT "assets/meshes/crytek_sponza.obj").second);

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, -Raz::Axis::Z, 1.f, Raz::ColorPreset::White);

    // TODO: the textures' dimensions must be the same as the rendering viewport's
    const auto colorBuffer = Raz::Texture2D::create(2468, 2584, Raz::TextureColorspace::RGBA);
    const auto depthBuffer = Raz::Texture2D::create(2468, 2584, Raz::TextureColorspace::DEPTH);

    Raz::RenderPass& geomPass = render.getGeometryPass();
    geomPass.addWriteColorTexture(colorBuffer, 0);
    geomPass.setWriteDepthTexture(depthBuffer);

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
