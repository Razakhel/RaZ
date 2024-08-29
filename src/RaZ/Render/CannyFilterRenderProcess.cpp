#include "RaZ/Render/CannyFilterRenderProcess.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view cannySource = {
#include "canny_filter.frag.embed"
};

} // namespace

CannyFilterRenderProcess::CannyFilterRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(cannySource), "Canny filter") {
  setLowerBound(0.1f);
  setUpperBound(0.3f);
}

void CannyFilterRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  const Vec2f invBufferSize(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));
  m_pass.getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
  m_pass.getProgram().sendAttributes();
}

void CannyFilterRenderProcess::setInputGradientBuffer(Texture2DPtr gradientBuffer) {
  assert("Error: The input gradient buffer is invalid." && gradientBuffer != nullptr);

  resizeBuffers(gradientBuffer->getWidth(), gradientBuffer->getHeight());
  MonoPassRenderProcess::setInputBuffer(std::move(gradientBuffer), "uniGradients");
}

void CannyFilterRenderProcess::setInputGradientDirectionBuffer(Texture2DPtr gradDirBuffer) {
  MonoPassRenderProcess::setInputBuffer(std::move(gradDirBuffer), "uniGradDirs");
}

void CannyFilterRenderProcess::setOutputBuffer(Texture2DPtr binaryBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(binaryBuffer), 0);
}

void CannyFilterRenderProcess::setLowerBound(float lowerBound) const {
  m_pass.getProgram().setAttribute(lowerBound, "uniLowerBound");
  m_pass.getProgram().sendAttributes();
}

void CannyFilterRenderProcess::setUpperBound(float upperBound) const {
  m_pass.getProgram().setAttribute(upperBound, "uniUpperBound");
  m_pass.getProgram().sendAttributes();
}

} // namespace Raz
