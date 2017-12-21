#include "RaZ/Render/Mesh.hpp"

namespace Raz {

void Mesh::load() const {
  for (const auto& submesh : m_submeshes)
    submesh->load();
}

void Mesh::draw() const {
  for (const auto& submesh : m_submeshes) {
    glActiveTexture(GL_TEXTURE0);
    m_materials[submesh->getMaterialIndex()].getTexture()->bind();
    submesh->draw();
  }
}

} // namespace Raz
