#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

int main() {
  try {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    Raz::Application app;
    Raz::World& world = app.addWorld();

    // Dimensions of [2468, 2584] (definition of each view of the Meta Quest 2) divided by 3
    auto& render = world.addSystem<Raz::RenderSystem>(823, 861, "RaZ", Raz::WindowSetting::NON_RESIZABLE);

    Raz::Window& window = render.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    auto& xr = world.addSystem<Raz::XrSystem>("RaZ - XR demo");
    render.enableXr(xr);

    // In an XR workflow, the camera is optional; its parameters aren't technically used, but its transform is
    auto& camera = world.addEntityWithComponent<Raz::Transform>();
    camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());

    DemoUtils::setupCameraControls(camera, window);

    world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, -1.f, -5.f), Raz::Quaternionf(90_deg, Raz::Axis::Y), Raz::Vec3f(0.01f))
      .addComponent<Raz::MeshRenderer>(Raz::MeshFormat::load(RAZ_ROOT "assets/meshes/crytek_sponza.obj").second);

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, -Raz::Axis::Z, 1.f, Raz::ColorPreset::White);

    // TODO: the textures' dimensions must be the same as the rendering viewport's
    const auto colorBuffer = Raz::Texture2D::create(2468, 2584, Raz::TextureColorspace::RGBA);
    const auto depthBuffer = Raz::Texture2D::create(2468, 2584, Raz::TextureColorspace::DEPTH);

    // The last executed pass *MUST* have a write color buffer, and at least the geometry pass *MUST* have a write depth buffer
    Raz::RenderPass& geomPass = render.getGeometryPass();
    geomPass.addWriteColorTexture(colorBuffer, 0);
    geomPass.setWriteDepthTexture(depthBuffer);

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
