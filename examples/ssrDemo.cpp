#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(12);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    auto& render = world.addSystem<Raz::RenderSystem>(1280, 720, "RaZ");

    Raz::Window& window = render.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    Raz::Entity& camera = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.25f, 10.f));
    camera.addComponent<Raz::Camera>(render.getSceneWidth(), render.getSceneHeight());

    DemoUtils::setupCameraControls(camera, window);

    ///////////
    // Scene //
    ///////////

    const auto checkerboard = Raz::Texture2D::create(Raz::ImageFormat::load(RAZ_ROOT "assets/textures/checkerboard.png"));
    const auto normalMap    = Raz::Texture2D::create(Raz::ImageFormat::load(RAZ_ROOT "assets/textures/rustediron_normal.png"));
    const Raz::Mesh planeMesh(Raz::Plane(0.f), 10.f, 10.f);

    auto& floor        = world.addEntityWithComponent<Raz::Transform>();
    auto& floorProgram = floor.addComponent<Raz::MeshRenderer>(planeMesh).getMaterials().front().getProgram();
    floorProgram.setTexture(checkerboard, Raz::MaterialTexture::Roughness);
    floorProgram.setTexture(normalMap, Raz::MaterialTexture::Normal);

    auto& backWall        = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 10.f, -10.f), Raz::Quaternionf(90_deg, Raz::Axis::X));
    auto& backWallProgram = backWall.addComponent<Raz::MeshRenderer>(planeMesh).getMaterials().front().getProgram();
    backWallProgram.setAttribute(Raz::ColorPreset::Cyan, Raz::MaterialAttribute::BaseColor);
    backWallProgram.setTexture(checkerboard, Raz::MaterialTexture::BaseColor);
    backWallProgram.setTexture(checkerboard, Raz::MaterialTexture::Roughness);
    backWallProgram.setTexture(normalMap, Raz::MaterialTexture::Normal);

    auto& leftWall        = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(-10.f, 10.f, 0.f), Raz::Quaternionf(-90_deg, Raz::Axis::Z));
    auto& leftWallProgram = leftWall.addComponent<Raz::MeshRenderer>(planeMesh).getMaterials().front().getProgram();
    leftWallProgram.setAttribute(Raz::ColorPreset::Magenta, Raz::MaterialAttribute::BaseColor);
    leftWallProgram.setTexture(checkerboard, Raz::MaterialTexture::BaseColor);
    leftWallProgram.setTexture(checkerboard, Raz::MaterialTexture::Roughness);
    leftWallProgram.setTexture(normalMap, Raz::MaterialTexture::Normal);

    auto& rightWall        = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(10.f, 10.f, 0.f), Raz::Quaternionf(90_deg, Raz::Axis::Z));
    auto& rightWallProgram = rightWall.addComponent<Raz::MeshRenderer>(planeMesh).getMaterials().front().getProgram();
    rightWallProgram.setAttribute(Raz::ColorPreset::Yellow, Raz::MaterialAttribute::BaseColor);
    rightWallProgram.setTexture(checkerboard, Raz::MaterialTexture::BaseColor);
    rightWallProgram.setTexture(checkerboard, Raz::MaterialTexture::Roughness);
    rightWallProgram.setTexture(normalMap, Raz::MaterialTexture::Normal);

    auto& redBox      = world.addEntity();
    auto& redBoxTrans = redBox.addComponent<Raz::Transform>(Raz::Vec3f(-3.f, 1.75f, 0.f));
    auto& redBoxMat   = redBox.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)))).getMaterials().front();
    redBoxMat.getProgram().setAttribute(Raz::ColorPreset::Red, Raz::MaterialAttribute::BaseColor);
    redBoxMat.getProgram().setAttribute(0.f, Raz::MaterialAttribute::Roughness);

    auto& greenBall      = world.addEntity();
    auto& greenBallTrans = greenBall.addComponent<Raz::Transform>();
    auto& greenBallMat   = greenBall.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Sphere({}, 1.f), 30.f, Raz::SphereMeshType::UV)).getMaterials().front();
    greenBallMat.getProgram().setAttribute(Raz::ColorPreset::Green, Raz::MaterialAttribute::BaseColor);
    greenBallMat.getProgram().setAttribute(0.f, Raz::MaterialAttribute::Roughness);

    auto& blueBall      = world.addEntity();
    auto& blueBallTrans = blueBall.addComponent<Raz::Transform>();
    auto& blueBallMat   = blueBall.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Sphere({}, 1.f), 30.f, Raz::SphereMeshType::UV)).getMaterials().front();
    blueBallMat.getProgram().setAttribute(Raz::ColorPreset::Blue, Raz::MaterialAttribute::BaseColor);
    blueBallMat.getProgram().setAttribute(0.f, Raz::MaterialAttribute::Roughness);

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::Down, 1.f);
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::Forward, 1.f);
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::Right, 1.f);
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::Left, 1.f);

    /////////
    // SSR //
    /////////

    Raz::RenderGraph& renderGraph = render.getRenderGraph();
    Raz::RenderPass& geometryPass = renderGraph.getGeometryPass();

    const auto depthBuffer        = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::DEPTH);
    const auto colorBuffer        = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto blurredColorBuffer = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto normalBuffer       = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto specularBuffer     = Raz::Texture2D::create(render.getSceneWidth(), render.getSceneHeight(), Raz::TextureColorspace::RGBA);

#if !defined(USE_OPENGL_ES)
    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, depthBuffer->getIndex(), "SSR depth buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, colorBuffer->getIndex(), "SSR color buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, blurredColorBuffer->getIndex(), "SSR blurred color buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, normalBuffer->getIndex(), "SSR normal buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, specularBuffer->getIndex(), "SSR specular buffer");
    }
#endif

    geometryPass.setWriteDepthTexture(depthBuffer);
    geometryPass.addWriteColorTexture(colorBuffer, 0);
    geometryPass.addWriteColorTexture(normalBuffer, 1);
    geometryPass.addWriteColorTexture(specularBuffer, 2);

    // Blur

    auto& blur = renderGraph.addRenderProcess<Raz::BoxBlurRenderProcess>();
    blur.addParent(geometryPass);
    blur.setInputBuffer(colorBuffer);
    blur.setOutputBuffer(blurredColorBuffer);
    blur.setStrength(16);

    // Screen space reflections

    auto& ssr = renderGraph.addRenderProcess<Raz::ScreenSpaceReflectionsRenderProcess>();
    ssr.addParent(blur);
    ssr.setInputDepthBuffer(depthBuffer);
    ssr.setInputColorBuffer(colorBuffer);
    ssr.setInputBlurredColorBuffer(blurredColorBuffer);
    ssr.setInputNormalBuffer(normalBuffer);
    ssr.setInputSpecularBuffer(specularBuffer);

    /////////////
    // Overlay //
    /////////////

    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - SSR demo", Raz::Vec2f(static_cast<float>(window.getWidth()) / 5.8f, window.getHeight()));

    overlay.addCheckbox("Metallic walls", [&backWallProgram, &leftWallProgram, &rightWallProgram] () {
      backWallProgram.setAttribute(1.f, Raz::MaterialAttribute::Metallic);
      backWallProgram.sendAttributes();
      leftWallProgram.setAttribute(1.f, Raz::MaterialAttribute::Metallic);
      leftWallProgram.sendAttributes();
      rightWallProgram.setAttribute(1.f, Raz::MaterialAttribute::Metallic);
      rightWallProgram.sendAttributes();
    }, [&backWallProgram, &leftWallProgram, &rightWallProgram] () {
      backWallProgram.setAttribute(0.f, Raz::MaterialAttribute::Metallic);
      backWallProgram.sendAttributes();
      leftWallProgram.setAttribute(0.f, Raz::MaterialAttribute::Metallic);
      leftWallProgram.sendAttributes();
      rightWallProgram.setAttribute(0.f, Raz::MaterialAttribute::Metallic);
      rightWallProgram.sendAttributes();
    }, false);

    overlay.addSeparator();

    overlay.addLabel("Depth buffer");
    overlay.addTexture(*depthBuffer, static_cast<unsigned int>(window.getWidth() / 6.45), static_cast<unsigned int>(window.getHeight() / 6.45));

    overlay.addLabel("Color buffer");
    overlay.addTexture(*colorBuffer, static_cast<unsigned int>(window.getWidth() / 6.45), static_cast<unsigned int>(window.getHeight() / 6.45));

    overlay.addLabel("Blurred color buffer");
    overlay.addTexture(*blurredColorBuffer, static_cast<unsigned int>(window.getWidth() / 6.45), static_cast<unsigned int>(window.getHeight() / 6.45));

    overlay.addLabel("Normal buffer");
    overlay.addTexture(*normalBuffer, static_cast<unsigned int>(window.getWidth() / 6.45), static_cast<unsigned int>(window.getHeight() / 6.45));

    overlay.addLabel("Specular buffer");
    overlay.addTexture(*specularBuffer, static_cast<unsigned int>(window.getWidth() / 6.45), static_cast<unsigned int>(window.getHeight() / 6.45));

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run([&] (const Raz::FrameTimeInfo& timeInfo) {
      const float sinTime = std::sin(timeInfo.globalTime);

      redBoxTrans.rotate(Raz::Quaternionf(90_deg * timeInfo.deltaTime, Raz::Vec3f(0.f, 0.707106769f, 0.707106769f)));
      greenBallTrans.setPosition(0.f, 2.f + sinTime, 0.f);
      blueBallTrans.setPosition(2.f - sinTime, 1.f, 0.f);
    });
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
