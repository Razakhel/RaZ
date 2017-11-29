#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Render/Model.hpp"

namespace Raz {

void Model::draw() const {
  if (m_material.getTexture())
    m_material.getTexture()->bind();

  m_mesh->draw();
}

} // namespace Raz
