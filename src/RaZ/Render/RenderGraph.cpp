#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace Raz {

bool RenderGraph::isValid() const {
  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes) {
    if (!renderPass->isValid())
      return false;
  }

  return true;
}

void RenderGraph::resizeViewport(unsigned int width, unsigned int height) {
  for (std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->resizeWriteBuffers(width, height); // TODO: resizing all write buffers will only work if they have all been created with equal dimensions

  for (std::unique_ptr<RenderProcess>& renderProcess : m_renderProcesses)
    renderProcess->resizeBuffers(width, height);
}

void RenderGraph::updateShaders() const {
  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->getProgram().updateShaders();
}

void RenderGraph::execute(RenderSystem& renderSystem) {
  assert("Error: The render system needs a camera for the render graph to be executed." && (renderSystem.m_cameraEntity != nullptr));

  Renderer::clear(MaskType::COLOR | MaskType::DEPTH | MaskType::STENCIL);

  const Framebuffer& geometryFramebuffer = m_geometryPass.getFramebuffer();

#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::pushDebugGroup("Geometry pass");
#endif

  if (!geometryFramebuffer.isEmpty())
    geometryFramebuffer.bind();

  auto& camera       = renderSystem.m_cameraEntity->getComponent<Camera>();
  auto& camTransform = renderSystem.m_cameraEntity->getComponent<Transform>();

  renderSystem.m_cameraUbo.bind();

  if (camTransform.hasUpdated()) {
    if (camera.getCameraType() == CameraType::LOOK_AT)
      camera.computeLookAt(camTransform.getPosition());
    else
      camera.computeViewMatrix(camTransform);

    camera.computeInverseViewMatrix();

    renderSystem.sendViewMatrix(camera.getViewMatrix());
    renderSystem.sendInverseViewMatrix(camera.getInverseViewMatrix());
    renderSystem.sendCameraPosition(camTransform.getPosition());

    camTransform.setUpdated(false);
  }

  renderSystem.sendProjectionMatrix(camera.getProjectionMatrix());
  renderSystem.sendInverseProjectionMatrix(camera.getInverseProjectionMatrix());
  renderSystem.sendViewProjectionMatrix(camera.getProjectionMatrix() * camera.getViewMatrix());

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

  if (renderSystem.hasCubemap())
    renderSystem.getCubemap().draw();

  geometryFramebuffer.unbind();

#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::popDebugGroup();
#endif

  m_executedPasses.reserve(m_nodes.size() + 1);
  m_executedPasses.emplace(&m_geometryPass);

  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes)
    execute(*renderPass);

  m_executedPasses.clear();
}

void RenderGraph::execute(const RenderPass& renderPass) {
  if (m_executedPasses.find(&renderPass) != m_executedPasses.cend())
    return;

  for (const RenderPass* parentPass : renderPass.getParents())
    execute(*parentPass);

#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::pushDebugGroup("Render pass #" + std::to_string(m_executedPasses.size()));
#endif

  renderPass.execute();

#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::popDebugGroup();
#endif

  m_executedPasses.emplace(&renderPass);
}

} // namespace Raz
