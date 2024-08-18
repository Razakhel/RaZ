#include "RaZ/Render/SobelFilterRenderProcess.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view sobelSource = {
#include "sobel_filter.frag.embed"
};

} // namespace

SobelFilterRenderProcess::SobelFilterRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(sobelSource), "Sobel filter") {}

void SobelFilterRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  const Vec2f invBufferSize(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));
  m_pass.getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
  m_pass.getProgram().sendAttributes();
}

void SobelFilterRenderProcess::setInputBuffer(Texture2DPtr colorBuffer) {
  resizeBuffers(colorBuffer->getWidth(), colorBuffer->getHeight());
  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniBuffer");
}

void SobelFilterRenderProcess::setOutputGradientBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 0);
}

void SobelFilterRenderProcess::setOutputGradientDirectionBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 1);
}

} // namespace Raz
