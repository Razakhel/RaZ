#include "RaZ/Render/SsaoRenderProcess.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

namespace {

constexpr std::string_view ssaoSource = {
#include "ssao.frag.embed"
};

} // namespace

SsaoRenderProcess::SsaoRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(ssaoSource), "SSAO") {}

void SsaoRenderProcess::setInputDepthBuffer(Texture2DPtr depthBuffer) {
  assert("Error: SSAO's input depth buffer has an invalid colorspace." && depthBuffer->getColorspace() == TextureColorspace::DEPTH);
  setInputBuffer(std::move(depthBuffer), "uniSceneBuffers.depth");
}

void SsaoRenderProcess::setInputNormalBuffer(Texture2DPtr normalBuffer) {
  assert("Error: SSAO's input normal buffer has an invalid colorspace." && normalBuffer->getColorspace() == TextureColorspace::RGB);
  setInputBuffer(std::move(normalBuffer), "uniSceneBuffers.normal");
}

void SsaoRenderProcess::setOutputBuffer(Texture2DPtr outputBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(outputBuffer), 0);
}

} // namespace Raz
