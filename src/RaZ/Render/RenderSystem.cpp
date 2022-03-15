#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace Raz {

void RenderSystem::resizeViewport(unsigned int width, unsigned int height) {
  m_sceneWidth  = width;
  m_sceneHeight = height;

  Renderer::resizeViewport(0, 0, m_sceneWidth, m_sceneHeight);

#if !defined(RAZ_NO_WINDOW)
  if (m_window)
    m_window->resize(m_sceneWidth, m_sceneHeight);
#endif

  if (m_cameraEntity)
    m_cameraEntity->getComponent<Camera>().resizeViewport(m_sceneWidth, m_sceneHeight);

  m_renderGraph.resizeViewport(width, height);
}

RenderPass& RenderSystem::addRenderPass(VertexShader vertShader, FragmentShader fragShader) {
  return m_renderGraph.addNode(std::move(vertShader), std::move(fragShader));
}

RenderPass& RenderSystem::addRenderPass(FragmentShader fragShader) {
  return m_renderGraph.addNode(std::move(fragShader));
}

bool RenderSystem::update([[maybe_unused]] float deltaTime) {
  m_renderGraph.execute(*this);

#if defined(RAZ_CONFIG_DEBUG) && !defined(SKIP_RENDERER_ERRORS)
  Renderer::printErrors();
#endif

#if !defined(RAZ_NO_WINDOW)
  if (m_window)
    return m_window->run(deltaTime);
#endif

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
  sendCameraMatrices(camera.getProjectionMatrix() * camera.getViewMatrix());
}

void RenderSystem::updateLight(const Entity* entity, std::size_t lightIndex) const {
  const RenderShaderProgram& geometryProgram = getGeometryProgram();

  geometryProgram.use();

  const std::string strBase = "uniLights[" + std::to_string(lightIndex) + "].";

  const std::string posStr    = strBase + "position";
  const std::string energyStr = strBase + "energy";
  const std::string colorStr  = strBase + "color";
  const std::string angleStr  = strBase + "angle";

  const auto& lightComp = entity->getComponent<Light>();
  Vec4f homogeneousPos(entity->getComponent<Transform>().getPosition(), 1.f);

  if (lightComp.getType() == LightType::DIRECTIONAL) {
    homogeneousPos.w() = 0.f;
    geometryProgram.sendUniform(strBase + "direction", lightComp.getDirection());
  }

  geometryProgram.sendUniform(posStr,    homogeneousPos);
  geometryProgram.sendUniform(energyStr, lightComp.getEnergy());
  geometryProgram.sendUniform(colorStr,  lightComp.getColor());
  geometryProgram.sendUniform(angleStr,  lightComp.getAngle());
}

void RenderSystem::updateLights() const {
  std::size_t lightCount = 0;

  for (const Entity* entity : m_entities) {
    if (entity->hasComponent<Light>() && entity->isEnabled()) {
      updateLight(entity, lightCount);
      ++lightCount;
    }
  }

  getGeometryProgram().sendUniform("uniLightCount", static_cast<unsigned int>(lightCount));
}

void RenderSystem::updateShaders() const {
  m_renderGraph.updateShaders();

  sendCameraMatrices();
  updateLights();

  for (const Entity* entity : m_entities) {
    if (entity->hasComponent<MeshRenderer>())
      entity->getComponent<MeshRenderer>().load(getGeometryProgram());
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

void RenderSystem::destroy() {
#if !defined(RAZ_NO_WINDOW)
  if (m_window)
    m_window->setShouldClose();
#endif
}

void RenderSystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);

  if (entity->hasComponent<Camera>())
    m_cameraEntity = entity.get();

  if (entity->hasComponent<Light>())
    updateLights();

  if (entity->hasComponent<MeshRenderer>())
    entity->getComponent<MeshRenderer>().load(getGeometryProgram());
}

void RenderSystem::initialize() {
  Renderer::initialize();

  m_acceptedComponents.setBit(Component::getId<Camera>());
  m_acceptedComponents.setBit(Component::getId<Light>());
  m_acceptedComponents.setBit(Component::getId<MeshRenderer>());

  m_cameraUbo.bindBufferBase(0);
}

void RenderSystem::initialize(unsigned int sceneWidth, unsigned int sceneHeight) {
  initialize();
  resizeViewport(sceneWidth, sceneHeight);
}

} // namespace Raz
