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

    auto& render = world.addSystem<Raz::RenderSystem>(textureSize, textureSize + 105, "RaZ");

    if (!Raz::Renderer::checkVersion(4, 3)) {
      throw std::runtime_error("Error: Compute is only available with an OpenGL 4.3 context or above; "
                               "please update your graphics drivers or try on another computer");
    }

    // A camera is needed by the RenderSystem, but since we won't render anything there is no need to initialize it
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Camera>();

    Raz::Window& window = world.getSystem<Raz::RenderSystem>().getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    /////////////////////
    // Compute program //
    /////////////////////

    Raz::Texture texture(textureSize, textureSize, Raz::ImageColorspace::GRAY, Raz::ImageDataType::FLOAT);
    Raz::Renderer::bindImageTexture(0, texture.getIndex(), 0, false, 0, Raz::ImageAccess::WRITE, Raz::ImageInternalFormat::R16F);

    Raz::ComputeShaderProgram perlinNoise(Raz::ComputeShader(RAZ_ROOT "shaders/perlin_noise.comp"));
    perlinNoise.execute(textureSize, textureSize);

    Raz::ComputeShaderProgram worleyNoise(Raz::ComputeShader(RAZ_ROOT "shaders/worley_noise.comp"));

    Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, texture.getIndex(), "Noise texture");
    Raz::Renderer::setLabel(Raz::RenderObjectType::PROGRAM, perlinNoise.getIndex(), "Perlin noise shader program");
    Raz::Renderer::setLabel(Raz::RenderObjectType::PROGRAM, worleyNoise.getIndex(), "Worley noise shader program");
    Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, perlinNoise.getShader().getIndex(), "Perlin noise compute shader");
    Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, worleyNoise.getShader().getIndex(), "Worley noise compute shader");

    /////////////
    // Overlay //
    /////////////

    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Compute demo", Raz::Vec2f(window.getWidth(), window.getHeight()));

    overlay.addTexture(texture, textureSize, textureSize);

    Raz::OverlaySlider& perlinFactor = overlay.addSlider("Perlin noise factor", [&perlinNoise] (float value) {
      perlinNoise.use();
      perlinNoise.sendUniform("uniNoiseFactor", value);
      perlinNoise.execute(textureSize, textureSize);
    }, 0.001f, 0.1f, 0.01f);

    Raz::OverlaySlider& perlinOctave = overlay.addSlider("Perlin noise octaves", [&perlinNoise] (float value) {
      perlinNoise.use();
      perlinNoise.sendUniform("uniOctaveCount", static_cast<int>(value));
      perlinNoise.execute(textureSize, textureSize);
    }, 1, 8, 8);

    Raz::OverlaySlider& worleyFactor = overlay.addSlider("Worley noise factor", [&worleyNoise] (float value) {
      worleyNoise.use();
      worleyNoise.sendUniform("uniNoiseFactor", value);
      worleyNoise.execute(textureSize, textureSize);
    }, 0.001f, 0.1f, 0.01f);
    worleyFactor.disable();

    overlay.addDropdown("Noise method", { "Perlin", "Worley" }, [&] (const std::string&, std::size_t index) {
      perlinFactor.disable();
      perlinOctave.disable();

      worleyFactor.disable();

      switch (index) {
        case 0:
          perlinFactor.enable();
          perlinOctave.enable();
          perlinNoise.execute(textureSize, textureSize);
          break;

        case 1:
          worleyFactor.enable();
          worleyNoise.execute(textureSize, textureSize);
          break;

        default:
          break;
      }
    });

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
