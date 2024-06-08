#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/RenderSystem.hpp"

#include "tracy/Tracy.hpp"
#include "GL/glew.h" // Needed by TracyOpenGL.hpp
#include "tracy/TracyOpenGL.hpp"

namespace Raz {

namespace {

constexpr std::string_view gammaCorrectionSource = {
#include "gamma_correction.frag.embed"
};

} // namespace

RenderGraph::RenderGraph() : m_gammaCorrectionPass(FragmentShader::loadFromSource(gammaCorrectionSource), "Gamma correction") {
  setGammaStrength(2.2f);
  m_gammaCorrectionPass.disable(); // The pass is disabled at first, a final buffer must be given to be corrected
}

bool RenderGraph::isValid() const {
  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes) {
    if (!renderPass->isValid())
      return false;
  }

  return true;
}

void RenderGraph::setFinalBuffer(Texture2DPtr texture) {
  m_gammaCorrectionPass.addReadTexture(std::move(texture), "uniBuffer");
  m_gammaCorrectionPass.enable();
}

void RenderGraph::setGammaStrength(float gammaStrength) {
  RenderShaderProgram& gammaProgram = m_gammaCorrectionPass.getProgram();
  gammaProgram.setAttribute(1.f / gammaStrength, "uniInvGamma");
  gammaProgram.sendAttributes();
}

void RenderGraph::resizeViewport(unsigned int width, unsigned int height) {
  ZoneScopedN("RenderGraph::resizeViewport");

  m_geometryPass.resizeWriteBuffers(width, height);

  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->resizeWriteBuffers(width, height); // TODO: resizing all write buffers will only work if they have all been created with equal dimensions

  for (const std::unique_ptr<RenderProcess>& renderProcess : m_renderProcesses)
    renderProcess->resizeBuffers(width, height);
}

void RenderGraph::updateShaders() const {
  ZoneScopedN("RenderGraph::updateShaders");

  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->getProgram().updateShaders();
}

void RenderGraph::execute(RenderSystem& renderSystem) {
  assert("Error: The render system needs a camera for the render graph to be executed." && (renderSystem.m_cameraEntity != nullptr));
  assert("Error: The camera referenced by the render system needs a transform component." && renderSystem.m_cameraEntity->hasComponent<Transform>());

  ZoneScopedN("RenderGraph::execute");

  {
    ZoneScopedN("Renderer::clear");
    Renderer::clear(MaskType::COLOR | MaskType::DEPTH | MaskType::STENCIL);
  }

  executeGeometryPass(renderSystem);

  m_executedPasses.reserve(m_nodes.size() + 1);
  m_executedPasses.emplace(&m_geometryPass);

  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes)
    executePass(*renderPass);

  m_executedPasses.clear();

  // TODO: the last pass' output buffer must be bound to be read by the gamma correction pass
  m_gammaCorrectionPass.execute();
}

void RenderGraph::executeGeometryPass(RenderSystem& renderSystem) const {
  ZoneScopedN("RenderGraph::executeGeometryPass");
  TracyGpuZone("Geometry pass")

#if !defined(USE_OPENGL_ES)
  m_geometryPass.m_timer.start();

#if defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::pushDebugGroup("Geometry pass");
#endif
#endif

  const Framebuffer& geometryFramebuffer = m_geometryPass.m_writeFramebuffer;

  if (!geometryFramebuffer.isEmpty())
    geometryFramebuffer.bind();

  if (renderSystem.hasCubemap())
    renderSystem.getCubemap().draw();

  renderSystem.m_modelUbo.bind();

  for (const Entity* entity : renderSystem.m_entities) {
    if (!entity->isEnabled() || !entity->hasComponent<MeshRenderer>() || !entity->hasComponent<Transform>())
      continue;

    const auto& meshRenderer = entity->getComponent<MeshRenderer>();

    if (!meshRenderer.isEnabled())
      continue;

    renderSystem.m_modelUbo.sendData(entity->getComponent<Transform>().computeTransformMatrix(), 0);
    meshRenderer.draw();
  }

  geometryFramebuffer.unbind();

#if !defined(USE_OPENGL_ES)
  m_geometryPass.m_timer.stop();

#if defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::popDebugGroup();
#endif
#endif
}

void RenderGraph::executePass(const RenderPass& renderPass) {
  if (m_executedPasses.find(&renderPass) != m_executedPasses.cend())
    return;

  for (const RenderPass* parentPass : renderPass.m_parents)
    executePass(*parentPass);

  renderPass.execute();

  m_executedPasses.emplace(&renderPass);
}

} // namespace Raz
