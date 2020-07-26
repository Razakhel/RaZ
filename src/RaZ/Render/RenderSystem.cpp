#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace Raz {

const GeometryPass& RenderSystem::getGeometryPass() const {
  const RenderPassPtr& renderPass = m_renderPasses[static_cast<std::size_t>(RenderPassType::GEOMETRY)];

  assert("Error: Geometry pass isn't enabled." && renderPass);

  return static_cast<const GeometryPass&>(*renderPass);
}

const SSRPass& RenderSystem::getSSRPass() const {
  const RenderPassPtr& renderPass = m_renderPasses[static_cast<std::size_t>(RenderPassType::SSR)];

  assert("Error: SSR pass isn't enabled." && renderPass);

  return static_cast<const SSRPass&>(*renderPass);
}

void RenderSystem::resizeViewport(unsigned int width, unsigned int height) {
  m_sceneWidth  = width;
  m_sceneHeight = height;

  Renderer::resizeViewport(0, 0, m_sceneWidth, m_sceneHeight);

  if (m_window)
    m_window->resize(m_sceneWidth, m_sceneHeight);

  if (m_cameraEntity)
    m_cameraEntity->getComponent<Camera>().resizeViewport(m_sceneWidth, m_sceneHeight);

  for (RenderPassPtr& renderPass : m_renderPasses) {
    if (renderPass)
      renderPass->resize(width, height);
  }
}

void RenderSystem::enableGeometryPass(VertexShader vertShader, FragmentShader fragShader) {
  m_renderPasses[static_cast<std::size_t>(RenderPassType::GEOMETRY)] = std::make_unique<GeometryPass>(m_sceneWidth, m_sceneHeight,
                                                                                                      std::move(vertShader), std::move(fragShader));
}

void RenderSystem::enableSSRPass(FragmentShader fragShader) {
  m_renderPasses[static_cast<std::size_t>(RenderPassType::SSR)] = std::make_unique<SSRPass>(m_sceneWidth, m_sceneHeight, std::move(fragShader));
}

bool RenderSystem::update(float deltaTime) {
  assert("Error: Geometry pass must be enabled for the RenderSystem to be updated." && m_renderPasses.front());

  m_renderPasses.front()->getProgram().use();

  auto& camera       = m_cameraEntity->getComponent<Camera>();
  auto& camTransform = m_cameraEntity->getComponent<Transform>();

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

  for (auto& entity : m_entities) {
    if (entity->isEnabled()) {
      if (entity->hasComponent<Mesh>() && entity->hasComponent<Transform>()) {
        const Mat4f modelMat = entity->getComponent<Transform>().computeTransformMatrix();

        const ShaderProgram& geometryProgram = m_renderPasses.front()->getProgram();

        geometryProgram.sendUniform("uniModelMatrix", modelMat);
        geometryProgram.sendUniform("uniMvpMatrix", modelMat * viewProjMat);

        entity->getComponent<Mesh>().draw(geometryProgram);
      }
    }
  }

  if (m_cubemap)
    m_cubemap->draw(camera);

#if defined(RAZ_CONFIG_DEBUG) && !defined(SKIP_RENDERER_ERRORS)
  Renderer::printErrors();
#endif

  if (m_window)
    return m_window->run(deltaTime);

  return true;
}

void RenderSystem::sendCameraMatrices(const Mat4f& viewProjMat) const {
  assert("Error: A camera must be given to a RenderSystem to send its matrices." && (m_cameraEntity != nullptr));

  const auto& camera = m_cameraEntity->getComponent<Camera>();

  m_cameraUbo.bind();
  sendViewMatrix(camera.getViewMatrix());
  sendInverseViewMatrix(camera.getInverseViewMatrix());
  sendProjectionMatrix(camera.getProjectionMatrix());
  sendInverseProjectionMatrix(camera.getInverseProjectionMatrix());
  sendViewProjectionMatrix(viewProjMat);
  sendCameraPosition(m_cameraEntity->getComponent<Transform>().getPosition());
}

void RenderSystem::sendCameraMatrices() const {
  assert("Error: A camera must be given to a RenderSystem to send its matrices." && (m_cameraEntity != nullptr));

  const auto& camera = m_cameraEntity->getComponent<Camera>();
  sendCameraMatrices(camera.getViewMatrix() * camera.getProjectionMatrix());
}

void RenderSystem::updateLight(const Entity* entity, std::size_t lightIndex) const {
  assert("Error: Geometry pass must be enabled for the RenderSystem's lights to be updated." && m_renderPasses.front());

  const ShaderProgram& geometryProgram = getGeometryPass().getProgram();

  geometryProgram.use();

  const std::string strBase = "uniLights[" + std::to_string(lightIndex) + "].";

  const std::string posStr    = strBase + "position";
  const std::string energyStr = strBase + "energy";
  const std::string colorStr  = strBase + "color";
  const std::string angleStr  = strBase + "angle";

  const auto& lightComp = entity->getComponent<Light>();
  Vec4f homogeneousPos(entity->getComponent<Transform>().getPosition(), 1.f);

  if (lightComp.getType() == LightType::DIRECTIONAL) {
    homogeneousPos[3] = 0.f;
    geometryProgram.sendUniform(strBase + "direction", lightComp.getDirection());
  }

  geometryProgram.sendUniform(posStr,    homogeneousPos);
  geometryProgram.sendUniform(energyStr, lightComp.getEnergy());
  geometryProgram.sendUniform(colorStr,  lightComp.getColor());
  geometryProgram.sendUniform(angleStr,  lightComp.getAngle());
}

void RenderSystem::updateLights() const {
  assert("Error: Geometry pass must be enabled for the RenderSystem's lights to be updated." && m_renderPasses.front());

  if (!m_renderPasses.front())
    return;

  std::size_t lightCount = 0;

  for (const Entity* entity : m_entities) {
    if (entity->hasComponent<Light>()) {
      updateLight(entity, lightCount);
      ++lightCount;
    }
  }

  getGeometryPass().getProgram().sendUniform("uniLightCount", static_cast<unsigned int>(lightCount));
}

void RenderSystem::updateShaders() const {
  assert("Error: Geometry pass must be enabled for the RenderSystem's shaders to be updated." && m_renderPasses.front());

  for (const RenderPassPtr& renderPass : m_renderPasses) {
    if (renderPass)
      renderPass->getProgram().updateShaders();
  }

  sendCameraMatrices();
  updateLights();

  for (const Entity* entity : m_entities) {
    if (entity->hasComponent<Mesh>())
      entity->getComponent<Mesh>().load(getGeometryPass().getProgram());
  }
}

void RenderSystem::saveToImage(const FilePath& filePath, TextureFormat format) const {
  ImageColorspace colorspace = ImageColorspace::RGB;
  TextureDataType dataType   = TextureDataType::UBYTE;

  switch (format) {
    case TextureFormat::DEPTH:
      colorspace = ImageColorspace::DEPTH;
      dataType   = TextureDataType::FLOAT;
      break;

    case TextureFormat::RGBA:
    case TextureFormat::BGRA:
      colorspace = ImageColorspace::RGBA;
      break;

    default:
      break;
  }

  Image img(m_sceneWidth, m_sceneHeight, colorspace);
  Renderer::recoverFrame(m_sceneWidth, m_sceneHeight, format, dataType, img.getDataPtr());

  img.save(filePath, true);
}

void RenderSystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);

  if (entity->hasComponent<Camera>())
    m_cameraEntity = entity.get();

  if (entity->hasComponent<Light>())
    updateLights();

  if (entity->hasComponent<Mesh>())
    entity->getComponent<Mesh>().load(m_renderPasses.front()->getProgram());
}

void RenderSystem::initialize() {
  Renderer::initialize();

  m_acceptedComponents.setBit(Component::getId<Camera>());
  m_acceptedComponents.setBit(Component::getId<Light>());
  m_acceptedComponents.setBit(Component::getId<Mesh>());

  m_cameraUbo.bindBufferBase(0);

  for (std::size_t passIndex = 1; passIndex < m_renderPasses.size(); ++passIndex) {
    const RenderPassPtr& pass = m_renderPasses[passIndex];

    if (pass) {
      const ShaderProgram& passProgram = pass->getProgram();

      pass->getFramebuffer().initBuffers(passProgram);
      m_cameraUbo.bindUniformBlock(passProgram, "uboCameraMatrices", 0);
    }
  }
}

void RenderSystem::initialize(unsigned int sceneWidth, unsigned int sceneHeight) {
  initialize();
  resizeViewport(sceneWidth, sceneHeight);
}

} // namespace Raz
