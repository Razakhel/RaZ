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
  m_geometryPass.resizeWriteBuffers(width, height);

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
  assert("Error: The camera referenced by the render system needs a transform component." && renderSystem.m_cameraEntity->hasComponent<Transform>());

  Renderer::clear(MaskType::COLOR | MaskType::DEPTH | MaskType::STENCIL);

  const Framebuffer& geometryFramebuffer = m_geometryPass.m_writeFramebuffer;

#if !defined(USE_OPENGL_ES)
  m_geometryPass.m_timer.start();

#if defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::pushDebugGroup("Geometry pass");
#endif
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

#if !defined(USE_OPENGL_ES)
  m_geometryPass.m_timer.stop();

#if defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::popDebugGroup();
#endif
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

  for (const RenderPass* parentPass : renderPass.m_parents)
    execute(*parentPass);

  renderPass.execute();

  m_executedPasses.emplace(&renderPass);
}

} // namespace Raz
