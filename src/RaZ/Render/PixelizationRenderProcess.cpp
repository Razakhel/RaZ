#include "RaZ/Render/PixelizationRenderProcess.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view pixelizationSource = {
#include "pixelization.frag.embed"
};

} // namespace

PixelizationRenderProcess::PixelizationRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(pixelizationSource), "Pixelization") { setStrength(0.f); }

void PixelizationRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  const Vec2f bufferSize(static_cast<float>(width), static_cast<float>(height));
  m_pass.getProgram().setAttribute(bufferSize, "uniBufferSize");
  m_pass.getProgram().sendAttributes();
}

void PixelizationRenderProcess::setInputBuffer(Texture2DPtr colorBuffer) {
  resizeBuffers(colorBuffer->getWidth(), colorBuffer->getHeight());
  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniBuffer");
}

void PixelizationRenderProcess::setOutputBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 0);
}

void PixelizationRenderProcess::setStrength(float strength) const {
  m_pass.getProgram().setAttribute(strength, "uniStrength");
  m_pass.getProgram().sendAttributes();
}

} // namespace Raz
