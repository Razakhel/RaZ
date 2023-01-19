#include "RaZ/Data/Color.hpp"
#include "RaZ/Render/VignetteRenderProcess.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view vignetteSource = {
#include "vignette.frag.embed"
};

} // namespace

VignetteRenderProcess::VignetteRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(vignetteSource), "Vignette") {
  setStrength(0.25f);
  setOpacity(1.f);
  setColor(ColorPreset::Black);
}

void VignetteRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  const float frameRatio = static_cast<float>(width) / static_cast<float>(height);
  m_pass.getProgram().setAttribute(frameRatio, "uniFrameRatio");
  m_pass.getProgram().sendAttributes();
}

void VignetteRenderProcess::setInputBuffer(Texture2DPtr colorBuffer) {
  resizeBuffers(colorBuffer->getWidth(), colorBuffer->getHeight());
  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniBuffer");
}

void VignetteRenderProcess::setOutputBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 0);
}

void VignetteRenderProcess::setStrength(float strength) const {
  m_pass.getProgram().setAttribute(strength, "uniStrength");
  m_pass.getProgram().sendAttributes();
}

void VignetteRenderProcess::setOpacity(float opacity) const {
  m_pass.getProgram().setAttribute(opacity, "uniOpacity");
  m_pass.getProgram().sendAttributes();
}

void VignetteRenderProcess::setColor(const Color& color) const {
  m_pass.getProgram().setAttribute(color, "uniColor");
  m_pass.getProgram().sendAttributes();
}

} // namespace Raz
