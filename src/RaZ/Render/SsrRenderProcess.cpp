#include "RaZ/Render/SsrRenderProcess.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

namespace {

constexpr std::string_view ssrSource = {
#include "ssr.frag.embed"
};

} // namespace

SsrRenderProcess::SsrRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(ssrSource), "SSR") {}

void SsrRenderProcess::setInputDepthBuffer(Texture2DPtr depthBuffer) {
  assert("Error: SSR's input depth buffer has an invalid colorspace." && depthBuffer->getColorspace() == TextureColorspace::DEPTH);
  setInputBuffer(std::move(depthBuffer), "uniSceneBuffers.depth");
}

void SsrRenderProcess::setInputColorBuffer(Texture2DPtr colorBuffer) {
  if (!m_pass.hasReadTexture("uniSceneBuffers.blurredColor"))
    setInputBlurredColorBuffer(colorBuffer);

  setInputBuffer(std::move(colorBuffer), "uniSceneBuffers.color");
}

void SsrRenderProcess::setInputBlurredColorBuffer(Texture2DPtr blurredColorBuffer) {
  setInputBuffer(std::move(blurredColorBuffer), "uniSceneBuffers.blurredColor");
}

void SsrRenderProcess::setInputNormalBuffer(Texture2DPtr normalBuffer) {
  assert("Error: SSR's input normal buffer has an invalid colorspace." && normalBuffer->getColorspace() == TextureColorspace::RGB);
  setInputBuffer(std::move(normalBuffer), "uniSceneBuffers.normal");
}

void SsrRenderProcess::setInputSpecularBuffer(Texture2DPtr specularBuffer) {
  assert("Error: SSR's input specular buffer has an invalid colorspace." && specularBuffer->getColorspace() == TextureColorspace::RGBA);
  setInputBuffer(std::move(specularBuffer), "uniSceneBuffers.specular");
}

void SsrRenderProcess::setOutputBuffer(Texture2DPtr outputBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(outputBuffer), 0);
}

} // namespace Raz
