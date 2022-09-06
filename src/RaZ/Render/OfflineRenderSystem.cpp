#include "RaZ/Data/BvhSystem.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/MathUtils.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/OfflineRenderSystem.hpp"
#include "RaZ/Utils/Threading.hpp"

namespace Raz {

void OfflineRenderSystem::resizeViewport(unsigned int width, unsigned int height) {
  m_renderedScene = Image(width, height, ImageColorspace::RGB);

#if !defined(RAZ_NO_WINDOW)
  if (m_window)
    m_window->resize(width, height);

  s_texture->resize(width, height);
#endif

  if (m_cameraEntity)
    m_cameraEntity->getComponent<Camera>().resizeViewport(width, height);
}

bool OfflineRenderSystem::update([[maybe_unused]] float deltaTime) {
  auto& camera   = m_cameraEntity->getComponent<Camera>();
  auto& camTrans = m_cameraEntity->getComponent<Transform>();

  // TODO: the camera update must not be made here, as it must not be in RenderGraph either. A better way must be added for it to be independent
  if (camTrans.hasUpdated()) {
    if (camera.getCameraType() == CameraType::LOOK_AT)
      camera.computeLookAt(camTrans.getPosition());
    else
      camera.computeViewMatrix(camTrans);

    camera.computeInverseViewMatrix();
    camTrans.setUpdated(false);
  }

  const float invWidth  = 1.f / static_cast<float>(m_renderedScene.getWidth());
  const float invHeight = 1.f / static_cast<float>(m_renderedScene.getHeight());

  auto* imgData = static_cast<uint8_t*>(m_renderedScene.getDataPtr());

  Threading::parallelize(0, m_renderedScene.getHeight(), [this, imgData, invWidth, invHeight, &camera, &camTrans] (const Threading::IndexRange& range) {
    for (std::size_t heightIndex = range.beginIndex; heightIndex < range.endIndex; ++heightIndex) {
      const float projHeight = static_cast<float>(heightIndex) * invHeight * 2.f - 1.f;
      const std::size_t pixelHeightIndex = heightIndex * m_renderedScene.getWidth() * 3;

      for (std::size_t widthIndex = 0; widthIndex < m_renderedScene.getWidth(); ++widthIndex) {
        const float projWidth = (static_cast<float>(widthIndex) * invWidth - 0.5f) * 2.f;

        const Vec3f projPos = camera.unproject(Vec2f(projWidth, projHeight));
        const Vec3f projDir = (projPos - camTrans.getPosition()).normalize();

        const Ray ray(camTrans.getPosition(), projDir);

        RayHit closestHit;
        const Entity* hitEntity = recoverHitEntity(ray, closestHit);

        const std::size_t pixelIndex = pixelHeightIndex + widthIndex * 3;

        if (hitEntity == nullptr) {
          imgData[pixelIndex    ] = 38; // ~0.15, the window's default clear color
          imgData[pixelIndex + 1] = 38;
          imgData[pixelIndex + 2] = 38;

          continue;
        }

        const Vec3f lighting = computeLighting(*hitEntity, closestHit);

        imgData[pixelIndex    ] = static_cast<uint8_t>(std::clamp(lighting.x(), 0.f, 1.f) * 255.f);
        imgData[pixelIndex + 1] = static_cast<uint8_t>(std::clamp(lighting.y(), 0.f, 1.f) * 255.f);
        imgData[pixelIndex + 2] = static_cast<uint8_t>(std::clamp(lighting.z(), 0.f, 1.f) * 255.f);
      }
    }
  });

#if !defined(RAZ_NO_WINDOW)
  s_texture->load(m_renderedScene, false);
  m_displayPass.execute();

  if (m_window)
    return m_window->run(deltaTime);
#endif

  return true;
}

void OfflineRenderSystem::saveToImage(const FilePath& filePath) const {
  ImageFormat::save(filePath, m_renderedScene, true);
}

void OfflineRenderSystem::destroy() {
#if !defined(RAZ_NO_WINDOW)
  if (m_window)
    m_window->setShouldClose();
#endif
}

void OfflineRenderSystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);

  if (entity->hasComponent<Camera>())
    m_cameraEntity = entity.get();
}

void OfflineRenderSystem::initialize() {
  registerComponents<Camera, Light, Mesh>();

#if !defined(RAZ_NO_WINDOW)
  s_texture = Texture2D::create(TextureColorspace::RGB);

  static constexpr std::string_view fragSource = R"(
    uniform sampler2D uniTexture;

    in vec2 fragTexcoords;

    layout(location = 0) out vec4 fragColor;

    void main() {
      fragColor = texture(uniTexture, fragTexcoords);
    }
  )";

  m_displayPass = RenderPass(FragmentShader::loadFromSource(fragSource), "Offline display render pass");
  m_displayPass.addReadTexture(s_texture, "uniTexture");
#endif
}

void OfflineRenderSystem::initialize(unsigned int sceneWidth, unsigned int sceneHeight) {
  initialize();
  resizeViewport(sceneWidth, sceneHeight);
}

const Entity* OfflineRenderSystem::recoverHitEntity(const Ray& ray, RayHit& closestHit) {
  if (m_bvh)
    return m_bvh->query(ray, &closestHit);

  // No BVH available, finding an intersection manually

  Entity* hitEntity {};

  for (Entity* entity : m_entities) {
    if (!entity->hasComponent<Mesh>())
      continue;

    RayHit meshHit;

    if (!ray.intersects(entity->getComponent<Mesh>().computeBoundingBox(), &meshHit) || meshHit.distance >= closestHit.distance)
      continue;

    hitEntity  = entity;
    closestHit = meshHit;
  }

  if (hitEntity == nullptr)
    return nullptr;

  bool triangleFound = false;

  for (const Submesh& submesh : hitEntity->getComponent<Mesh>().getSubmeshes()) {
    for (std::size_t i = 0; i < submesh.getTriangleIndexCount(); i += 3) {
      // TODO: transform the triangle with the entity's Transform component
      const Triangle triangle(submesh.getVertices()[submesh.getTriangleIndices()[i    ]].position,
                              submesh.getVertices()[submesh.getTriangleIndices()[i + 1]].position,
                              submesh.getVertices()[submesh.getTriangleIndices()[i + 2]].position);

      RayHit triangleHit;

      if (ray.intersects(triangle, &triangleHit) && triangleHit.distance < closestHit.distance)
        triangleFound = true;
    }
  }

  return (triangleFound ? hitEntity : nullptr);
}

Vec3f OfflineRenderSystem::computeLighting(const Entity& hitEntity, const RayHit& hitInfo) {
  const auto& material = hitEntity.getComponent<MeshRenderer>().getMaterials().front();

  const auto baseColor         = material.getAttribute<Vec3f>(MaterialAttribute::BaseColor);
  const auto metallic          = material.getAttribute<float>(MaterialAttribute::Metallic);
  const Vec3f baseReflectivity = MathUtils::lerp(Vec3f(0.04f), baseColor, metallic);

  const Vec3f viewDir = (m_cameraEntity->getComponent<Transform>().getPosition() - hitInfo.position).normalize();

  Vec3f lightRadiance(0.f);

  for (const Entity* entity : m_entities) {
    if (!entity->hasComponent<Light>())
      continue;

    const auto& light      = entity->getComponent<Light>();
    const auto& lightTrans = entity->getComponent<Transform>();

    Vec3f lightDir = (light.getType() == LightType::POINT ? lightTrans.getPosition() - hitInfo.position : -light.getDirection());

    const float lightSqDist = lightDir.computeSquaredLength();
    const float attenuation = light.getEnergy() / lightSqDist;

    lightDir /= std::sqrt(lightSqDist); // Normalizing the light direction

    const Vec3f halfDir = (viewDir + lightDir).normalize();

    // Fresnel
    const float cosTheta = std::max(halfDir.dot(viewDir), 0.f);
    const Vec3f fresnel  = baseReflectivity + (Vec3f(1.f) - baseReflectivity) * std::pow(2.f, (-5.55473f * cosTheta - 6.98316f) * cosTheta);

    const Vec3f radiance = light.getColor() * attenuation * std::max(lightDir.dot(hitInfo.normal), 0.f);
    const Vec3f diffuse  = (Vec3f(1.f) - fresnel) * (1.f - metallic);

    lightRadiance += (diffuse * baseColor /*+ specular*/) * radiance;
  }

  const Vec3f ambient = 0.03f * baseColor/* * ambOcc*/;

  Vec3f lighting = ambient + lightRadiance;

  if (material.hasAttribute(MaterialAttribute::Emissive))
    lighting += material.getAttribute<Vec3f>(MaterialAttribute::Emissive);

  // HDR tone mapping
  lighting = lighting / (lighting + 1.f);

  // Gamma correction
  lighting.x() = std::pow(lighting.x(), 0.4545454545f); // 1 / 2.2
  lighting.y() = std::pow(lighting.y(), 0.4545454545f);
  lighting.z() = std::pow(lighting.z(), 0.4545454545f);

  return lighting;
}

} // namespace Raz
