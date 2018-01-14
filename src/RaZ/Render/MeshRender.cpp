#include "RaZ/Render/Mesh.hpp"

namespace Raz {

void Mesh::load() const {
  for (const auto& submesh : m_submeshes)
    submesh->load();
}

void Mesh::draw() const {
  for (const auto& submesh : m_submeshes) {
    if (!m_materials.empty()) {
      const auto& material = m_materials[submesh->getMaterialIndex()];

      if (material && material->getDiffuseMap()) {
        glActiveTexture(GL_TEXTURE0);
        material->getDiffuseMap()->bind();
      }
    }

    submesh->draw();
  }
}

} // namespace Raz
