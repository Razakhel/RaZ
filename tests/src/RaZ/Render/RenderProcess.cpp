#include "RaZ/Application.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/ChromaticAberrationRenderProcess.hpp"
#include "RaZ/Render/ConvolutionRenderProcess.hpp"
#include "RaZ/Render/FilmGrainRenderProcess.hpp"
#include "RaZ/Render/PixelizationRenderProcess.hpp"
#include "RaZ/Render/RenderProcess.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/SobelFilterRenderProcess.hpp"
#include "RaZ/Render/VignetteRenderProcess.hpp"
#include "RaZ/Render/Window.hpp"

#include "CatchCustomMatchers.hpp"
#include "TestUtils.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

Raz::Image recoverImage(const Raz::Texture2DPtr& outputTexture, const Raz::FilePath& renderedImgPath = {}) {
  Raz::Image renderedImg = outputTexture->recoverImage();

  if (!renderedImgPath.isEmpty())
    Raz::ImageFormat::save(renderedImgPath, renderedImg, true);

  return renderedImg;
}

} // namespace

TEST_CASE("ChromaticAberrationRenderProcess execution", "[render]") {
  Raz::World world;

  const Raz::Window& window = TestUtils::getWindow();

  auto& render = world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  // RenderSystem::update() needs a Camera with a Transform component
  world.addEntityWithComponents<Raz::Camera, Raz::Transform>();

  const Raz::Image baseImg = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_base.png", true);
  REQUIRE(baseImg.getWidth() == window.getWidth());
  REQUIRE(baseImg.getHeight() == window.getHeight());

  Raz::Texture2DPtr input = Raz::Texture2D::create(baseImg);
  const Raz::Texture2DPtr output = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);

  auto& chromaticAberration = render.getRenderGraph().addRenderProcess<Raz::ChromaticAberrationRenderProcess>();
  chromaticAberration.addParent(render.getGeometryPass());
  chromaticAberration.setInputBuffer(std::move(input));
  chromaticAberration.setOutputBuffer(output);

  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(baseImg));

  // 1
  // 0
  // 1
  Raz::Image img(1, 3, Raz::ImageColorspace::GRAY);
  img.setByteValue(0, 0, 0, 255);
  img.setByteValue(0, 1, 0, 0);
  img.setByteValue(0, 2, 0, 255);

  auto texture = Raz::Texture2D::create(img, false);
  texture->setFilter(Raz::TextureFilter::NEAREST);
  texture->setWrapping(Raz::TextureWrapping::CLAMP);

  chromaticAberration.setStrength(5.f);
  chromaticAberration.setDirection(Raz::Vec2f(1.f, 1.f));
  chromaticAberration.setMaskTexture(std::move(texture));

  world.update({});
  CHECK_THAT(recoverImage(output),
             IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_chromatic_aberration.png", true)));
}

TEST_CASE("ConvolutionRenderProcess execution", "[render]") {
  Raz::World world;

  const Raz::Window& window = TestUtils::getWindow();

  auto& render = world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  // RenderSystem::update() needs a Camera with a Transform component
  world.addEntityWithComponents<Raz::Camera, Raz::Transform>();

  const Raz::Image baseImg = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_base.png", true);
  REQUIRE(baseImg.getWidth() == window.getWidth());
  REQUIRE(baseImg.getHeight() == window.getHeight());

  Raz::Texture2DPtr input = Raz::Texture2D::create(baseImg);
  const Raz::Texture2DPtr output = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);

  // Giving a unit kernel so that the output is the same as the input
  auto& convolution = render.getRenderGraph().addRenderProcess<Raz::ConvolutionRenderProcess>(Raz::Mat3f(0.f, 0.f, 0.f,
                                                                                                         0.f, 1.f, 0.f,
                                                                                                         0.f, 0.f, 0.f));
  convolution.addParent(render.getGeometryPass());
  convolution.setInputBuffer(std::move(input));
  convolution.setOutputBuffer(output);

  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(baseImg));

  convolution.setKernel(Raz::Mat3f(-1.f, -1.f, -1.f,
                                   -1.f,  8.f, -1.f,
                                   -1.f, -1.f, -1.f));
  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_convolved.png", true)));
}

TEST_CASE("FilmGrainRenderProcess execution", "[render][!mayfail]") { // May fail under Linux for yet unknown reasons (second frame is empty)
  Raz::World world;

  const Raz::Window& window = TestUtils::getWindow();

  auto& render = world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  // RenderSystem::update() needs a Camera with a Transform component
  world.addEntityWithComponents<Raz::Camera, Raz::Transform>();

  Raz::Texture2DPtr input = Raz::Texture2D::create(Raz::ColorPreset::White, window.getWidth(), window.getHeight());
  const Raz::Texture2DPtr output = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);

  auto& filmGrain = render.getRenderGraph().addRenderProcess<Raz::FilmGrainRenderProcess>();
  filmGrain.addParent(render.getGeometryPass());
  filmGrain.setInputBuffer(std::move(input));
  filmGrain.setOutputBuffer(output);

  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/film_grain_weak.png", true)));

  filmGrain.setStrength(0.5f);
  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/film_grain_strong.png", true), 0.062f));
}

TEST_CASE("PixelizationRenderProcess execution", "[render][!mayfail]") { // May fail under Linux for yet unknown reasons (second frame is empty)
  Raz::World world;

  const Raz::Window& window = TestUtils::getWindow();

  auto& render = world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  // RenderSystem::update() needs a Camera with a Transform component
  world.addEntityWithComponents<Raz::Camera, Raz::Transform>();

  const Raz::Image baseImg = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_base.png", true);
  REQUIRE(baseImg.getWidth() == window.getWidth());
  REQUIRE(baseImg.getHeight() == window.getHeight());

  Raz::Texture2DPtr input = Raz::Texture2D::create(baseImg);
  const Raz::Texture2DPtr output = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);

  auto& pixelization = render.getRenderGraph().addRenderProcess<Raz::PixelizationRenderProcess>();
  pixelization.addParent(render.getGeometryPass());
  pixelization.setInputBuffer(std::move(input));
  pixelization.setOutputBuffer(output);

  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(baseImg));

  pixelization.setStrength(0.75f);
  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_pixelated.png", true)));
}

TEST_CASE("SobelFilterRenderProcess execution", "[render]") {
  Raz::World world;

  const Raz::Window& window = TestUtils::getWindow();

  auto& render = world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  // RenderSystem::update() needs a Camera with a Transform component
  world.addEntityWithComponents<Raz::Camera, Raz::Transform>();

  const Raz::Image baseImg = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_base.png", true);
  REQUIRE(baseImg.getWidth() == window.getWidth());
  REQUIRE(baseImg.getHeight() == window.getHeight());

  Raz::Texture2DPtr input = Raz::Texture2D::create(baseImg);
  const Raz::Texture2DPtr outputGrad    = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);
  const Raz::Texture2DPtr outputGradDir = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);

  auto& sobel = render.getRenderGraph().addRenderProcess<Raz::SobelFilterRenderProcess>();
  sobel.addParent(render.getGeometryPass());
  sobel.setInputBuffer(std::move(input));
  sobel.setOutputGradientBuffer(outputGrad);
  sobel.setOutputGradientDirectionBuffer(outputGradDir);

  world.update({});
  CHECK_THAT(recoverImage(outputGrad),
             IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_sobel_grad.png", true)));
  CHECK_THAT(recoverImage(outputGradDir),
             IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_sobel_grad_dir.png", true), 0.06f));
}

TEST_CASE("VignetteRenderProcess execution", "[render]") {
  Raz::World world;

  const Raz::Window& window = TestUtils::getWindow();

  auto& render = world.addSystem<Raz::RenderSystem>(window.getWidth(), window.getHeight());

  // RenderSystem::update() needs a Camera with a Transform component
  world.addEntityWithComponents<Raz::Camera, Raz::Transform>();

  Raz::Texture2DPtr input = Raz::Texture2D::create(Raz::ColorPreset::White, window.getWidth(), window.getHeight());
  const Raz::Texture2DPtr output = Raz::Texture2D::create(window.getWidth(), window.getHeight(), Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);

  auto& vignette = render.getRenderGraph().addRenderProcess<Raz::VignetteRenderProcess>();
  vignette.addParent(render.getGeometryPass());
  vignette.setInputBuffer(std::move(input));
  vignette.setOutputBuffer(output);

  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/vignette_weak_black.png", true)));

  vignette.setStrength(1.f);
  vignette.setOpacity(0.5f);
  vignette.setColor(Raz::ColorPreset::Red);
  world.update({});
  CHECK_THAT(recoverImage(output), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/vignette_strong_red.png", true)));
}
