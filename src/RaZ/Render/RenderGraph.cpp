#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace Raz {

bool RenderGraph::isValid() const {
  if (!m_geometryPass.isValid())
    return false;

  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes) {
    if (!renderPass->isValid())
      return false;
  }

  return true;
}

void RenderGraph::resizeViewport(unsigned int width, unsigned int height) {
  for (std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->resizeWriteBuffers(width, height);
}

void RenderGraph::updateShaders() const {
  m_geometryPass.getProgram().updateShaders();

  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->getProgram().updateShaders();
}

void RenderGraph::execute(RenderSystem& renderSystem) const {
  assert("Error: The render system needs a camera for the render graph to be executed." && (renderSystem.m_cameraEntity != nullptr));

  const Framebuffer& geometryFramebuffer = m_geometryPass.getFramebuffer();

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

  // Binding textures marks the pass' program as used
  m_geometryPass.bindTextures();

  const RenderShaderProgram& geometryProgram = m_geometryPass.getProgram();

  for (const Entity* entity : renderSystem.m_entities) {
    if (!entity->isEnabled() || !entity->hasComponent<MeshRenderer>() || !entity->hasComponent<Transform>())
      continue;

    const auto& meshRenderer = entity->getComponent<MeshRenderer>();

    if (!meshRenderer.isEnabled())
      continue;

    //if (meshRenderer.getInstanceCount() > 1)
      meshRenderer.updateInstancesMatrices();

    renderSystem.m_modelUbo.sendData(entity->getComponent<Transform>().computeTransformMatrix(), 0);
    meshRenderer.draw(geometryProgram);
  }

  if (renderSystem.hasCubemap())
    renderSystem.getCubemap().draw();

  geometryFramebuffer.unbind();

  for (const RenderPass* renderPass : m_geometryPass.getChildren())
    renderPass->execute(geometryFramebuffer);
}

} // namespace Raz
