#include "RaZ/Render/EnhancedKawaseBlurRenderProcess.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <string_view>

namespace Raz {

namespace {

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

    fragColor = vec4(color, 1.0); // TODO: divide by 2?
  }
)";

constexpr std::string_view finalSource = R"(
  in vec2 fragTexcoords;

  uniform sampler2D uniFinalUpscaledBuffer;
  uniform float uniInvPassCount;

  layout(location = 0) out vec4 fragColor;

  void main() {
    // TODO: divide the result by the amount of passes (https://www.froyok.fr/blog/2021-12-ue4-custom-bloom/#upsample_shader)
    fragColor = vec4(texture(uniFinalUpscaledBuffer, fragTexcoords).rgb * uniInvPassCount, 1.0);
  }
)";

} // namespace

EnhancedKawaseBlurRenderProcess::EnhancedKawaseBlurRenderProcess(RenderGraph& renderGraph,
                                                                 unsigned int frameWidth,
                                                                 unsigned int frameHeight,
                                                                 unsigned int strength) : RenderProcess(renderGraph) {
  // Based on Call of Duty: Advanced Warfare's modified Kawase blur:
  //  - https://www.froyok.fr/blog/2021-12-ue4-custom-bloom/#the_call_of_duty_method
  //  - http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare (slides 148-162)

  m_downscalePasses.resize(strength);
  m_downscaleBuffers.resize(strength);

  m_upscalePasses.resize(strength - 1);
  m_upscaleBuffers.resize(strength - 1);

  unsigned int bufferWidth  = frameWidth;
  unsigned int bufferHeight = frameHeight;

  for (std::size_t passIndex = 0; passIndex < strength; ++passIndex) {
    bufferWidth  = std::max(1u, bufferWidth / 2);
    bufferHeight = std::max(1u, bufferHeight / 2);

    const Vec2f invBufferSize(1.f / static_cast<float>(bufferWidth), 1.f / static_cast<float>(bufferHeight));

    /////////////////
    // Downscaling //
    /////////////////

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

    std::string idStr = std::to_string(passIndex + 1);

    RenderPass& downscalePass = renderGraph.addNode(FragmentShader::loadFromSource(downscaleSource), "Enhanced Kawase blur downscale #" + idStr);

    downscalePass.getProgram().use();
    downscalePass.getProgram().sendUniform("uniInvBufferSize", invBufferSize);

    const auto downscaledBuffer = Texture2D::create(bufferWidth, bufferHeight, TextureColorspace::RGB, TextureDataType::FLOAT16);
    downscalePass.addWriteColorTexture(downscaledBuffer, 0);

    m_downscalePasses[passIndex]  = &downscalePass;
    m_downscaleBuffers[passIndex] = downscaledBuffer;

    if (passIndex > 0) {
      downscalePass.addReadTexture(m_downscaleBuffers[passIndex - 1].lock(), "uniPrevDownscaledBuffer");
      downscalePass.addParents(*m_downscalePasses[passIndex - 1]);
    }

#if !defined(USE_OPENGL_ES)
    if (Renderer::checkVersion(4, 3)) {
      Renderer::setLabel(RenderObjectType::PROGRAM, downscalePass.getProgram().getIndex(), "Enhanced Kawase blur downscale program #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, downscalePass.getProgram().getVertexShader().getIndex(), "Enhanced Kawase blur downscale vertex shader #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, downscalePass.getProgram().getFragmentShader().getIndex(), "Enhanced Kawase blur downscale fragment shader #" + idStr);
      Renderer::setLabel(RenderObjectType::FRAMEBUFFER, downscalePass.getFramebuffer().getIndex(), "Enhanced Kawase blur downscale framebuffer #" + idStr);
      Renderer::setLabel(RenderObjectType::TEXTURE, downscaledBuffer->getIndex(), "Enhanced Kawase blur downscale buffer #" + idStr);
    }
#endif

    if (passIndex == strength - 1)
      break; // There is always one less upscale pass

    ///////////////
    // Upscaling //
    ///////////////

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

    idStr = std::to_string(strength - passIndex - 1);

    RenderPass& upscalePass = renderGraph.addNode(FragmentShader::loadFromSource(upscaleSource), "Enhanced Kawase blur upscale #" + idStr);

    upscalePass.getProgram().use();
    upscalePass.getProgram().sendUniform("uniInvBufferSize", invBufferSize);

    upscalePass.addReadTexture(m_downscaleBuffers[passIndex].lock(), "uniDownscaledBuffer");

    const auto upscaledBuffer = Texture2D::create(bufferWidth, bufferHeight, TextureColorspace::RGB, TextureDataType::FLOAT16);
    upscalePass.addWriteColorTexture(upscaledBuffer, 0);

    m_upscalePasses[passIndex]  = &upscalePass;
    m_upscaleBuffers[passIndex] = upscaledBuffer;

#if !defined(USE_OPENGL_ES)
    if (Renderer::checkVersion(4, 3)) {
      Renderer::setLabel(RenderObjectType::PROGRAM, upscalePass.getProgram().getIndex(), "Enhanced Kawase blur upscale program #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, upscalePass.getProgram().getVertexShader().getIndex(), "Enhanced Kawase blur upscale vertex shader #" + idStr);
      Renderer::setLabel(RenderObjectType::SHADER, upscalePass.getProgram().getFragmentShader().getIndex(), "Enhanced Kawase blur upscale fragment shader #" + idStr);
      Renderer::setLabel(RenderObjectType::FRAMEBUFFER, upscalePass.getFramebuffer().getIndex(), "Enhanced Kawase blur upscale framebuffer #" + idStr);
      Renderer::setLabel(RenderObjectType::TEXTURE, upscaledBuffer->getIndex(), "Enhanced Kawase blur upscale buffer #" + idStr);
    }
#endif
  }

  // Mapping upscale read textures & parent passes
  for (std::size_t upscalePassIndex = 0; upscalePassIndex < strength - 1; ++upscalePassIndex) {
    RenderPass& upscalePass = *m_upscalePasses[upscalePassIndex];

    upscalePass.addReadTexture((upscalePassIndex == strength - 2 ? m_downscaleBuffers.back() : m_upscaleBuffers[upscalePassIndex + 1]).lock(), "uniPrevUpscaledBuffer");

    // Although each upscaling pass is technically dependent on the matching downscaling one, the render graph only needs
    //  direct dependencies, that is, passes that can be executed anytime after their parents have been. In this case, we need
    //  to execute each one sequentially whenever the previous upscaling pass has finished anyway. Hence, although we could, we
    //  do not set any dependency between upscaling & downscaling passes except for the last downscale/first upscale ones
    upscalePass.addParents((upscalePassIndex == strength - 2 ? *m_downscalePasses.back() : *m_upscalePasses[upscalePassIndex + 1]));
  }

  ////////////////
  // Final pass //
  ////////////////

  m_finalPass = &renderGraph.addNode(FragmentShader::loadFromSource(finalSource), "Enhanced Kawase blur final pass");

  m_finalPass->addParents(*m_upscalePasses.front());
  m_finalPass->addReadTexture(m_upscaleBuffers.front().lock(), "uniFinalUpscaledBuffer");

#if !defined(USE_OPENGL_ES)
  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::PROGRAM, m_finalPass->getProgram().getIndex(), "Enhanced Kawase blur final pass program");
    Renderer::setLabel(RenderObjectType::SHADER, m_finalPass->getProgram().getVertexShader().getIndex(), "Enhanced Kawase blur final pass vertex shader");
    Renderer::setLabel(RenderObjectType::SHADER, m_finalPass->getProgram().getFragmentShader().getIndex(), "Enhanced Kawase blur final pass fragment shader");
    // Adding a label to an empty framebuffer produces an OpenGL error, but the label is properly set nonetheless
    // This could be done in setOutputBuffer() to make sure it isn't empty, but it would be done for each assigned buffer in that case
    Renderer::setLabel(RenderObjectType::FRAMEBUFFER, m_finalPass->getFramebuffer().getIndex(), "Enhanced Kawase blur final pass framebuffer");
  }
#endif

  setStrength(strength);

  // Validating the render graph
  if (!renderGraph.isValid())
    throw std::runtime_error("Error: The bloom process is invalid");
}

bool EnhancedKawaseBlurRenderProcess::isEnabled() const noexcept {
  return m_downscalePasses.front()->isEnabled();
}

void EnhancedKawaseBlurRenderProcess::setState(bool enabled) {
  for (RenderPass* downscalePass : m_downscalePasses)
    downscalePass->enable(enabled);

  for (RenderPass* upscalePass : m_upscalePasses)
    upscalePass->enable(enabled);

  m_finalPass->enable(enabled);
}

void EnhancedKawaseBlurRenderProcess::addParent(RenderPass& parentPass) {
  m_downscalePasses.front()->addParents(parentPass);
}

void EnhancedKawaseBlurRenderProcess::addParent(RenderProcess& parentProcess) {
  parentProcess.addChild(*m_downscalePasses.front());
}

void EnhancedKawaseBlurRenderProcess::addChild(RenderPass& childPass) {
  m_finalPass->addChildren(childPass); // TODO: should be the front & the upscale passes should be inserted backwards so that the front one is always valid
}

void EnhancedKawaseBlurRenderProcess::addChild(RenderProcess& childProcess) {
  childProcess.addParent(*m_finalPass);
}

void EnhancedKawaseBlurRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  for (std::size_t bufferIndex = 0; bufferIndex < m_downscaleBuffers.size(); ++bufferIndex) {
    width  = std::max(1u, width / 2);
    height = std::max(1u, height / 2);

    const Vec2f invBufferSize(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));

    m_downscaleBuffers[bufferIndex].lock()->resize(width, height);

    m_downscalePasses[bufferIndex]->getProgram().use();
    m_downscalePasses[bufferIndex]->getProgram().sendUniform("uniInvBufferSize", invBufferSize);

    if (bufferIndex < m_upscaleBuffers.size()) {
      m_upscaleBuffers[bufferIndex].lock()->resize(width, height);

      m_upscalePasses[bufferIndex]->getProgram().use();
      m_upscalePasses[bufferIndex]->getProgram().sendUniform("uniInvBufferSize", invBufferSize);
    }
  }
}

void EnhancedKawaseBlurRenderProcess::setInputBuffer(Texture2DPtr inputBuffer) {
  m_downscalePasses.front()->addReadTexture(std::move(inputBuffer), "uniPrevDownscaledBuffer");
}

void EnhancedKawaseBlurRenderProcess::setOutputBuffer(Texture2DPtr outputBuffer) {
  m_finalPass->addWriteColorTexture(std::move(outputBuffer), 0);
}

void EnhancedKawaseBlurRenderProcess::setStrength(unsigned int strength) {
  if (strength < 2) {
    Logger::warn("[EnhancedKawaseBlurRenderProcess] The enhanced Kawase blur process cannot have less than 2 passes; setting it to 2.");
    strength = 2;
  }

  // TODO: (re)make passes

  m_finalPass->getProgram().use();
  m_finalPass->getProgram().sendUniform("uniInvPassCount", 1.f / static_cast<float>(strength));
}

} // namespace Raz
