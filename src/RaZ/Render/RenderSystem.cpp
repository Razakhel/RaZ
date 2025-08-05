#include "RaZ/Application.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#if defined(RAZ_USE_XR)
#include "RaZ/XR/XrSystem.hpp"
#endif

#include "tracy/Tracy.hpp"
#include "GL/glew.h" // Needed by TracyOpenGL.hpp
#include "tracy/TracyOpenGL.hpp"

namespace Raz {

void RenderSystem::setCubemap(Cubemap&& cubemap) {
  m_cubemap = std::move(cubemap);
  m_cameraUbo.bindUniformBlock(m_cubemap->getProgram(), "uboCameraInfo", 0);
}

#if defined(RAZ_USE_XR)
void RenderSystem::enableXr(XrSystem& xrSystem) {
  m_xrSystem = &xrSystem;

  xrSystem.initializeSession();
  resizeViewport(xrSystem.getOptimalViewWidth(), xrSystem.getOptimalViewHeight());
}
#endif

void RenderSystem::resizeViewport(unsigned int width, unsigned int height) {
  ZoneScopedN("RenderSystem::resizeViewport");

  m_sceneWidth  = width;
  m_sceneHeight = height;

  Renderer::resizeViewport(0, 0, m_sceneWidth, m_sceneHeight);

  if (m_cameraEntity)
    m_cameraEntity->getComponent<Camera>().resizeViewport(m_sceneWidth, m_sceneHeight);

  m_renderGraph.resizeViewport(m_sceneWidth, m_sceneHeight);
}

bool RenderSystem::update(const FrameTimeInfo& timeInfo) {
  ZoneScopedN("RenderSystem::update");
  TracyGpuZone("RenderSystem::update")

  m_cameraUbo.bindBase(0);
  m_lightsUbo.bindBase(1);
  m_timeUbo.bindBase(2);
  m_modelUbo.bindBase(3);

  // TODO: this should be made only once at the passes' shader programs' initialization (as is done when updating shaders), not every frame
  //   Forcing to update shaders when adding a new pass would not be ideal either, as it implies many operations. Find a better & user-friendly way
  for (std::size_t i = 0; i < m_renderGraph.getNodeCount(); ++i) {
    const RenderShaderProgram& passProgram = m_renderGraph.getNode(i).getProgram();
    m_cameraUbo.bindUniformBlock(passProgram, "uboCameraInfo", 0);
    m_lightsUbo.bindUniformBlock(passProgram, "uboLightsInfo", 1);
    m_timeUbo.bindUniformBlock(passProgram, "uboTimeInfo", 2);
  }

  m_timeUbo.bind();
  m_timeUbo.sendData(timeInfo.deltaTime, 0);
  m_timeUbo.sendData(timeInfo.globalTime, sizeof(float));

#if defined(RAZ_USE_XR)
  if (m_xrSystem) {
    renderXrFrame();
  } else
#endif
  {
    sendCameraInfo();
    m_renderGraph.execute(*this);
  }

#if defined(RAZ_CONFIG_DEBUG) && !defined(SKIP_RENDERER_ERRORS)
  Renderer::printErrors();
#endif

#if !defined(RAZ_NO_WINDOW)
  if (m_window)
    return m_window->run(timeInfo.deltaTime);
#endif

  return true;
}

void RenderSystem::updateLights() const {
  ZoneScopedN("RenderSystem::updateLights");

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
  ZoneScopedN("RenderSystem::updateShaders");

  m_renderGraph.updateShaders();

  for (std::size_t i = 0; i < m_renderGraph.getNodeCount(); ++i) {
    const RenderShaderProgram& passProgram = m_renderGraph.getNode(i).getProgram();
    m_cameraUbo.bindUniformBlock(passProgram, "uboCameraInfo", 0);
    m_lightsUbo.bindUniformBlock(passProgram, "uboLightsInfo", 1);
    m_timeUbo.bindUniformBlock(passProgram, "uboTimeInfo", 2);
  }

  for (Entity* entity : m_entities) {
    if (!entity->hasComponent<MeshRenderer>())
      continue;

    auto& meshRenderer = entity->getComponent<MeshRenderer>();

    for (Material& material : meshRenderer.getMaterials())
      material.getProgram().updateShaders();

    updateMaterials(meshRenderer);
  }
}

void RenderSystem::updateMaterials(const MeshRenderer& meshRenderer) const {
  ZoneScopedN("RenderSystem::updateMaterials(MeshRenderer)");

  for (const Material& material : meshRenderer.getMaterials()) {
    const RenderShaderProgram& materialProgram = material.getProgram();

    materialProgram.sendAttributes();
    materialProgram.initTextures();
#if !defined(USE_WEBGL)
    materialProgram.initImageTextures();
#endif

    m_cameraUbo.bindUniformBlock(materialProgram, "uboCameraInfo", 0);
    m_lightsUbo.bindUniformBlock(materialProgram, "uboLightsInfo", 1);
    m_timeUbo.bindUniformBlock(materialProgram, "uboTimeInfo", 2);
    m_modelUbo.bindUniformBlock(materialProgram, "uboModelInfo", 3);
  }
}

void RenderSystem::updateMaterials() const {
  ZoneScopedN("RenderSystem::updateMaterials");

  for (const Entity* entity : m_entities) {
    if (entity->hasComponent<MeshRenderer>())
      updateMaterials(entity->getComponent<MeshRenderer>());
  }
}

void RenderSystem::saveToImage(const FilePath& filePath, TextureFormat format, PixelDataType dataType) const {
  ZoneScopedN("RenderSystem::saveToImage");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  ImageColorspace colorspace = ImageColorspace::RGB;

  switch (format) {
    case TextureFormat::DEPTH:
      colorspace = ImageColorspace::GRAY;
      dataType   = PixelDataType::FLOAT;
      break;

    case TextureFormat::RGBA:
    case TextureFormat::BGRA:
      colorspace = ImageColorspace::RGBA;
      break;

    default:
      break;
  }

  Image img(m_sceneWidth, m_sceneHeight, colorspace, (dataType == PixelDataType::FLOAT ? ImageDataType::FLOAT : ImageDataType::BYTE));
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
  ZoneScopedN("RenderSystem::linkEntity");

  System::linkEntity(entity);

  if (entity->hasComponent<Camera>())
    m_cameraEntity = entity.get();

  if (entity->hasComponent<Light>())
    updateLights();

  if (entity->hasComponent<MeshRenderer>())
    updateMaterials(entity->getComponent<MeshRenderer>());
}

void RenderSystem::initialize() {
  ZoneScopedN("RenderSystem::initialize");

  registerComponents<Camera, Light, MeshRenderer>();

  // TODO: this Renderer initialization is technically useless; the RenderSystem needs to have it initialized before construction
  //  (either manually or through the Window's initialization), since it constructs the RenderGraph's rendering objects
  //  As such, if reaching here, the Renderer is necessarily already functional. Ideally, this call below should be the only one in the whole program
  Renderer::initialize();
  Renderer::enable(Capability::CULL);
  Renderer::enable(Capability::BLEND);
  Renderer::enable(Capability::DEPTH_TEST);
  Renderer::enable(Capability::STENCIL_TEST);
#if !defined(USE_OPENGL_ES)
  Renderer::enable(Capability::CUBEMAP_SEAMLESS);
#endif

#if !defined(USE_OPENGL_ES)
  // Setting the depth to a [0; 1] range instead of a [-1; 1] one is always a good thing, since the [-1; 0] subrange is never used anyway
  if (Renderer::checkVersion(4, 5) || Renderer::isExtensionSupported("GL_ARB_clip_control"))
    Renderer::setClipControl(ClipOrigin::LOWER_LEFT, ClipDepth::ZERO_TO_ONE);

  if (Renderer::checkVersion(4, 3)) {
    Renderer::setLabel(RenderObjectType::BUFFER, m_cameraUbo.getIndex(), "Camera uniform buffer");
    Renderer::setLabel(RenderObjectType::BUFFER, m_lightsUbo.getIndex(), "Lights uniform buffer");
    Renderer::setLabel(RenderObjectType::BUFFER, m_timeUbo.getIndex(), "Time uniform buffer");
    Renderer::setLabel(RenderObjectType::BUFFER, m_modelUbo.getIndex(), "Model uniform buffer");
  }
#endif
}

void RenderSystem::initialize(unsigned int sceneWidth, unsigned int sceneHeight) {
  initialize();
  resizeViewport(sceneWidth, sceneHeight);
}

void RenderSystem::sendCameraInfo() const {
  assert("Error: The render system needs a camera to send its info." && (m_cameraEntity != nullptr));
  assert("Error: The camera must have a transform component to send its info." && m_cameraEntity->hasComponent<Transform>());

  ZoneScopedN("RenderSystem::sendCameraInfo");

  auto& camera       = m_cameraEntity->getComponent<Camera>();
  auto& camTransform = m_cameraEntity->getComponent<Transform>();

  m_cameraUbo.bind();

  if (camTransform.hasUpdated()) {
    if (camera.getCameraType() == CameraType::LOOK_AT)
      camera.computeLookAt(camTransform.getPosition());
    else
      camera.computeViewMatrix(camTransform);

    camera.computeInverseViewMatrix();

    sendViewMatrix(camera.getViewMatrix());
    sendInverseViewMatrix(camera.getInverseViewMatrix());
    sendCameraPosition(camTransform.getPosition());

    camTransform.setUpdated(false);
  }

  sendProjectionMatrix(camera.getProjectionMatrix());
  sendInverseProjectionMatrix(camera.getInverseProjectionMatrix());
  sendViewProjectionMatrix(camera.getProjectionMatrix() * camera.getViewMatrix());
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

#if defined(RAZ_USE_XR)
void RenderSystem::renderXrFrame() {
  ZoneScopedN("RenderSystem::renderXrFrame");
  TracyGpuZone("RenderSystem::renderXrFrame")

  const bool hasRendered = m_xrSystem->renderFrame([this] (Vec3f position, Quaternionf rotation, ViewFov viewFov) {
    if (m_cameraEntity) {
      const auto& camTransform = m_cameraEntity->getComponent<Transform>();
      position = camTransform.getRotation() * position + camTransform.getPosition();
      rotation = camTransform.getRotation() * rotation;
    }

    Mat4f invViewMat = rotation.computeMatrix();
    invViewMat.getElement(3, 0) = position.x();
    invViewMat.getElement(3, 1) = position.y();
    invViewMat.getElement(3, 2) = position.z();
    const Mat4f viewMat = invViewMat.inverse();

    const float tanAngleRight    = std::tan(viewFov.angleRight.value);
    const float tanAngleLeft     = std::tan(viewFov.angleLeft.value);
    const float tanAngleUp       = std::tan(viewFov.angleUp.value);
    const float tanAngleDown     = std::tan(viewFov.angleDown.value);
    const float invAngleWidth    = 1.f / (tanAngleRight - tanAngleLeft);
    const float invAngleHeight   = 1.f / (tanAngleUp - tanAngleDown);
    const float angleWidthDiff   = tanAngleRight + tanAngleLeft;
    const float angleHeightDiff  = tanAngleUp + tanAngleDown;
    constexpr float nearZ        = 0.1f;
    constexpr float farZ         = 1000.f;
    constexpr float invDepthDiff = 1.f / (farZ - nearZ);
    const Mat4f projMat(2.f * invAngleWidth, 0.f,                  angleWidthDiff * invAngleWidth,   0.f,
                        0.f,                 2.f * invAngleHeight, angleHeightDiff * invAngleHeight, 0.f,
                        0.f,                 0.f,                  -(farZ + nearZ) * invDepthDiff,   -(farZ * (nearZ + nearZ)) * invDepthDiff,
                        0.f,                 0.f,                  -1.f,                             0.f);

    m_cameraUbo.bind();
    sendViewMatrix(viewMat);
    sendInverseViewMatrix(invViewMat);
    sendProjectionMatrix(projMat);
    sendInverseProjectionMatrix(projMat.inverse());
    sendViewProjectionMatrix(projMat * viewMat);
    sendCameraPosition(position);

    m_renderGraph.execute(*this);

    assert("Error: There is no valid last executed pass." && m_renderGraph.m_lastExecutedPass);
    const Framebuffer& finalFramebuffer = m_renderGraph.m_lastExecutedPass->getFramebuffer();
    assert("Error: The last executed pass must have at least one write color buffer." && finalFramebuffer.getColorBufferCount() >= 1);
    assert("Error: Either the last executed pass or the geometry pass must have a write depth buffer."
      && (finalFramebuffer.hasDepthBuffer() || m_renderGraph.m_geometryPass.getFramebuffer().hasDepthBuffer()));

    const Texture2D& depthBuffer = (finalFramebuffer.hasDepthBuffer() ? finalFramebuffer.getDepthBuffer()
                                                                      : m_renderGraph.m_geometryPass.getFramebuffer().getDepthBuffer());
    return std::make_pair(std::cref(finalFramebuffer.getColorBuffer(0)), std::cref(depthBuffer));
  });

#if !defined(RAZ_NO_WINDOW)
  if (!hasRendered)
    return;

  const Framebuffer& finalFramebuffer = m_renderGraph.m_lastExecutedPass->getFramebuffer();
  const Texture2D& depthBuffer        = (finalFramebuffer.hasDepthBuffer() ? finalFramebuffer.getDepthBuffer()
                                                                           : m_renderGraph.m_geometryPass.getFramebuffer().getDepthBuffer());
  copyToWindow(finalFramebuffer.getColorBuffer(0), depthBuffer, m_window->getWidth(), m_window->getHeight());
#endif
}
#endif

void RenderSystem::copyToWindow(const Texture2D& colorBuffer, const Texture2D& depthBuffer, unsigned int windowWidth, unsigned int windowHeight) const {
  assert("Error: The given color buffer must have a valid & non-depth colorspace to be copied to the window."
      && colorBuffer.getColorspace() != TextureColorspace::DEPTH && colorBuffer.getColorspace() != TextureColorspace::INVALID);
  assert("Error: The given depth buffer must have a depth colorspace to be copied to the window."
      && depthBuffer.getColorspace() == TextureColorspace::DEPTH);

  ZoneScopedN("RenderSystem::copyToWindow");
  TracyGpuZone("RenderSystem::copyToWindow")

  static RenderPass windowCopyPass = [] () {
    RenderPass copyPass(FragmentShader::loadFromSource(R"(
      in vec2 fragTexcoords;

      uniform sampler2D uniFinalColorBuffer;
      uniform sampler2D uniFinalDepthBuffer;
      uniform vec2 uniSizeFactor;

      layout(location = 0) out vec4 fragColor;

      void main() {
        vec2 scaledUv = fragTexcoords * uniSizeFactor;
        fragColor     = texture(uniFinalColorBuffer, scaledUv).rgba;
        gl_FragDepth  = texture(uniFinalDepthBuffer, scaledUv).r;
      }
    )"), "Window copy pass");

    RenderShaderProgram& copyProgram = copyPass.getProgram();
    copyProgram.setAttribute(0, "uniFinalColorBuffer");
    copyProgram.setAttribute(1, "uniFinalDepthBuffer");

    return copyPass;
  }();

  RenderShaderProgram& windowCopyProgram = windowCopyPass.getProgram();

  const Vec2f sizeFactor(static_cast<float>(m_sceneWidth) / static_cast<float>(windowWidth),
                         static_cast<float>(m_sceneHeight) / static_cast<float>(windowHeight));
  windowCopyProgram.setAttribute(sizeFactor, "uniSizeFactor");
  windowCopyProgram.sendAttributes();

  windowCopyProgram.use();
  Renderer::setActiveTexture(0);
  colorBuffer.bind();
  Renderer::setActiveTexture(1);
  depthBuffer.bind();

  Renderer::bindFramebuffer(0);
  Renderer::clear(MaskType::COLOR | MaskType::DEPTH | MaskType::STENCIL);

  Renderer::setDepthFunction(DepthStencilFunction::ALWAYS);
  windowCopyPass.execute();
  Renderer::setDepthFunction(DepthStencilFunction::LESS);
}

} // namespace Raz
