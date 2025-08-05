#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"
#include "GL/glew.h" // Needed by TracyOpenGL.hpp
#include "tracy/TracyOpenGL.hpp"

namespace Raz {

void MeshRenderer::setRenderMode(RenderMode renderMode, const Mesh& mesh) {
  for (std::size_t i = 0; i < m_submeshRenderers.size(); ++i)
    m_submeshRenderers[i].setRenderMode(renderMode, mesh.getSubmeshes()[i]);
}

Material& MeshRenderer::setMaterial(Material&& material) {
  ZoneScopedN("MeshRenderer::setMaterial");

  m_materials.clear();

  Material& newMaterial = m_materials.emplace_back(std::move(material));
  newMaterial.getProgram().sendAttributes();
  newMaterial.getProgram().initTextures();
#if !defined(USE_WEBGL)
  newMaterial.getProgram().initImageTextures();
#endif

  for (SubmeshRenderer& submeshRenderer : m_submeshRenderers)
    submeshRenderer.setMaterialIndex(0);

  return newMaterial;
}

void MeshRenderer::removeMaterial(std::size_t materialIndex) {
  assert("Error: Cannot remove a material that does not exist." && materialIndex < m_materials.size());

  m_materials.erase(m_materials.begin() + static_cast<std::ptrdiff_t>(materialIndex));

  for (SubmeshRenderer& submeshRenderer : m_submeshRenderers) {
    const std::size_t submeshMaterialIndex = submeshRenderer.getMaterialIndex();

    if (submeshMaterialIndex == std::numeric_limits<std::size_t>::max())
      continue;

    if (submeshMaterialIndex == materialIndex)
      submeshRenderer.setMaterialIndex(std::numeric_limits<std::size_t>::max());
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
  for (const Material& material : m_materials)
    meshRenderer.m_materials.emplace_back(material.clone());

  return meshRenderer;
}

void MeshRenderer::load(const Mesh& mesh, RenderMode renderMode) {
  ZoneScopedN("MeshRenderer::load");

  if (mesh.getSubmeshes().empty()) {
    Logger::error("[MeshRenderer] Cannot load an empty mesh");
    return;
  }

  Logger::debug("[MeshRenderer] Loading mesh data...");

  m_submeshRenderers.resize(mesh.getSubmeshes().size());

  for (std::size_t submeshIndex = 0; submeshIndex < mesh.getSubmeshes().size(); ++submeshIndex)
    m_submeshRenderers[submeshIndex].load(mesh.getSubmeshes()[submeshIndex], renderMode);

  // If no material exists, create a default one
  if (m_materials.empty())
    setMaterial(Material(MaterialType::COOK_TORRANCE));

  Logger::debug("[MeshRenderer] Loaded mesh data");
}

void MeshRenderer::loadMaterials() const {
  ZoneScopedN("MeshRenderer::loadMaterials");

  for (const Material& material : m_materials) {
    material.getProgram().sendAttributes();
    material.getProgram().initTextures();
#if !defined(USE_WEBGL)
    material.getProgram().initImageTextures();
#endif
  }
}

void MeshRenderer::draw() const {
  ZoneScopedN("MeshRenderer::draw");
  TracyGpuZone("MeshRenderer::draw")

  for (const SubmeshRenderer& submeshRenderer : m_submeshRenderers) {
    if (submeshRenderer.getMaterialIndex() != std::numeric_limits<std::size_t>::max()) {
      assert("Error: The material index does not reference any existing material." && (submeshRenderer.getMaterialIndex() < m_materials.size()));
      m_materials[submeshRenderer.getMaterialIndex()].getProgram().bindTextures();
    }

    submeshRenderer.draw();
  }
}

} // namespace Raz
