#include "RaZ/RaZ.hpp"

static constexpr int textureSize  = 512;
static constexpr int textureDepth = 16;

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

    auto& render = world.addSystem<Raz::RenderSystem>(textureSize, textureSize, "RaZ", Raz::WindowSetting::NON_RESIZABLE);

    if (!Raz::Renderer::checkVersion(4, 3) && !Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader")) {
      throw std::runtime_error("Error: Compute is only available with an OpenGL 4.3 context or above, or with the 'GL_ARB_compute_shader' extension; "
                               "please update your graphics drivers or try on another computer");
    }

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Camera>(textureSize, textureSize);

    Raz::Window& window = world.getSystem<Raz::RenderSystem>().getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    /////////////////////
    // Compute program //
    /////////////////////

    const auto texture = Raz::Texture3D::create(textureSize, textureSize, textureDepth, Raz::TextureColorspace::GRAY, Raz::TextureDataType::FLOAT16);

    Raz::ComputeShaderProgram perlinNoise(Raz::ComputeShader(RAZ_ROOT "shaders/perlin_noise_3d.comp"));
    perlinNoise.setImageTexture(texture, "uniNoiseMap", Raz::ImageTextureUsage::WRITE);
    perlinNoise.initImageTextures();
    perlinNoise.execute(textureSize, textureSize, textureDepth);

    Raz::ComputeShaderProgram worleyNoise(Raz::ComputeShader(RAZ_ROOT "shaders/worley_noise_3d.comp"));
    worleyNoise.setImageTexture(texture, "uniNoiseMap", Raz::ImageTextureUsage::WRITE);
    worleyNoise.initImageTextures();

    Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, texture->getIndex(), "Noise texture");
    Raz::Renderer::setLabel(Raz::RenderObjectType::PROGRAM, perlinNoise.getIndex(), "Perlin noise shader program");
    Raz::Renderer::setLabel(Raz::RenderObjectType::PROGRAM, worleyNoise.getIndex(), "Worley noise shader program");
    Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, perlinNoise.getShader().getIndex(), "Perlin noise compute shader");
    Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, worleyNoise.getShader().getIndex(), "Worley noise compute shader");

    /////////////////////
    // Display surface //
    /////////////////////

    // Creating a triangle just big enough to cover the screen
    auto& surface = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Triangle(Raz::Vec3f(-0.42f, -0.42f, -1.f),
                                                                                                                           Raz::Vec3f( 1.25f, -0.42f, -1.f),
                                                                                                                           Raz::Vec3f(-0.42f,  1.25f, -1.f)),
                                                                                                             Raz::Vec2f(0.f, 0.f),
                                                                                                             Raz::Vec2f(2.f, 0.f),
                                                                                                             Raz::Vec2f(0.f, 2.f)));

    Raz::RenderShaderProgram& surfaceProgram = surface.setMaterial(Raz::Material(Raz::MaterialType::SINGLE_TEXTURE_3D)).getProgram();
    surfaceProgram.setTexture(texture, Raz::MaterialTexture::BaseColor);

    /////////////
    // Overlay //
    /////////////

    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Compute demo", Raz::Vec2f(-1.f));

    Raz::OverlaySlider& perlinFactor = overlay.addSlider("Perlin noise factor", [&perlinNoise] (float value) {
      perlinNoise.setAttribute(value, "uniNoiseFactor");
      perlinNoise.sendAttributes();
      perlinNoise.execute(textureSize, textureSize, textureDepth);
    }, 0.001f, 0.1f, 0.01f);

    Raz::OverlaySlider& perlinOctaves = overlay.addSlider("Perlin noise octaves", [&perlinNoise] (float value) {
      perlinNoise.setAttribute(static_cast<int>(value), "uniOctaveCount");
      perlinNoise.sendAttributes();
      perlinNoise.execute(textureSize, textureSize, textureDepth);
    }, 1, 8, 1);

    Raz::OverlaySlider& worleyFactor = overlay.addSlider("Worley noise factor", [&worleyNoise] (float value) {
      worleyNoise.setAttribute(value, "uniNoiseFactor");
      worleyNoise.sendAttributes();
      worleyNoise.execute(textureSize, textureSize, textureDepth);
    }, 0.001f, 0.1f, 0.01f);
    worleyFactor.disable();

    overlay.addSlider("Depth", [&surfaceProgram] (float value) {
      surfaceProgram.setAttribute(value / textureDepth, "uniDepth");
      surfaceProgram.sendAttributes();
    }, 0.f, textureDepth, 0.f);

    overlay.addDropdown("Noise method", { "Perlin", "Worley" }, [&] (const std::string&, std::size_t index) {
      perlinFactor.disable();
      perlinOctaves.disable();

      worleyFactor.disable();

      switch (index) {
        case 0:
          perlinFactor.enable();
          perlinOctaves.enable();
          perlinNoise.execute(textureSize, textureSize, textureDepth);
          break;

        case 1:
          worleyFactor.enable();
          worleyNoise.execute(textureSize, textureSize, textureDepth);
          break;

        default:
          break;
      }
    });

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
