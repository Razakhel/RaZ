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

void BoxBlurRenderProcess::setInputBuffer(TexturePtr colorBuffer) {
  const Vec2f invBufferSize(1.f / static_cast<float>(colorBuffer->getImage().getWidth()), 1.f / static_cast<float>(colorBuffer->getImage().getHeight()));

  m_pass.getProgram().use();
  m_pass.getProgram().sendUniform("uniInvBufferSize", invBufferSize);

  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniBuffer");
}

void BoxBlurRenderProcess::setStrength(unsigned int strength) {
  m_pass.getProgram().use();
  m_pass.getProgram().sendUniform("uniKernelSize", static_cast<int>(strength));
}

} // namespace Raz
