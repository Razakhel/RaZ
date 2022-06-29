#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"

namespace Raz {

void RenderSystem::setCubemap(Cubemap&& cubemap) {
  m_cubemap = std::move(cubemap);
  m_cameraUbo.bindUniformBlock(m_cubemap->getProgram(), "uboCameraInfo", 0);
}

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
  m_cameraUbo.bindBase(0);
  m_lightsUbo.bindBase(1);
  m_modelUbo.bindBase(2);

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

void RenderSystem::sendCameraMatrices() const {
  assert("Error: A camera must be given to a RenderSystem to send its matrices." && (m_cameraEntity != nullptr));

  const auto& camera = m_cameraEntity->getComponent<Camera>();

  m_cameraUbo.bind();
  sendViewMatrix(camera.getViewMatrix());
  sendInverseViewMatrix(camera.getInverseViewMatrix());
  sendProjectionMatrix(camera.getProjectionMatrix());
  sendInverseProjectionMatrix(camera.getInverseProjectionMatrix());
  sendViewProjectionMatrix(camera.getProjectionMatrix() * camera.getViewMatrix());
  sendCameraPosition(m_cameraEntity->getComponent<Transform>().getPosition());
}

void RenderSystem::updateLight(const Entity& entity, unsigned int lightIndex) const {
  const auto& light = entity.getComponent<Light>();
  const std::size_t dataStride = sizeof(Vec4f) * 4 * lightIndex;

  if (light.getType() == LightType::DIRECTIONAL) {
    m_lightsUbo.sendData(Vec4f(0.f), static_cast<unsigned int>(dataStride));
  } else {
    assert("Error: A non-directional light needs to have a Transform component." && entity.hasComponent<Transform>());
    m_lightsUbo.sendData(Vec4f(entity.getComponent<Transform>().getPosition(), 1.f), static_cast<unsigned int>(dataStride));
  }

  m_lightsUbo.sendData(light.getDirection(), static_cast<unsigned int>(dataStride + sizeof(Vec4f)));
  m_lightsUbo.sendData(light.getColor(), static_cast<unsigned int>(dataStride + sizeof(Vec4f) * 2));
  m_lightsUbo.sendData(light.getEnergy(), static_cast<unsigned int>(dataStride + sizeof(Vec4f) * 3));
  m_lightsUbo.sendData(light.getAngle().value, static_cast<unsigned int>(dataStride + sizeof(Vec4f) * 3 + sizeof(float)));
}

void RenderSystem::updateLights() const {
  unsigned int lightCount = 0;

  m_lightsUbo.bind();

  for (const Entity* entity : m_entities) {
    if (!entity->isEnabled() || !entity->hasComponent<Light>())
      continue;

    updateLight(*entity, lightCount);
    ++lightCount;
  }

  m_lightsUbo.sendData(lightCount, sizeof(Vec4f) * 4 * 100);
}

void RenderSystem::updateShaders() const {
  m_renderGraph.updateShaders();

  for (const Entity* entity : m_entities) {
    if (!entity->hasComponent<MeshRenderer>())
      continue;

    const auto& meshRenderer = entity->getComponent<MeshRenderer>();

    for (const Material& material : meshRenderer.getMaterials())
      material.getProgram().updateShaders();

    updateMaterials(meshRenderer);
  }
}

void RenderSystem::updateMaterials(const MeshRenderer& meshRenderer) const {
  for (const Material& material : meshRenderer.getMaterials()) {
    material.sendAttributes();
    material.initTextures();

    const RenderShaderProgram& materialProgram = material.getProgram();
    m_cameraUbo.bindUniformBlock(materialProgram, "uboCameraInfo", 0);
    m_lightsUbo.bindUniformBlock(materialProgram, "uboLightsInfo", 1);
    m_modelUbo.bindUniformBlock(materialProgram, "uboModelInfo", 2);
  }
}

void RenderSystem::updateMaterials() const {
  for (const Entity* entity : m_entities) {
    if (entity->hasComponent<MeshRenderer>())
      updateMaterials(entity->getComponent<MeshRenderer>());
  }
}

void RenderSystem::saveToImage(const FilePath& filePath, TextureFormat format, TextureDataType dataType) const {
  ImageColorspace colorspace = ImageColorspace::RGB;

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

  Image img(m_sceneWidth, m_sceneHeight, colorspace, (dataType == TextureDataType::FLOAT ? ImageDataType::FLOAT : ImageDataType::BYTE));
  Renderer::recoverFrame(m_sceneWidth, m_sceneHeight, format, dataType, img.getDataPtr());

  ImageFormat::save(filePath, img, true);
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
    updateMaterials(entity->getComponent<MeshRenderer>());
}

void RenderSystem::initialize() {
  // TODO: this Renderer initialization is technically useless; the RenderSystem needs to have it initialized before construction
  //  (either manually or through the Window's initialization), since it constructs the RenderGraph's rendering objects
  //  As such, if reaching here, the Renderer is necessarily already functional. Ideally, this call below should be the only one in the whole program
  Renderer::initialize();
  Renderer::enable(Capability::CULL);
  Renderer::enable(Capability::DEPTH_TEST);
  Renderer::enable(Capability::STENCIL_TEST);
#if !defined(USE_OPENGL_ES)
  Renderer::enable(Capability::CUBEMAP_SEAMLESS);
#endif

  m_acceptedComponents.setBit(Component::getId<Camera>());
  m_acceptedComponents.setBit(Component::getId<Light>());
  m_acceptedComponents.setBit(Component::getId<MeshRenderer>());
}

void RenderSystem::initialize(unsigned int sceneWidth, unsigned int sceneHeight) {
  initialize();
  resizeViewport(sceneWidth, sceneHeight);
}

} // namespace Raz
