#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace Raz {

RenderSystem::RenderSystem(unsigned int windowWidth, unsigned int windowHeight,
                           const std::string& windowTitle) : m_window(windowWidth, windowHeight, windowTitle) {
  m_camera.addComponent<Camera>(windowWidth, windowHeight);
  m_camera.addComponent<Transform>();

  m_acceptedComponents.setBit(Component::getId<Mesh>());
  m_acceptedComponents.setBit(Component::getId<Light>());

  // Creating the SSR program & framebuffer
  const auto ssrIndex = static_cast<std::size_t>(RenderPass::SSR);
  m_programs[ssrIndex].setFragmentShader(FragmentShader("../../shaders/ssr.glsl"));
  Framebuffer::assignVertexShader(m_programs[ssrIndex]);

  // Initializing all our shader programs & framebuffers
  for (std::size_t programIndex = 1; programIndex < m_programs.size(); ++programIndex) {
    Framebuffer& framebuffer = m_framebuffers[programIndex];

    framebuffer.resize(windowWidth, windowHeight);
    framebuffer.initBuffers(m_programs[programIndex]);
    framebuffer.mapBuffers();

    m_cameraUbo.bindUniformBlock(m_programs[programIndex], "uboCameraMatrices", 0);
  }

  updateShaders();

  m_cameraUbo.bindBufferBase(0);
}

void RenderSystem::setProgram(RenderPass renderPass, ShaderProgram&& program) {
  const auto programIndex = static_cast<std::size_t>(renderPass);
  ShaderProgram& passProgram = m_programs[programIndex];
  passProgram = std::move(program);

  if (renderPass != RenderPass::GEOMETRY)
    Framebuffer::assignVertexShader(passProgram);

  passProgram.updateShaders();
  enableRenderPass(programIndex);
}

void RenderSystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);

  if (entity->hasComponent<Mesh>())
    entity->getComponent<Mesh>().load(m_programs.front());

  if (entity->hasComponent<Light>())
    updateLights();
}

bool RenderSystem::update(float deltaTime) {
  m_programs.front().use();

  auto& camera       = m_camera.getComponent<Camera>();
  auto& camTransform = m_camera.getComponent<Transform>();

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

    sendCameraMatrices(viewProjMat);

    camTransform.setUpdated(false);
  } else {
    viewProjMat = camera.getViewMatrix() * camera.getProjectionMatrix();
  }

  const auto ssrIndex = static_cast<std::size_t>(RenderPass::SSR);

  if (m_enabledPasses[ssrIndex])
    m_framebuffers[ssrIndex].bind();

  for (auto& entity : m_entities) {
    if (entity->isEnabled()) {
      if (entity->hasComponent<Mesh>() && entity->hasComponent<Transform>()) {
        const Mat4f modelMat = entity->getComponent<Transform>().computeTransformMatrix();

        m_programs.front().sendUniform("uniModelMatrix", modelMat);
        m_programs.front().sendUniform("uniMvpMatrix", modelMat * viewProjMat);

        entity->getComponent<Mesh>().draw(m_programs.front());
      }
    }
  }

  if (m_enabledPasses[ssrIndex]) {
    m_framebuffers[ssrIndex].unbind();
    m_framebuffers[ssrIndex].display(m_programs[ssrIndex]);
  }

  if (m_cubemap)
    m_cubemap->draw(camera);

  return m_window.run(deltaTime);
}

void RenderSystem::sendCameraMatrices(const Mat4f& viewProjMat) const {
  const auto& camera = m_camera.getComponent<Camera>();

  m_cameraUbo.bind();
  sendViewMatrix(camera.getViewMatrix());
  sendInverseViewMatrix(camera.getInverseViewMatrix());
  sendProjectionMatrix(camera.getProjectionMatrix());
  sendInverseProjectionMatrix(camera.getInverseProjectionMatrix());
  sendViewProjectionMatrix(viewProjMat);
  sendCameraPosition(m_camera.getComponent<Transform>().getPosition());
}

void RenderSystem::sendCameraMatrices() const {
  const auto& camera = m_camera.getComponent<Camera>();
  sendCameraMatrices(camera.getViewMatrix() * camera.getProjectionMatrix());
}

void RenderSystem::updateLight(const Entity* entity, std::size_t lightIndex) const {
  m_programs.front().use();

  const std::string strBase = "uniLights[" + std::to_string(lightIndex) + "].";

  const std::string posStr    = strBase + "position";
  const std::string colorStr  = strBase + "color";
  const std::string energyStr = strBase + "energy";
  const std::string angleStr  = strBase + "angle";

  const auto& lightComp = entity->getComponent<Light>();
  Vec4f homogeneousPos(entity->getComponent<Transform>().getPosition(), 1.f);

  if (lightComp.getType() == LightType::DIRECTIONAL) {
    homogeneousPos[3] = 0.f;
    m_programs.front().sendUniform(strBase + "direction", lightComp.getDirection());
  }

  m_programs.front().sendUniform(posStr,    homogeneousPos);
  m_programs.front().sendUniform(colorStr,  lightComp.getColor());
  m_programs.front().sendUniform(energyStr, lightComp.getEnergy());
  m_programs.front().sendUniform(angleStr,  lightComp.getAngle());
}

void RenderSystem::updateLights() const {
  std::size_t lightCount = 0;

  for (const auto& entity : m_entities) {
    if (entity->hasComponent<Light>()) {
      updateLight(entity, lightCount);
      ++lightCount;
    }
  }

  m_programs.front().sendUniform("uniLightCount", lightCount);
}

void RenderSystem::updateShaders() const {
  for (std::size_t programIndex = 0; programIndex < m_programs.size(); ++programIndex) {
    if (m_enabledPasses[programIndex])
      m_programs[programIndex].updateShaders();
  }

  sendCameraMatrices();
  updateLights();

  for (const auto& entity : m_entities) {
    if (entity->hasComponent<Mesh>())
      entity->getComponent<Mesh>().load(m_programs.front());
  }
}

void RenderSystem::saveToImage(const std::string& fileName) const {
  Image img(m_window.getWidth(), m_window.getHeight());
  glReadPixels(0, 0, m_window.getWidth(), m_window.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, img.getDataPtr());

  img.save(fileName, true);
}

} // namespace Raz
