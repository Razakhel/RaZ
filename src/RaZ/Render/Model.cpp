#include "RaZ/Render/Model.hpp"

namespace Raz {

void Model::draw() const {
  if (m_material.getTexture()) {
    glActiveTexture(GL_TEXTURE0);
    m_material.getTexture()->bind();
  }

  m_mesh->draw();
}

} // namespace Raz
