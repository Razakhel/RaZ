#include "Catch.hpp"
#include "TestUtils.hpp"

#include "RaZ/World.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace {

Raz::Image renderFrame(Raz::World& world, const Raz::FilePath& renderedImgPath = {}) {
  Raz::Window& window = TestUtils::getWindow();

  // Rendering a frame of the scene by updating the World's RenderSystem & running the Window
  world.update(0.f);
  window.run(0.f);

  // Recovering the rendered frame into an image
  Raz::Image renderedImg(window.getWidth(), window.getHeight(), Raz::ImageColorspace::RGB, Raz::ImageDataType::BYTE);
  Raz::Renderer::recoverFrame(window.getWidth(), window.getHeight(), Raz::TextureFormat::RGB, Raz::PixelDataType::UBYTE, renderedImg.getDataPtr());

  if (!renderedImgPath.isEmpty())
    Raz::ImageFormat::save(renderedImgPath, renderedImg, true);

  return renderedImg;
}

} // namespace

TEST_CASE("RenderSystem accepted components") {
  Raz::World world(3);

  auto& render = world.addSystem<Raz::RenderSystem>(0, 0);

  Raz::Entity& camera       = world.addEntityWithComponents<Raz::Camera, Raz::Transform>(); // RenderSystem::update() needs a Camera with a Transform component
  Raz::Entity& meshRenderer = world.addEntityWithComponent<Raz::MeshRenderer>();
  Raz::Entity& light        = world.addEntityWithComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Axis::X, 1.f);

  world.update(0.f);

  CHECK(render.containsEntity(camera));
  CHECK(render.containsEntity(meshRenderer));
  CHECK(render.containsEntity(light));
}

TEST_CASE("RenderSystem Cook-Torrance ball") {
  Raz::World world(7);

  Raz::Window& window = TestUtils::getWindow();

  auto& renderSystem = world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  Raz::Entity& camera = world.addEntity();
  auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 3.f));
  auto& cameraComp    = camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());

  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::MeshRenderer>(Raz::ObjFormat::load(RAZ_TESTS_ROOT "../assets/meshes/ball.obj").second);

  world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 1.5f)).addComponent<Raz::Light>(Raz::LightType::POINT, 1.5f, Raz::ColorPreset::White);
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(5.f, 0.f, -1.f).normalize(),
                                                                          1.f, Raz::ColorPreset::Yellow);
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(-5.f, 0.f, -1.f).normalize(),
                                                                          1.f, Raz::ColorPreset::Magenta);
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, 5.f, -1.f).normalize(),
                                                                          1.f, Raz::ColorPreset::Cyan);
  world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, Raz::Vec3f(0.f, -5.f, -1.f).normalize(),
                                                                          1.f, Raz::ColorPreset::Red);

  CHECK_THAT(renderFrame(world), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_base.png", true)));

  // Setting a cubemap & moving the camera to look the ball from below, in order to see the left, top & back faces of the cubemap

  renderSystem.setCubemap(Raz::Cubemap(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ŔŖȒȐ.png"),
                                       Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ŔŖȒȐ.png"),
                                       Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ĜƓGǦ.png"),
                                       Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ĜƓGǦ.png"),
                                       Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/BƁḂɃ.png"),
                                       Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/BƁḂɃ.png")));

  cameraTrans.translate(Raz::Vec3f(3.f, -3.f, 0.f));
  cameraComp.setCameraType(Raz::CameraType::LOOK_AT);
  cameraComp.setTarget(Raz::Vec3f(0.f));

  CHECK_THAT(renderFrame(world), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_cubemap_base.png", true)));
}

TEST_CASE("RenderSystem overlay render") {
  Raz::World world(1);

  Raz::Window& window = TestUtils::getWindow();

  world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  // The RenderSystem needs a Camera in order to be run
  world.addEntityWithComponents<Raz::Transform, Raz::Camera>();

  {
    Raz::OverlayWindow& overlay1 = window.getOverlay().addWindow("RaZ - Overlay test 1", Raz::Vec2f(window.getWidth(), window.getHeight()));
    overlay1.addLabel("Label");
    overlay1.addColoredLabel("Label red", Raz::ColorPreset::Red);
    overlay1.addColoredLabel("Label cyan", 0.f, 1.f, 1.f);
    overlay1.addButton("Button", [] () noexcept {});
    overlay1.addCheckbox("Checkbox on", [] () noexcept {}, [] () noexcept {}, true);
    overlay1.addCheckbox("Checkbox off", [] () noexcept {}, [] () noexcept {}, false);
    overlay1.addTextbox("Textbox", [] (const std::string&) noexcept {});

    CHECK_THAT(renderFrame(world), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/overlay1_base.png", true)));

    overlay1.disable(); // Hiding the overlay window
  }

  {
    Raz::OverlayWindow& overlay2 = window.getOverlay().addWindow("RaZ - Overlay test 2", Raz::Vec2f(window.getWidth(), window.getHeight()));
    overlay2.addSlider("Slider", [] (float) noexcept {}, 0.f, 10.f, 3.123f);
    overlay2.addListBox("List box", { "Value 0", "Value 1" }, [] (const std::string&, std::size_t) noexcept {}, 1);
    overlay2.addDropdown("Dropdown", { "Value 0", "Value 1" }, [] (const std::string&, std::size_t) noexcept {}, 1);
    overlay2.addProgressBar(0, 100, true).setCurrentValue(35);
    const Raz::Texture2D texture(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/₁₀₀₁.png", true), false);
    overlay2.addTexture(texture, 30, 30);

    CHECK_THAT(renderFrame(world), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/overlay2_base.png", true)));

    overlay2.disable();
  }

  {
    Raz::OverlayWindow& overlay3 = window.getOverlay().addWindow("RaZ - Overlay test 3", Raz::Vec2f(window.getWidth(), window.getHeight()));
    Raz::OverlayPlot& plot = overlay3.addPlot("Plot", 5, "X", "Y", 0.f, 100.f, false, -1.f);

    Raz::OverlayPlotEntry& entry1 = plot.addEntry("1", Raz::OverlayPlotType::LINE);
    entry1.push(0.f);
    entry1.push(50.f);
    entry1.push(75.f);
    entry1.push(25.f);
    entry1.push(10.f);
    entry1.push(50.f); // Pushing more values than the allowed amount will erase those in front

    Raz::OverlayPlotEntry& entry2 = plot.addEntry("2", Raz::OverlayPlotType::SHADED);
    entry2.push(50.f);
    entry2.push(15.f);
    entry2.push(30.f);
    entry2.push(0.f);
    entry2.push(60.f);
    entry2.push(40.f); // Like the above, pushing this final value will remove the first one

    CHECK_THAT(renderFrame(world), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/overlay3_base.png", true)));

    overlay3.disable();
  }
}
