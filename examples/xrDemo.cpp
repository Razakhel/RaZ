#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

int main() {
  try {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    Raz::Application app;
    Raz::World& world = app.addWorld();

    auto& xr = world.addSystem<Raz::XrSystem>("RaZ - XR demo");

    // The dimensions given to the window are of one third of the XR device's views. This works well for e.g. the Meta Quest 2, yielding dimensions
    //   of [822; 861]. Ideally, this should be more flexible and make a window of a good enough size according to what the monitor allows
    // In an XR application, the window shouldn't be resizable:
    // - At the time of writing, resizing it will also resize the rendering viewport, breaking XR rendering (which requires fixed dimensions)
    // - As the visualization depends on what's rendered from the XR device, which has its own definition for each view, the aspect ratio should be
    //    maintained to preserve a clean, undistorted image
    auto& render = world.addSystem<Raz::RenderSystem>(xr.getOptimalViewWidth() / 3, xr.getOptimalViewHeight() / 3, "RaZ", Raz::WindowSetting::NON_RESIZABLE);

    Raz::Window& window = render.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    // Enabling XR in the render system changes the render viewport's size according to what the detected XR device expects
    render.enableXr(xr);

    // In an XR workflow, the camera is optional; its parameters aren't technically used, but its transform is
    auto& camera = world.addEntityWithComponent<Raz::Transform>();
    camera.addComponent<Raz::Camera>(render.getSceneWidth(), render.getSceneHeight());

    DemoUtils::setupCameraControls(camera, window);

    world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, -1.f, -5.f), Raz::Quaternionf(90_deg, Raz::Axis::Y), Raz::Vec3f(0.01f))
      .addComponent<Raz::MeshRenderer>(Raz::MeshFormat::load(RAZ_ROOT "assets/meshes/crytek_sponza.obj").second);

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, -Raz::Axis::Z, 1.f, Raz::ColorPreset::White);

    const auto colorBuffer = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::RGBA);
    const auto depthBuffer = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::DEPTH);

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
