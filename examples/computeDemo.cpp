#include "RaZ/RaZ.hpp"

using namespace std::literals;

static constexpr int textureSize = 512;

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(1);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    auto& render = world.addSystem<Raz::RenderSystem>(textureSize, textureSize + 85, "RaZ");

    if (!Raz::Renderer::checkVersion(4, 3)) {
      throw std::runtime_error("Error: Compute is only available with an OpenGL 4.3 context or above; "
                               "please update your graphics drivers or try on another computer");
    }

    Raz::RenderShaderProgram& geometryProgram = render.getGeometryProgram();

    geometryProgram.setShaders(Raz::VertexShader(RAZ_ROOT "shaders/common.vert"),
                               Raz::FragmentShader(RAZ_ROOT "shaders/cook-torrance.frag"));

    // A camera is needed by the RenderSystem, but since we won't render anything there is no need to initialize it
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Camera>();

    Raz::Window& window = world.getSystem<Raz::RenderSystem>().getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    /////////////////////
    // Compute program //
    /////////////////////

    Raz::Texture texture(textureSize, textureSize, 0, Raz::ImageColorspace::GRAY, Raz::ImageDataType::FLOAT);
    Raz::Renderer::bindImageTexture(0, texture.getIndex(), 0, false, 0, Raz::ImageAccess::WRITE, Raz::ImageInternalFormat::R16F);

    Raz::ComputeShaderProgram compProgram(Raz::ComputeShader(RAZ_ROOT "shaders/perlin_noise.comp"));
    compProgram.execute(textureSize, textureSize);

    /////////////
    // Overlay //
    /////////////

    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Compute demo", Raz::Vec2f(window.getWidth(), window.getHeight()));

    overlay.addTexture(texture, textureSize, textureSize);

    overlay.addSlider("Noise map factor", [&compProgram] (float value) {
      compProgram.use();
      compProgram.sendUniform("uniNoiseFactor", value);
      compProgram.execute(textureSize, textureSize);
    }, 0.001f, 0.1f, 0.01f);

    overlay.addSlider("Octave count", [&compProgram] (float value) {
      compProgram.use();
      compProgram.sendUniform("uniOctaveCount", static_cast<int>(value));
      compProgram.execute(textureSize, textureSize);
    }, 1, 8, 8);

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
