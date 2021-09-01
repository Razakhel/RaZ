#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
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

const Texture& RenderGraph::addTextureBuffer(unsigned int width, unsigned int height, int bindingIndex, ImageColorspace colorspace) {
  return *m_buffers.emplace_back(std::make_unique<Texture>(width, height, bindingIndex, colorspace, false));
}

void RenderGraph::resizeViewport(unsigned int width, unsigned int height) {
  for (std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->resizeWriteBuffers(width, height);
}

void RenderGraph::updateShaders() const {
  for (const std::unique_ptr<RenderPass>& renderPass : m_nodes)
    renderPass->getProgram().updateShaders();
}

void RenderGraph::execute(RenderSystem& renderSystem) const {
  assert("Error: The render system needs a camera for the render graph to be executed." && (renderSystem.m_cameraEntity != nullptr));

  m_geometryPass.getProgram().use();

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
      camera.computeViewMatrix(camTransform.computeTranslationMatrix(true),
                               camTransform.getRotation().inverse());
    }

    camera.computeInverseViewMatrix();

    const Mat4f& viewMat = camera.getViewMatrix();
    viewProjMat = viewMat * camera.getProjectionMatrix();

    renderSystem.sendCameraMatrices(viewProjMat);

    camTransform.setUpdated(false);
  } else {
    viewProjMat = camera.getViewMatrix() * camera.getProjectionMatrix();
  }

  for (const Entity* entity : renderSystem.m_entities) {
    if (entity->isEnabled()) {
      if (entity->hasComponent<Mesh>() && entity->hasComponent<Transform>()) {
        const Mat4f modelMat = entity->getComponent<Transform>().computeTransformMatrix();

        const ShaderProgram& geometryProgram = m_geometryPass.getProgram();

#if !defined(RAZ_USE_VULKAN)
        geometryProgram.sendUniform("uniModelMatrix", modelMat);
        geometryProgram.sendUniform("uniMvpMatrix", modelMat * viewProjMat);
#endif

        entity->getComponent<Mesh>().draw(geometryProgram);
      }
    }
  }

  if (renderSystem.hasCubemap())
    renderSystem.getCubemap().draw(camera);

  geometryFramebuffer.unbind();

#if !defined(RAZ_USE_VULKAN)
  for (const RenderPass* renderPass : m_geometryPass.getChildren())
    renderPass->execute(geometryFramebuffer);
#endif
}

} // namespace Raz
