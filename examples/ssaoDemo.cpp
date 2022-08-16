#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

int main() {
  try {
    Raz::Application app;
    Raz::World& world = app.addWorld(2);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");

    Raz::Window& window = render.getWindow();

    Raz::Entity& camera = world.addEntity();
    auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 3.f, 5.f), Raz::Quaternionf(-15_deg, Raz::Axis::X));
    auto& cameraComp    = camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());

    DemoUtils::setupCameraControls(camera, window);

    Raz::Entity& mesh = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(), Raz::Quaternionf(90_deg, Raz::Axis::Y), Raz::Vec3f(0.02f));
    mesh.addComponent<Raz::MeshRenderer>(Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/crytek_sponza.obj").second);

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL,
                                                                            Raz::Vec3f(0.f, -1.f, -1.f),
                                                                            2.f,
                                                                            Raz::ColorPreset::White);

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    //////////
    // SSAO //
    //////////

    Raz::RenderGraph& renderGraph = render.getRenderGraph();
    Raz::RenderPass& geometryPass = renderGraph.getGeometryPass();

    const auto depthBuffer  = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::DEPTH);
    const auto normalBuffer = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB);

#if !defined(USE_OPENGL_ES)
    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, depthBuffer->getIndex(), "SSAO depth buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, normalBuffer->getIndex(), "SSAO normal buffer");
    }
#endif

    geometryPass.setWriteDepthTexture(depthBuffer);
    geometryPass.addWriteColorTexture(normalBuffer, 1);

    // SSAO

    auto& ssao = renderGraph.addRenderProcess<Raz::SsaoRenderProcess>();
    ssao.addParent(geometryPass);
    ssao.setInputDepthBuffer(depthBuffer);
    ssao.setInputNormalBuffer(normalBuffer);

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
