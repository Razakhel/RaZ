#include "RaZ/Render/BoxBlurRenderProcess.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view boxBlurSource = {
#include "box_blur.frag.embed"
};

} // namespace

BoxBlurRenderProcess::BoxBlurRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(boxBlurSource), "Box blur") { setStrength(1); }

void BoxBlurRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  const Vec2f invBufferSize(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));
  m_pass.getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
  m_pass.getProgram().sendAttributes();
}

void BoxBlurRenderProcess::setInputBuffer(Texture2DPtr colorBuffer) {
  resizeBuffers(colorBuffer->getWidth(), colorBuffer->getHeight());
  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniBuffer");
}

void BoxBlurRenderProcess::setOutputBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 0);
}

void BoxBlurRenderProcess::setStrength(unsigned int strength) const {
  m_pass.getProgram().setAttribute(static_cast<int>(strength), "uniKernelSize");
  m_pass.getProgram().sendAttributes();
}

} // namespace Raz
