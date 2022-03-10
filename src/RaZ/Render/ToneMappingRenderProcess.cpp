#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/ToneMappingRenderProcess.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view reinhardSource = {
#include "reinhard_tone_mapping.frag.embed"
};

} // namespace

ReinhardToneMapping::ReinhardToneMapping(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(reinhardSource)) {}

void ReinhardToneMapping::setInputBuffer(Texture2DPtr colorBuffer) {
  assert("Error: Reinhard tone mapping's input buffer must have an RGB colorspace." && colorBuffer->getColorspace() == TextureColorspace::RGB);
  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniColorBuffer");
}

void ReinhardToneMapping::setOutputBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 0);
}

void ReinhardToneMapping::setMaxWhiteValue(float strength) const {
  m_pass.getProgram().setAttribute(strength, "uniMaxWhite");
  m_pass.getProgram().sendAttributes();
}

} // namespace Raz
