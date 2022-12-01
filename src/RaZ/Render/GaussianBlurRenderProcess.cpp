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

GaussianBlurRenderProcess::GaussianBlurRenderProcess(RenderGraph& renderGraph) : RenderProcess(renderGraph), m_horizontalBuffer{ Texture2D::create() } {
  // Two-pass gaussian blur based on:
  //  - https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
  //  - https://www.intel.com/content/www/us/en/developer/articles/technical/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms.html

  /////////////////////
  // Horizontal pass //
  /////////////////////

  m_horizontalPass = &renderGraph.addNode(FragmentShader::loadFromSource(gaussianBlurSource), "Gaussian blur (horizontal)");
  m_horizontalPass->getProgram().setAttribute(Vec2f(1.f, 0.f), "uniBlurDirection");
  m_horizontalPass->getProgram().sendAttributes();

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::PROGRAM, m_horizontalPass->getProgram().getIndex(), "Gaussian blur (horizontal) program");
    Renderer::setLabel(RenderObjectType::SHADER, m_horizontalPass->getProgram().getVertexShader().getIndex(), "Gaussian blur (horizontal) vertex shader");
    Renderer::setLabel(RenderObjectType::SHADER, m_horizontalPass->getProgram().getFragmentShader().getIndex(), "Gaussian blur (horizontal) fragment shader");
  }
#endif

  ///////////////////
  // Vertical pass //
  ///////////////////

  m_verticalPass = &renderGraph.addNode(FragmentShader::loadFromSource(gaussianBlurSource), "Gaussian blur (vertical)");
  m_verticalPass->getProgram().setAttribute(Vec2f(0.f, 1.f), "uniBlurDirection");
  m_verticalPass->getProgram().sendAttributes();

  m_verticalPass->addReadTexture(m_horizontalBuffer, "uniBuffer");

  m_verticalPass->addParents(*m_horizontalPass);

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::PROGRAM, m_verticalPass->getProgram().getIndex(), "Gaussian blur (vertical) program");
    Renderer::setLabel(RenderObjectType::SHADER, m_verticalPass->getProgram().getVertexShader().getIndex(), "Gaussian blur (vertical) vertex shader");
    Renderer::setLabel(RenderObjectType::SHADER, m_verticalPass->getProgram().getFragmentShader().getIndex(), "Gaussian blur (vertical) fragment shader");
  }
#endif

  // Validating the render graph
  if (!renderGraph.isValid())
    throw std::runtime_error("Error: The gaussian blur process is invalid");
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

void GaussianBlurRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  m_horizontalBuffer->resize(width, height);

  const Vec2f invBufferSize(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));

  m_horizontalPass->getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
  m_horizontalPass->getProgram().sendAttributes();

  m_verticalPass->getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
  m_verticalPass->getProgram().sendAttributes();
}

void GaussianBlurRenderProcess::setInputBuffer(Texture2DPtr inputBuffer) {
  m_horizontalBuffer->setColorspace(inputBuffer->getColorspace(), inputBuffer->getDataType());
  resizeBuffers(inputBuffer->getWidth(), inputBuffer->getHeight());

  m_horizontalPass->clearReadTextures();
  m_horizontalPass->addReadTexture(std::move(inputBuffer), "uniBuffer");

  m_horizontalPass->clearWriteTextures();
  m_horizontalPass->addWriteColorTexture(m_horizontalBuffer, 0);

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::FRAMEBUFFER, m_horizontalPass->getFramebuffer().getIndex(), "Gaussian blur (horizontal) framebuffer");
    Renderer::setLabel(RenderObjectType::TEXTURE, m_horizontalBuffer->getIndex(), "Gaussian blurred (horizontal) buffer");
  }
#endif

  // Validating the render graph
  if (!m_renderGraph.isValid())
    throw std::runtime_error("Error: The gaussian blur process is invalid");
}

void GaussianBlurRenderProcess::setOutputBuffer(Texture2DPtr outputBuffer) {
  m_verticalPass->addWriteColorTexture(std::move(outputBuffer), 0);

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3))
    Renderer::setLabel(RenderObjectType::FRAMEBUFFER, m_verticalPass->getFramebuffer().getIndex(), "Gaussian blur (vertical) framebuffer");
#endif
}

} // namespace Raz
