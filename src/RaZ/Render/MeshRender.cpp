#include "RaZ/Render/Mesh.hpp"

namespace Raz {

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
