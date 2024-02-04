#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/ScreenSpaceReflectionsRenderProcess.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

namespace {

constexpr std::string_view ssrSource = {
#include "ssr.frag.embed"
};

constexpr std::string_view depthUniformName        = "uniSceneBuffers.depth";
constexpr std::string_view colorUniformName        = "uniSceneBuffers.color";
constexpr std::string_view blurredColorUniformName = "uniSceneBuffers.blurredColor";
constexpr std::string_view normalUniformName       = "uniSceneBuffers.normal";
constexpr std::string_view specularUniformName     = "uniSceneBuffers.specular";

} // namespace

ScreenSpaceReflectionsRenderProcess::ScreenSpaceReflectionsRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(ssrSource), "Screen space reflections (SSR)") {}

void ScreenSpaceReflectionsRenderProcess::setInputDepthBuffer(Texture2DPtr depthBuffer) {
  assert("Error: The SSR's input depth buffer has an invalid colorspace." && depthBuffer->getColorspace() == TextureColorspace::DEPTH);
  setInputBuffer(std::move(depthBuffer), std::string(depthUniformName));
}

void ScreenSpaceReflectionsRenderProcess::setInputColorBuffer(Texture2DPtr colorBuffer) {
  if (!m_pass.hasReadTexture(std::string(blurredColorUniformName)))
    setInputBlurredColorBuffer(colorBuffer);

  setInputBuffer(std::move(colorBuffer), std::string(colorUniformName));
}

void ScreenSpaceReflectionsRenderProcess::setInputBlurredColorBuffer(Texture2DPtr blurredColorBuffer) {
  setInputBuffer(std::move(blurredColorBuffer), std::string(blurredColorUniformName));
}

void ScreenSpaceReflectionsRenderProcess::setInputNormalBuffer(Texture2DPtr normalBuffer) {
  assert("Error: The SSR's input normal buffer has an invalid colorspace." && normalBuffer->getColorspace() == TextureColorspace::RGB);
  setInputBuffer(std::move(normalBuffer), std::string(normalUniformName));
}

void ScreenSpaceReflectionsRenderProcess::setInputSpecularBuffer(Texture2DPtr specularBuffer) {
  assert("Error: The SSR's input specular buffer has an invalid colorspace." && specularBuffer->getColorspace() == TextureColorspace::RGBA);
  setInputBuffer(std::move(specularBuffer), std::string(specularUniformName));
}

void ScreenSpaceReflectionsRenderProcess::setOutputBuffer(Texture2DPtr outputBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(outputBuffer), 0);
}

} // namespace Raz
