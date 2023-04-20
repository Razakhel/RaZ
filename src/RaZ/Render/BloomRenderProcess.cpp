#include "RaZ/Render/BloomRenderProcess.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/Texture.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr int passCount = 5;

constexpr std::string_view thresholdSource = R"(
  in vec2 fragTexcoords;

  uniform sampler2D uniColorBuffer;
  uniform float uniThreshold;

  layout(location = 0) out vec4 fragColor;

  void main() {
    vec3 color = texture(uniColorBuffer, fragTexcoords).rgb;

    // Thresholding pixels according to their luminance: https://en.wikipedia.org/wiki/Luma_(video)#Use_of_relative_luminance
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    fragColor        = vec4(color * float(brightness > uniThreshold), 1.0);
  }
)";

constexpr std::string_view downscaleSource = R"(
  in vec2 fragTexcoords;

  uniform sampler2D uniPrevDownscaledBuffer;
  uniform vec2 uniInvBufferSize;

  layout(location = 0) out vec4 fragColor;

  const vec2 kernelOffsets[13] = vec2[](
    vec2(-1.0,  1.0), vec2(1.0,  1.0),
    vec2(-1.0, -1.0), vec2(1.0, -1.0),

    vec2(-2.0,  2.0), vec2(0.0,  2.0), vec2(2.0,  2.0),
    vec2(-2.0,  0.0), vec2(0.0,  0.0), vec2(2.0,  0.0),
    vec2(-2.0, -2.0), vec2(0.0, -2.0), vec2(2.0, -2.0)
  );

  const float kernelWeights[13] = float[](
    // 4 inner samples: (1 / 4) * 0.5
    0.125, 0.125,
    0.125, 0.125,

    // 1 middle & 8 outer samples: (1 / 9) * 0.5
    0.0555555, 0.0555555, 0.0555555,
    0.0555555, 0.0555555, 0.0555555,
    0.0555555, 0.0555555, 0.0555555
  );

  void main() {
    vec3 color = vec3(0.0);

    for (int i = 0; i < 13; ++i) {
      vec2 normalizedTexcoords = (gl_FragCoord.xy + kernelOffsets[i]) * uniInvBufferSize;
      color += texture(uniPrevDownscaledBuffer, normalizedTexcoords).rgb * kernelWeights[i];
    }

    fragColor = vec4(color, 1.0);
  }
)";

constexpr std::string_view upscaleSource = R"(
  in vec2 fragTexcoords;

  uniform sampler2D uniDownscaledBuffer;
  uniform sampler2D uniPrevUpscaledBuffer;
  uniform vec2 uniInvBufferSize;

  layout(location = 0) out vec4 fragColor;

  const vec2 kernelOffsets[9] = vec2[](
      vec2(-1.0,  1.0), vec2(0.0,  1.0), vec2(1.0,  1.0),
      vec2(-1.0,  0.0), vec2(0.0,  0.0), vec2(1.0,  0.0),
      vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0)
  );

  const float kernelWeights[9] = float[](
      0.0625, 0.125, 0.0625,
      0.125,  0.25,  0.125,
      0.0625, 0.125, 0.0625
  );

  void main() {
    vec3 color = texture(uniDownscaledBuffer, gl_FragCoord.xy * uniInvBufferSize).rgb;

    for (int i = 0; i < 9; ++i) {
      vec2 normalizedTexcoords = (gl_FragCoord.xy + kernelOffsets[i]) * uniInvBufferSize;
      color += texture(uniPrevUpscaledBuffer, normalizedTexcoords).rgb * kernelWeights[i];
    }

    fragColor = vec4(color, 1.0);
  }
)";

constexpr std::string_view finalSource = R"(
  in vec2 fragTexcoords;

  uniform sampler2D uniOriginalColorBuffer;
  uniform sampler2D uniFinalUpscaledBuffer;

  layout(location = 0) out vec4 fragColor;

  void main() {
    vec3 originalColor = texture(uniOriginalColorBuffer, fragTexcoords).rgb;
    vec3 blurredColor  = texture(uniFinalUpscaledBuffer, fragTexcoords).rgb;

    // The following is technically incorrect, since tone mapping must be done on the whole scene at the very end of the rendering. This will be removed later
    //blurredColor = blurredColor / (blurredColor + vec3(1.0)); // Tone mapping
    //blurredColor = pow(blurredColor, vec3(1.0 / 2.2)); // Gamma correction

    // TODO: divide the result by the amount of passes (https://www.froyok.fr/blog/2021-12-ue4-custom-bloom/#upsample_shader)
    fragColor = vec4(originalColor + blurredColor, 1.0);
  }
)";

} // namespace

BloomRenderProcess::BloomRenderProcess(RenderGraph& renderGraph) : RenderProcess(renderGraph) {
  // Based on Fabrice "froyok" Piquet's bloom, itself based on the one used in Unreal Engine 4/Call of Duty: Advanced Warfare
  // See: https://www.froyok.fr/blog/2021-12-ue4-custom-bloom/

  //////////////////
  // Thresholding //
  //////////////////

  m_thresholdPass = &renderGraph.addNode(FragmentShader::loadFromSource(thresholdSource), "Bloom thresholding");
  setThresholdValue(1.f);

  const auto thresholdBuffer = Texture2D::create(TextureColorspace::RGB, TextureDataType::FLOAT16);
  m_thresholdPass->addWriteColorTexture(thresholdBuffer, 0);

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::PROGRAM, m_thresholdPass->getProgram().getIndex(), "Bloom threshold program");
    Renderer::setLabel(RenderObjectType::SHADER, m_thresholdPass->getProgram().getVertexShader().getIndex(), "Bloom threshold vertex shader");
    Renderer::setLabel(RenderObjectType::SHADER, m_thresholdPass->getProgram().getFragmentShader().getIndex(), "Bloom threshold fragment shader");
    Renderer::setLabel(RenderObjectType::FRAMEBUFFER, m_thresholdPass->getFramebuffer().getIndex(), "Bloom threshold framebuffer");
    Renderer::setLabel(RenderObjectType::TEXTURE, thresholdBuffer->getIndex(), "Bloom threshold buffer");
  }
#endif

  /////////////////
  // Downscaling //
  /////////////////

  m_downscalePasses.resize(passCount);
  m_downscaleBuffers.resize(passCount);

  for (std::size_t downscalePassIndex = 0; downscalePassIndex < passCount; ++downscalePassIndex) {
    const std::string idStr = std::to_string(downscalePassIndex);

    RenderPass& downscalePass = renderGraph.addNode(FragmentShader::loadFromSource(downscaleSource), "Bloom downscale #" + idStr);

    //  ----------
    //  |        |
    //  |   D0   |
    //  |        |
    //  ----------
    //      |
    //      v prevDownscaledBuffer
    //    ------
    //    | D1 |
    //    ------
    //      |
    //      v prevDownscaledBuffer
    //     ...

    downscalePass.addReadTexture((downscalePassIndex == 0 ? thresholdBuffer : m_downscaleBuffers[downscalePassIndex - 1].lock()), "uniPrevDownscaledBuffer");

    const auto downscaledBuffer = Texture2D::create(TextureColorspace::RGB, TextureDataType::FLOAT16);
    downscalePass.addWriteColorTexture(downscaledBuffer, 0);

    m_downscalePasses[downscalePassIndex]  = &downscalePass;
    m_downscaleBuffers[downscalePassIndex] = downscaledBuffer;

    downscalePass.addParents((downscalePassIndex == 0 ? *m_thresholdPass : *m_downscalePasses[downscalePassIndex - 1]));

#if !defined(USE_OPENGL_ES)
    if (Renderer::checkVersion(4, 3)) {
      Renderer::setLabel(RenderObjectType::PROGRAM, downscalePass.getProgram().getIndex(), "Bloom downscale program #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, downscalePass.getProgram().getVertexShader().getIndex(), "Bloom downscale vertex shader #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, downscalePass.getProgram().getFragmentShader().getIndex(), "Bloom downscale fragment shader #" + idStr);
      Renderer::setLabel(RenderObjectType::FRAMEBUFFER, downscalePass.getFramebuffer().getIndex(), "Bloom downscale framebuffer #" + idStr);
      Renderer::setLabel(RenderObjectType::TEXTURE, downscaledBuffer->getIndex(), "Bloom downscale buffer #" + idStr);
    }
#endif
  }

  ///////////////
  // Upscaling //
  ///////////////

  m_upscalePasses.resize(passCount - 1);
  m_upscaleBuffers.resize(passCount - 1);

  for (std::size_t upscalePassIndex = 0; upscalePassIndex < passCount - 1; ++upscalePassIndex) {
    const std::string idStr = std::to_string(upscalePassIndex);

    RenderPass& upscalePass = renderGraph.addNode(FragmentShader::loadFromSource(upscaleSource), "Bloom upscale #" + idStr);

    //  ----------                ----------
    //  |        |                |        |
    //  |   D0   |--------------->|   U0   |
    //  |        |                |        |
    //  ----------                ----------
    //      |                         ^
    //      v                         |
    //    ------   downscaledBuffer ------
    //    | D1 |------------------->| U1 |
    //    ------                    ------
    //      |                         ^ prevUpscaledBuffer
    //      v                         |
    //     ... ------------------------

    const std::size_t correspDownscalePassIndex = passCount - upscalePassIndex - 2;

    upscalePass.addReadTexture(m_downscaleBuffers[correspDownscalePassIndex].lock(), "uniDownscaledBuffer");
    upscalePass.addReadTexture((upscalePassIndex == 0 ? m_downscaleBuffers.back() : m_upscaleBuffers[upscalePassIndex - 1]).lock(), "uniPrevUpscaledBuffer");

    const auto upscaledBuffer = Texture2D::create(TextureColorspace::RGB, TextureDataType::FLOAT16);
    upscalePass.addWriteColorTexture(upscaledBuffer, 0);

    m_upscalePasses[upscalePassIndex]  = &upscalePass;
    m_upscaleBuffers[upscalePassIndex] = upscaledBuffer;

    // Although each upscaling pass is technically dependent on the matching downscaling one, the render graph only needs
    //  direct dependencies, that is, passes that can be executed anytime after their parents have been. In this case, we need
    //  to execute each one sequentially whenever the previous upscaling pass has finished anyway. Hence, although we could, we
    //  do not set any dependency between upscaling & downscaling passes aside from the first one

    upscalePass.addParents((upscalePassIndex == 0 ? *m_downscalePasses.back() : *m_upscalePasses[upscalePassIndex - 1]));

#if !defined(USE_OPENGL_ES)
    if (Renderer::checkVersion(4, 3)) {
      Renderer::setLabel(RenderObjectType::PROGRAM, upscalePass.getProgram().getIndex(), "Bloom upscale program #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, upscalePass.getProgram().getVertexShader().getIndex(), "Bloom upscale vertex shader #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, upscalePass.getProgram().getFragmentShader().getIndex(), "Bloom upscale fragment shader #" + idStr);
      Renderer::setLabel(RenderObjectType::FRAMEBUFFER, upscalePass.getFramebuffer().getIndex(), "Bloom upscale framebuffer #" + idStr);
      Renderer::setLabel(RenderObjectType::TEXTURE, upscaledBuffer->getIndex(), "Bloom upscale buffer #" + idStr);
    }
#endif
  }

  ////////////////////////
  // Final display pass //
  ////////////////////////

  m_finalPass = &renderGraph.addNode(FragmentShader::loadFromSource(finalSource), "Bloom final pass");

  m_finalPass->addParents(*m_upscalePasses.back());
  m_finalPass->addReadTexture(m_upscaleBuffers.back().lock(), "uniFinalUpscaledBuffer");

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::PROGRAM, m_finalPass->getProgram().getIndex(), "Bloom final pass program");
    Renderer::setLabel(RenderObjectType::SHADER, m_finalPass->getProgram().getVertexShader().getIndex(), "Bloom final pass vertex shader");
    Renderer::setLabel(RenderObjectType::SHADER, m_finalPass->getProgram().getFragmentShader().getIndex(), "Bloom final pass fragment shader");
  }
#endif

  // Validating the render graph
  if (!renderGraph.isValid())
    throw std::runtime_error("Error: The bloom process is invalid");
}

bool BloomRenderProcess::isEnabled() const noexcept {
  return m_thresholdPass->isEnabled();
}

void BloomRenderProcess::setState(bool enabled) {
  m_thresholdPass->enable(enabled);

  for (RenderPass* downscalePass : m_downscalePasses)
    downscalePass->enable(enabled);

  for (RenderPass* upscalePass : m_upscalePasses)
    upscalePass->enable(enabled);

  m_finalPass->enable(enabled);
}

void BloomRenderProcess::addParent(RenderPass& parentPass) {
  m_thresholdPass->addParents(parentPass);
}

void BloomRenderProcess::addParent(RenderProcess& parentProcess) {
  parentProcess.addChild(*m_thresholdPass);
}

void BloomRenderProcess::addChild(RenderPass& childPass) {
  m_finalPass->addChildren(childPass);
}

void BloomRenderProcess::addChild(RenderProcess& childProcess) {
  childProcess.addParent(*m_finalPass);
}

void BloomRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  m_thresholdPass->resizeWriteBuffers(width, height);
  m_finalPass->resizeWriteBuffers(width, height);

  for (std::size_t i = 0; i < m_downscaleBuffers.size(); ++i) {
    width  /= 2;
    height /= 2;

    const Vec2f invBufferSize(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));

    m_downscalePasses[i]->resizeWriteBuffers(width, height);

    m_downscalePasses[i]->getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
    m_downscalePasses[i]->getProgram().sendAttributes();

    if (i >= m_upscalePasses.size())
      break;

    const std::size_t correspIndex = m_downscaleBuffers.size() - i - 2;

    m_upscalePasses[correspIndex]->resizeWriteBuffers(width, height);

    m_upscalePasses[correspIndex]->getProgram().setAttribute(invBufferSize, "uniInvBufferSize");
    m_upscalePasses[correspIndex]->getProgram().sendAttributes();
  }
}

float BloomRenderProcess::recoverElapsedTime() const {
  float time = m_thresholdPass->recoverElapsedTime() + m_finalPass->recoverElapsedTime();

  for (const RenderPass* pass : m_downscalePasses)
    time += pass->recoverElapsedTime();

  for (const RenderPass* pass : m_upscalePasses)
    time += pass->recoverElapsedTime();

  return time;
}

void BloomRenderProcess::setInputColorBuffer(Texture2DPtr colorBuffer) {
  resizeBuffers(colorBuffer->getWidth(), colorBuffer->getHeight());

  m_thresholdPass->addReadTexture(colorBuffer, "uniColorBuffer");
  m_finalPass->addReadTexture(std::move(colorBuffer), "uniOriginalColorBuffer");
}

void BloomRenderProcess::setOutputBuffer(Texture2DPtr outputBuffer) {
  // TODO: if the input buffer has a floating-point data type, the output should too
  m_finalPass->addWriteColorTexture(std::move(outputBuffer), 0);

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3))
    Renderer::setLabel(RenderObjectType::FRAMEBUFFER, m_finalPass->getFramebuffer().getIndex(), "Bloom final pass framebuffer");
#endif
}

void BloomRenderProcess::setThresholdValue(float threshold) const {
  m_thresholdPass->getProgram().setAttribute(threshold, "uniThreshold");
  m_thresholdPass->getProgram().sendAttributes();
}

} // namespace Raz
