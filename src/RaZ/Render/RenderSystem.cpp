#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace Raz {

RenderSystem::RenderSystem(unsigned int windowWidth, unsigned int windowHeight,
                           const std::string& windowTitle) : m_window(windowWidth, windowHeight, windowTitle) {
  m_camera.addComponent<Camera>(windowWidth, windowHeight);
  m_camera.addComponent<Transform>();

  m_acceptedComponents.setBit(Component::getId<Mesh>());
}

void RenderSystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);

  if (entity->hasComponent<Mesh>())
    entity->getComponent<Mesh>().load(m_program);
}

void RenderSystem::update(float deltaTime) {
  auto& camera       = m_camera.getComponent<Camera>();
  auto& camTransform = m_camera.getComponent<Transform>();

  Mat4f viewProjMat;

  if (camTransform.hasUpdated()) {
    const Mat4f& viewMat    = camera.computeViewMatrix(camTransform.getRotation().inverse(),
                                                       camTransform.computeTranslationMatrix(true));
    const Mat4f& invViewMat = camera.computeInverseViewMatrix();
    viewProjMat = camera.getProjectionMatrix() * viewMat;

    m_cameraUbo.bind();
    sendViewMatrix(viewMat);
    sendInverseViewMatrix(invViewMat);
    sendProjectionMatrix(camera.getProjectionMatrix());
    sendInverseProjectionMatrix(camera.getInverseProjectionMatrix());
    sendViewProjectionMatrix(viewProjMat);
    sendCameraPosition(camTransform.getPosition());

    camTransform.setUpdated(false);
  } else {
    viewProjMat = camera.getProjectionMatrix() * camera.getViewMatrix();
  }

  for (auto& entity : m_entities) {
    if (entity->isEnabled()) {
      if (entity->hasComponent<Mesh>() && entity->hasComponent<Transform>()) {
        const auto modelMat = entity->getComponent<Transform>().computeTransformMatrix();

        m_program.sendUniform("uniModelMatrix", modelMat);
        m_program.sendUniform("uniMvpMatrix", viewProjMat * modelMat);

        entity->getComponent<Mesh>().draw(m_program);
      }
    }
  }

  m_window.run(deltaTime);
}

} // namespace Raz
