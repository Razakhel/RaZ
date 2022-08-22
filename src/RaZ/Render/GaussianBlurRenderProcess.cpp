#include "RaZ/Render/GaussianBlurRenderProcess.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/Texture.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view gaussianBlurSource = {
#include "gaussian_blur.frag.embed"
};

} // namespace

GaussianBlurRenderProcess::GaussianBlurRenderProcess(RenderGraph& renderGraph, unsigned int frameWidth, unsigned int frameHeight) {
  // Two-pass gaussian blur based on:
  //  - https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
  //  - https://www.intel.com/content/www/us/en/developer/articles/technical/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms.html

  const Vec2f invBufferSize(1.f / static_cast<float>(frameWidth), 1.f / static_cast<float>(frameHeight));

  /////////////////////
  // Horizontal pass //
  /////////////////////

  m_horizontalPass = &renderGraph.addNode(FragmentShader::loadFromSource(gaussianBlurSource), "Gaussian blur (horizontal)");
  m_horizontalPass->getProgram().use();
  m_horizontalPass->getProgram().sendUniform("uniInvBufferSize", invBufferSize);
  m_horizontalPass->getProgram().sendUniform("uniBlurDirection", Vec2f(1.f, 0.f));

  const auto horizontalBlurredBuffer = Texture::create(frameWidth, frameHeight, ImageColorspace::RGB, ImageDataType::FLOAT);
  m_horizontalPass->addWriteTexture(horizontalBlurredBuffer);

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::PROGRAM, m_horizontalPass->getProgram().getIndex(), "Gaussian blur (horizontal) program");
    Renderer::setLabel(RenderObjectType::SHADER, m_horizontalPass->getProgram().getVertexShader().getIndex(), "Gaussian blur (horizontal) vertex shader");
    Renderer::setLabel(RenderObjectType::SHADER, m_horizontalPass->getProgram().getFragmentShader().getIndex(), "Gaussian blur (horizontal) fragment shader");
    Renderer::setLabel(RenderObjectType::FRAMEBUFFER, m_horizontalPass->getFramebuffer().getIndex(), "Gaussian blur (horizontal) framebuffer");
    Renderer::setLabel(RenderObjectType::TEXTURE, horizontalBlurredBuffer->getIndex(), "Gaussian blurred (horizontal) buffer");
  }
#endif

  ///////////////////
  // Vertical pass //
  ///////////////////

  m_verticalPass = &renderGraph.addNode(FragmentShader::loadFromSource(gaussianBlurSource), "Gaussian blur (vertical)");
  m_verticalPass->getProgram().use();
  m_verticalPass->getProgram().sendUniform("uniInvBufferSize", invBufferSize);
  m_verticalPass->getProgram().sendUniform("uniBlurDirection", Vec2f(0.f, 1.f));

  m_verticalPass->addParents(*m_horizontalPass);
  m_verticalPass->addReadTexture(horizontalBlurredBuffer, "uniBuffer");

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::PROGRAM, m_verticalPass->getProgram().getIndex(), "Gaussian blur (vertical) program");
    Renderer::setLabel(RenderObjectType::SHADER, m_verticalPass->getProgram().getVertexShader().getIndex(), "Gaussian blur (vertical) vertex shader");
    Renderer::setLabel(RenderObjectType::SHADER, m_verticalPass->getProgram().getFragmentShader().getIndex(), "Gaussian blur (vertical) fragment shader");
    Renderer::setLabel(RenderObjectType::FRAMEBUFFER, m_verticalPass->getFramebuffer().getIndex(), "Gaussian blur (vertical) framebuffer");
  }
#endif

  // Validating the render graph
  if (!renderGraph.isValid())
    throw std::runtime_error("Error: The bloom process is invalid");
}

bool GaussianBlurRenderProcess::isEnabled() const noexcept {
  return m_horizontalPass->isEnabled();
}

void GaussianBlurRenderProcess::setState(bool enabled) {
  m_horizontalPass->enable(enabled);
  m_verticalPass->enable(enabled);
}

void GaussianBlurRenderProcess::addParent(RenderPass& parentPass) {
  m_horizontalPass->addParents(parentPass);
}

void GaussianBlurRenderProcess::addParent(RenderProcess& parentProcess) {
  parentProcess.addChild(*m_horizontalPass);
}

void GaussianBlurRenderProcess::addChild(RenderPass& childPass) {
  m_verticalPass->addChildren(childPass);
}

void GaussianBlurRenderProcess::addChild(RenderProcess& childProcess) {
  childProcess.addParent(*m_verticalPass);
}

void GaussianBlurRenderProcess::setOutputBuffer(TexturePtr outputBuffer) {
  m_verticalPass->addWriteTexture(std::move(outputBuffer));
}

void GaussianBlurRenderProcess::setInputBuffer(TexturePtr inputBuffer) {
  m_horizontalPass->addReadTexture(std::move(inputBuffer), "uniBuffer");
}

} // namespace Raz
