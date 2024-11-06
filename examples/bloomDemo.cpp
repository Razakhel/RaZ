#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(7);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");

    Raz::Window& window = render.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    Raz::Entity& camera = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));
    camera.addComponent<Raz::Camera>(render.getSceneWidth(), render.getSceneHeight());

    DemoUtils::setupCameraControls(camera, window);

    Raz::Entity& mesh  = world.addEntity();
    auto& meshTrans    = mesh.addComponent<Raz::Transform>();
    auto& meshRenderer = mesh.addComponent<Raz::MeshRenderer>(Raz::MeshFormat::load(RAZ_ROOT "assets/meshes/ball.obj").second);

    auto& whiteLight = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, -Raz::Axis::Z,
                                                                                               3.f, Raz::ColorPreset::White);
    auto& yellowLight = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(5.f, 0.f, -1.f),
                                                                                                1.f, Raz::ColorPreset::Yellow);
    auto& purpleLight = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(-5.f, 0.f, -1.f),
                                                                                                1.f, Raz::ColorPreset::Magenta);
    auto& cyanLight = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, 5.f, -1.f),
                                                                                              1.f, Raz::ColorPreset::Cyan);
    auto& redLight = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, -5.f, -1.f),
                                                                                             1.f, Raz::ColorPreset::Red);

    ///////////
    // Bloom //
    ///////////

    Raz::RenderGraph& renderGraph = render.getRenderGraph();
    Raz::RenderPass& geometryPass = renderGraph.getGeometryPass();

    const auto depthBuffer = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::DEPTH);
    const auto colorBuffer = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(),
                                                    Raz::TextureColorspace::RGB, Raz::TextureDataType::FLOAT16);

#if !defined(USE_OPENGL_ES)
    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, depthBuffer->getIndex(), "Depth buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, colorBuffer->getIndex(), "Color buffer");
    }
#endif

    geometryPass.setWriteDepthTexture(depthBuffer);
    geometryPass.addWriteColorTexture(colorBuffer, 0);

    // Bloom

    auto& bloom = renderGraph.addRenderProcess<Raz::BloomRenderProcess>();
    bloom.addParent(geometryPass);
    bloom.setInputColorBuffer(colorBuffer);

    /////////////
    // Overlay //
    /////////////

    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Bloom demo", Raz::Vec2f(-1.f));

    DemoUtils::insertOverlayCameraControlsHelp(overlay);

    overlay.addSeparator();

    DemoUtils::insertOverlayVerticalSyncOption(window, overlay);

    overlay.addSeparator();

    overlay.addSlider("Emissive strength", [&meshRenderer] (float value) {
      Raz::RenderShaderProgram& matProgram = meshRenderer.getMaterials().front().getProgram();
      matProgram.setAttribute(Raz::Vec3f(value), Raz::MaterialAttribute::Emissive);
      matProgram.sendAttributes();
    }, 0.f, 10.f, 0.f);

    overlay.addSlider("White light energy", [&whiteLight, &render] (float value) noexcept {
      whiteLight.setEnergy(value);
      render.updateLights();
    }, 0.f, 15.f, whiteLight.getEnergy());

    overlay.addSlider("Yellow light energy", [&yellowLight, &render] (float value) noexcept {
      yellowLight.setEnergy(value);
      render.updateLights();
    }, 0.f, 15.f, yellowLight.getEnergy());

    overlay.addSlider("Purple light energy", [&purpleLight, &render] (float value) noexcept {
      purpleLight.setEnergy(value);
      render.updateLights();
    }, 0.f, 15.f, purpleLight.getEnergy());

    overlay.addSlider("Cyan light energy", [&cyanLight, &render] (float value) noexcept {
      cyanLight.setEnergy(value);
      render.updateLights();
    }, 0.f, 15.f, cyanLight.getEnergy());

    overlay.addSlider("Red light energy", [&redLight, &render] (float value) noexcept {
      redLight.setEnergy(value);
      render.updateLights();
    }, 0.f, 15.f, redLight.getEnergy());

    overlay.addSlider("Threshold value", [&bloom] (float value) {
      bloom.setThresholdValue(value);
    }, 0.15f, 1.f, 0.75f);

    overlay.addSeparator();

    overlay.addLabel("Thresholded color");
    overlay.addTexture(bloom.getThresholdPass().getFramebuffer().getColorBuffer(0), window.getWidth() / 7, window.getHeight() / 7);

    overlay.addSeparator();

    {
      Raz::OverlayTexture& downscaleTexture = overlay.addTexture({});
      downscaleTexture.disable();

      std::vector<std::string> downscaleEntries;
      downscaleEntries.reserve(bloom.getDownscaleBufferCount() + 1);

      downscaleEntries.emplace_back("None");
      for (std::size_t i = 1; i < bloom.getDownscaleBufferCount() + 1; ++i)
        downscaleEntries.emplace_back("Downscale pass #" + std::to_string(i));

      overlay.addDropdown("Downscale buffer", std::move(downscaleEntries), [&downscaleTexture, &bloom, &window] (const std::string&, std::size_t newIndex) noexcept {
        if (newIndex == 0) {
          downscaleTexture.disable();
          return;
        }

        downscaleTexture.enable();
        downscaleTexture.setTexture(bloom.getDownscaleBuffer(newIndex - 1),
                                    window.getWidth() / static_cast<unsigned int>(7 * newIndex),
                                    window.getHeight() / static_cast<unsigned int>(7 * newIndex));
      });
    }

    {
      Raz::OverlayTexture& upscaleTexture = overlay.addTexture({});
      upscaleTexture.disable();

      std::vector<std::string> upscaleEntries;
      upscaleEntries.reserve(bloom.getUpscaleBufferCount() + 1);

      upscaleEntries.emplace_back("None");
      for (std::size_t i = 1; i < bloom.getUpscaleBufferCount() + 1; ++i)
        upscaleEntries.emplace_back("Upscale pass #" + std::to_string(i));

      overlay.addDropdown("Upscale buffer", std::move(upscaleEntries), [&upscaleTexture, &bloom, &window] (const std::string&, std::size_t newIndex) noexcept {
        if (newIndex == 0) {
          upscaleTexture.disable();
          return;
        }

        upscaleTexture.enable();
        upscaleTexture.setTexture(bloom.getUpscaleBuffer(newIndex - 1),
                                  window.getWidth() / static_cast<unsigned int>(7 * (bloom.getUpscaleBufferCount() - newIndex + 1)),
                                  window.getHeight() / static_cast<unsigned int>(7 * (bloom.getUpscaleBufferCount() - newIndex + 1)));
      });
    }

#if !defined(USE_OPENGL_ES)
    overlay.addSeparator();

    Raz::OverlayPlot& plot = overlay.addPlot({}, 100, {}, "Time (ms)", 0.f, 100.f, false, 200.f);
    Raz::OverlayPlotEntry& geomPlot  = plot.addEntry("Geometry");
    Raz::OverlayPlotEntry& bloomPlot = plot.addEntry("Bloom");
#endif

    overlay.addSeparator();

    DemoUtils::insertOverlayFrameSpeed(overlay);

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run([&] (const Raz::FrameTimeInfo& timeInfo) {
      meshTrans.rotate(-45.0_deg * timeInfo.deltaTime, Raz::Axis::Y);

#if !defined(USE_OPENGL_ES)
      geomPlot.push(geometryPass.recoverElapsedTime());
      bloomPlot.push(bloom.recoverElapsedTime());
#endif
    });
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
