#include "RaZ/Render/Mesh.hpp"

namespace Raz {

void Mesh::setMaterial(MaterialPreset material, float roughnessFactor) {
  m_materials.resize(1);
  m_materials.front() = Material::recoverMaterial(material, roughnessFactor);

  for (auto& submesh : m_submeshes)
    submesh->setMaterialIndex(0);
}

void Mesh::load(const ShaderProgram& program) const {
  for (const auto& submesh : m_submeshes)
    submesh->load();

  for (const auto& material : m_materials)
    material->initTextures(program);
}

void Mesh::draw(const ShaderProgram& program) const {
  for (const auto& submesh : m_submeshes) {
    if (!m_materials.empty()) {
      const auto& material = m_materials[submesh->getMaterialIndex()];

      if (material)
        material->bindAttributes(program);
    }

    submesh->draw();
  }
}

} // namespace Raz
