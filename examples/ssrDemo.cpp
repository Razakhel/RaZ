#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

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
    camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());

    DemoUtils::setupCameraControls(camera, window);

    const auto checkerboard = Raz::Texture2D::create(Raz::ImageFormat::load(RAZ_ROOT "assets/textures/checkerboard.png"));
    const auto normalMap    = Raz::Texture2D::create(Raz::ImageFormat::load(RAZ_ROOT "assets/textures/rustediron_normal.png"));

    auto& floor        = world.addEntityWithComponent<Raz::Transform>();
    auto& floorProgram = floor.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Plane(0.f), 10.f, 10.f)).getMaterials().front().getProgram();
    floorProgram.setTexture(checkerboard, Raz::MaterialTexture::Roughness);
    floorProgram.setTexture(normalMap, Raz::MaterialTexture::Normal);

    auto& backWall        = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 10.f, -10.f), Raz::Quaternionf(90_deg, Raz::Axis::X));
    auto& backWallProgram = backWall.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Plane(0.f), 10.f, 10.f)).getMaterials().front().getProgram();
    backWallProgram.setAttribute(Raz::ColorPreset::Cyan, Raz::MaterialAttribute::BaseColor);
    backWallProgram.setTexture(checkerboard, Raz::MaterialTexture::BaseColor);
    backWallProgram.setTexture(checkerboard, Raz::MaterialTexture::Roughness);
    backWallProgram.setTexture(normalMap, Raz::MaterialTexture::Normal);

    auto& leftWall        = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(-10.f, 10.f, 0.f), Raz::Quaternionf(-90_deg, Raz::Axis::Z));
    auto& leftWallProgram = leftWall.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Plane(0.f), 10.f, 10.f)).getMaterials().front().getProgram();
    leftWallProgram.setAttribute(Raz::ColorPreset::Magenta, Raz::MaterialAttribute::BaseColor);
    leftWallProgram.setTexture(checkerboard, Raz::MaterialTexture::BaseColor);
    leftWallProgram.setTexture(checkerboard, Raz::MaterialTexture::Roughness);
    leftWallProgram.setTexture(normalMap, Raz::MaterialTexture::Normal);

    auto& rightWall        = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(10.f, 10.f, 0.f), Raz::Quaternionf(90_deg, Raz::Axis::Z));
    auto& rightWallProgram = rightWall.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Plane(0.f), 10.f, 10.f)).getMaterials().front().getProgram();
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
    auto& greenBallTrans = greenBall.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 0.f));
    auto& greenBallMat   = greenBall.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Sphere({}, 1.f), 30.f, Raz::SphereMeshType::UV)).getMaterials().front();
    greenBallMat.getProgram().setAttribute(Raz::ColorPreset::Green, Raz::MaterialAttribute::BaseColor);
    greenBallMat.getProgram().setAttribute(0.f, Raz::MaterialAttribute::Roughness);

    auto& blueBall      = world.addEntity();
    auto& blueBallTrans = blueBall.addComponent<Raz::Transform>(Raz::Vec3f(3.f, 1.f, 0.f));
    auto& blueBallMat   = blueBall.addComponent<Raz::MeshRenderer>(Raz::Mesh(Raz::Sphere({}, 1.f), 30.f, Raz::SphereMeshType::UV)).getMaterials().front();
    blueBallMat.getProgram().setAttribute(Raz::ColorPreset::Blue, Raz::MaterialAttribute::BaseColor);
    blueBallMat.getProgram().setAttribute(0.f, Raz::MaterialAttribute::Roughness);

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, -Raz::Axis::Y, 1.f);
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, -Raz::Axis::Z, 1.f);
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL,  Raz::Axis::X, 1.f);
    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, -Raz::Axis::X, 1.f);

    /////////
    // SSR //
    /////////

    Raz::RenderGraph& renderGraph = render.getRenderGraph();
    Raz::RenderPass& geometryPass = renderGraph.getGeometryPass();

    const auto depthBuffer    = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::DEPTH);
    const auto colorBuffer    = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB);
    const auto normalBuffer   = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB);
    const auto specularBuffer = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGBA);

#if !defined(USE_OPENGL_ES)
    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, depthBuffer->getIndex(), "SSR depth buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, colorBuffer->getIndex(), "SSR color buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, normalBuffer->getIndex(), "SSR normal buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, specularBuffer->getIndex(), "SSR specular buffer");
    }
#endif

    geometryPass.setWriteDepthTexture(depthBuffer);
    geometryPass.addWriteColorTexture(colorBuffer, 0);
    geometryPass.addWriteColorTexture(normalBuffer, 1);
    geometryPass.addWriteColorTexture(specularBuffer, 2);

    // SSR

    auto& ssr = renderGraph.addRenderProcess<Raz::SsrRenderProcess>();
    ssr.addParent(geometryPass);
    ssr.setInputDepthBuffer(depthBuffer);
    ssr.setInputColorBuffer(colorBuffer);
    ssr.setInputNormalBuffer(normalBuffer);
    ssr.setInputSpecularBuffer(specularBuffer);

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run([&] (float deltaTime) {
      static float totalTime = 0.f;

      const float sinTime = std::sin(totalTime);

      redBoxTrans.rotate(Raz::Quaternionf(90_deg * deltaTime, Raz::Vec3f(0.f, 0.707106769f, 0.707106769f)));
      greenBallTrans.translate(0.f, sinTime * deltaTime, 0.f);
      blueBallTrans.translate(-sinTime * deltaTime, 0.f, 0.f);

      totalTime += deltaTime;
    });
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
