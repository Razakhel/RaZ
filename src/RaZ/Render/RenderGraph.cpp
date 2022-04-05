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

const Texture& RenderGraph::addTextureBuffer(unsigned int width, unsigned int height, ImageColorspace colorspace) {
  return addTextureBuffer(width, height, colorspace, (colorspace == ImageColorspace::DEPTH ? ImageDataType::FLOAT : ImageDataType::BYTE));
}

const Texture& RenderGraph::addTextureBuffer(unsigned int width, unsigned int height, ImageColorspace colorspace, ImageDataType dataType) {
  return *m_buffers.emplace_back(std::make_unique<Texture>(width, height, static_cast<int>(m_buffers.size()), colorspace, dataType));
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

  Mat4f viewProjMat;

  if (camTransform.hasUpdated()) {
    if (camera.getCameraType() == CameraType::LOOK_AT) {
      camera.computeLookAt(camTransform.getPosition());
    } else {
      camera.computeViewMatrix(camTransform);
    }

    camera.computeInverseViewMatrix();
    viewProjMat = camera.getProjectionMatrix() * camera.getViewMatrix();

    renderSystem.sendCameraMatrices(viewProjMat);

    camTransform.setUpdated(false);
  } else {
    viewProjMat = camera.getProjectionMatrix() * camera.getViewMatrix();
  }

  // Binding textures marks the pass' program as used
  m_geometryPass.bindTextures();

  const RenderShaderProgram& geometryProgram = m_geometryPass.getProgram();

  for (const Entity* entity : renderSystem.m_entities) {
    if (!entity->isEnabled() || !entity->hasComponent<MeshRenderer>() || !entity->hasComponent<Transform>())
      continue;

    const auto& meshRenderer = entity->getComponent<MeshRenderer>();

    if (!meshRenderer.isEnabled())
      continue;

    const Mat4f modelMat = entity->getComponent<Transform>().computeTransformMatrix();

    geometryProgram.sendUniform("uniModelMatrix", modelMat);
    geometryProgram.sendUniform("uniMvpMatrix", viewProjMat * modelMat);

    meshRenderer.draw(geometryProgram);
  }

  if (renderSystem.hasCubemap())
    renderSystem.getCubemap().draw();

  geometryFramebuffer.unbind();

  for (const RenderPass* renderPass : m_geometryPass.getChildren())
    renderPass->execute(geometryFramebuffer);
}

} // namespace Raz
