#include "Catch.hpp"
#include "TestUtils.hpp"

#include "RaZ/Application.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/ConvolutionRenderProcess.hpp"
#include "RaZ/Render/RenderProcess.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"

namespace {

Raz::Image renderFrame(Raz::World& world, const Raz::Texture2DPtr& output, const Raz::FilePath& renderedImgPath = {}) {
  Raz::Window& window = TestUtils::getWindow();

  // Rendering a frame of the scene by updating the World's RenderSystem & running the Window
  world.update({});
  window.run(0.f);

#if !defined(USE_OPENGL_ES)
  Raz::Image renderedImg = output->recoverImage();
#else
  // Recovering an image directly from a texture (glReadPixels()) is not possible with OpenGL ES; a framebuffer must be used to read the texture from instead
  // See: https://stackoverflow.com/a/53993894/3292304
  const Raz::Framebuffer intermFramebuffer;
  Raz::Renderer::bindFramebuffer(intermFramebuffer.getIndex(), Raz::FramebufferType::READ_FRAMEBUFFER);
  Raz::Renderer::setFramebufferTexture2D(Raz::FramebufferAttachment::COLOR0,
                                         output->getIndex(),
                                         0,
                                         Raz::TextureType::TEXTURE_2D,
                                         Raz::FramebufferType::READ_FRAMEBUFFER);

  Raz::Image renderedImg(window.getWidth(), window.getHeight(), Raz::ImageColorspace::RGB, Raz::ImageDataType::BYTE);
  Raz::Renderer::recoverFrame(window.getWidth(), window.getHeight(), Raz::TextureFormat::RGB, Raz::PixelDataType::UBYTE, renderedImg.getDataPtr());

  Raz::Renderer::unbindFramebuffer(Raz::FramebufferType::READ_FRAMEBUFFER);
#endif

  if (!renderedImgPath.isEmpty())
    Raz::ImageFormat::save(renderedImgPath, renderedImg, true);

  return renderedImg;
}

} // namespace

TEST_CASE("ConvolutionRenderProcess execution") {
  Raz::World world(1);

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
  convolution.setInputBuffer(std::move(input));
  convolution.setOutputBuffer(output);

  CHECK_THAT(renderFrame(world, output), IsNearlyEqualToImage(baseImg));

  convolution.setKernel(Raz::Mat3f(-1.f, -1.f, -1.f,
                                   -1.f,  8.f, -1.f,
                                   -1.f, -1.f, -1.f));
  CHECK_THAT(renderFrame(world, output), IsNearlyEqualToImage(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/renders/cook-torrance_ball_convolved.png", true)));
}
