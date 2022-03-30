#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

void MeshRenderer::drawUnitPlane() {
  static const MeshRenderer plane(Mesh(Plane(Vec3f(0.f), Axis::Y), 1.f, 1.f), RenderMode::TRIANGLE);
  plane.draw();
}

void MeshRenderer::drawUnitSphere() {
  static const MeshRenderer sphere(Mesh(Sphere(Vec3f(0.f), 1.f), 10, SphereMeshType::UV), RenderMode::TRIANGLE);
  sphere.draw();
}

void MeshRenderer::drawUnitQuad() {
  static const MeshRenderer quad(Mesh(Quad(Vec3f(-1.f,  1.f, 0.f),
                                           Vec3f( 1.f,  1.f, 0.f),
                                           Vec3f( 1.f, -1.f, 0.f),
                                           Vec3f(-1.f, -1.f, 0.f))), RenderMode::TRIANGLE);
  quad.draw();
}

void MeshRenderer::drawUnitCube() {
  static const MeshRenderer cube(Mesh(AABB(Vec3f(-1.f, -1.f, -1.f),
                                           Vec3f( 1.f,  1.f,  1.f))), RenderMode::TRIANGLE);
  cube.draw();
}

void MeshRenderer::setRenderMode(RenderMode renderMode, const Mesh& mesh) {
  for (std::size_t i = 0; i < m_submeshRenderers.size(); ++i)
    m_submeshRenderers[i].setRenderMode(renderMode, mesh.getSubmeshes()[i]);
}

void MeshRenderer::setMaterial(MaterialPtr material) {
  m_materials.clear();
  m_materials.emplace_back(std::move(material));

  for (SubmeshRenderer& submeshRenderer : m_submeshRenderers)
    submeshRenderer.setMaterialIndex(0);
}

void MeshRenderer::setMaterial(MaterialPreset materialPreset, float roughnessFactor) {
  const MaterialCookTorrancePtr& newMaterial = Material::recoverMaterial(materialPreset, roughnessFactor);

  for (MaterialPtr& material : m_materials) {
    material->setBaseColor(newMaterial->getBaseColor());

    if (material->getType() == MaterialType::COOK_TORRANCE) {
      auto* materialCT = static_cast<MaterialCookTorrance*>(material.get());

      materialCT->setMetallicFactor(newMaterial->getMetallicFactor());
      materialCT->setRoughnessFactor(roughnessFactor);
      materialCT->setAlbedoMap(Texture::create(ColorPreset::WHITE, 0));
    } else {
      auto* materialBP = static_cast<MaterialBlinnPhong*>(material.get());

      const float specular = newMaterial->getMetallicFactor() * (1.f - roughnessFactor);
      materialBP->setSpecular(Vec3f(specular));
      materialBP->setDiffuseMap(Texture::create(ColorPreset::WHITE, 0));
    }
  }
}

void MeshRenderer::removeMaterial(std::size_t materialIndex) {
  assert("Error: Cannot remove a material that doesn't exist." && materialIndex < m_materials.size());

  m_materials.erase(m_materials.begin() + static_cast<std::ptrdiff_t>(materialIndex));

  for (SubmeshRenderer& submeshRenderer : m_submeshRenderers) {
    const std::size_t submeshMaterialIndex = submeshRenderer.getMaterialIndex();

    if (submeshMaterialIndex == std::numeric_limits<std::size_t>::max())
      continue;

    if (submeshMaterialIndex == materialIndex)
      submeshRenderer.setMaterialIndex(0);
    else if (submeshMaterialIndex > materialIndex)
      submeshRenderer.setMaterialIndex(submeshMaterialIndex - 1);
  }
}

MeshRenderer MeshRenderer::clone() const {
  MeshRenderer meshRenderer;

  meshRenderer.m_submeshRenderers.reserve(m_submeshRenderers.size());
  for (const SubmeshRenderer& submeshRenderer : m_submeshRenderers)
    meshRenderer.m_submeshRenderers.emplace_back(submeshRenderer.clone());

  meshRenderer.m_materials.reserve(m_materials.size());
  for (const MaterialPtr& material : m_materials)
    meshRenderer.m_materials.emplace_back(material->clone());

  return meshRenderer;
}

void MeshRenderer::load(const Mesh& mesh, RenderMode renderMode) {
  if (mesh.getSubmeshes().empty()) {
    Logger::error("[MeshRenderer] Cannot load an empty mesh.");
    return;
  }

  Logger::debug("[MeshRenderer] Loading mesh data...");

  m_submeshRenderers.resize(mesh.getSubmeshes().size());

  for (std::size_t submeshIndex = 0; submeshIndex < mesh.getSubmeshes().size(); ++submeshIndex)
    m_submeshRenderers[submeshIndex].load(mesh.getSubmeshes()[submeshIndex], renderMode);

  // If no material exists, create a default one
  if (m_materials.empty())
    setMaterial(MaterialCookTorrance::create());

  Logger::debug("[MeshRenderer] Loaded mesh data");
}

void MeshRenderer::load(const RenderShaderProgram& program) const {
  for (const MaterialPtr& material : m_materials)
    material->initTextures(program);
}

void MeshRenderer::load(const Mesh& mesh, const RenderShaderProgram& program, RenderMode renderMode) {
  load(mesh, renderMode);
  load(program);
}

void MeshRenderer::draw() const {
  for (const SubmeshRenderer& submeshRenderer : m_submeshRenderers)
    submeshRenderer.draw();
}

void MeshRenderer::draw(const RenderShaderProgram& program) const {
  for (const SubmeshRenderer& submeshRenderer : m_submeshRenderers) {
    if (submeshRenderer.getMaterialIndex() != std::numeric_limits<std::size_t>::max()) {
      assert("Error: Material index does not reference any existing material." && submeshRenderer.getMaterialIndex() < m_materials.size());

      const MaterialPtr& material = m_materials[submeshRenderer.getMaterialIndex()];

      if (material)
        material->bindAttributes(program);
    }

    submeshRenderer.draw();
  }
}

} // namespace Raz
