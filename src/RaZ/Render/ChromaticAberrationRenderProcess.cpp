#include "RaZ/Data/Color.hpp"
#include "RaZ/Render/ChromaticAberrationRenderProcess.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view chromaticAberrationSource = {
#include "chromatic_aberration.frag.embed"
};

} // namespace

ChromaticAberrationRenderProcess::ChromaticAberrationRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(chromaticAberrationSource), "Chromatic aberration") {
  setStrength(0.f);
  setDirection(Vec2f(1.f, 0.f));
  setMaskTexture(Texture2D::create(ColorPreset::White));
}

void ChromaticAberrationRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  const Vec2f invBufferSize(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));
  m_pass.getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
  m_pass.getProgram().sendAttributes();
}

void ChromaticAberrationRenderProcess::setInputBuffer(Texture2DPtr colorBuffer) {
  resizeBuffers(colorBuffer->getWidth(), colorBuffer->getHeight());
  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniBuffer");
}

void ChromaticAberrationRenderProcess::setOutputBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 0);
}

void ChromaticAberrationRenderProcess::setStrength(float strength) const {
  m_pass.getProgram().setAttribute(strength, "uniStrength");
  m_pass.getProgram().sendAttributes();
}

void ChromaticAberrationRenderProcess::setDirection(const Vec2f& direction) const {
  m_pass.getProgram().setAttribute(direction, "uniDirection");
  m_pass.getProgram().sendAttributes();
}

void ChromaticAberrationRenderProcess::setMaskTexture(Texture2DPtr mask) const {
  m_pass.getProgram().setTexture(std::move(mask), "uniMask");
}

} // namespace Raz
