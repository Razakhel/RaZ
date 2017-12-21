#include "RaZ/Render/Model.hpp"

namespace Raz {

Model::Model(const Vec3f& topLeftPos, const Vec3f& topRightPos,
        const Vec3f& bottomRightPos, const Vec3f& bottomLeftPos,
        const Material& material) {
  m_mesh = std::make_shared<Mesh>(topLeftPos, topRightPos, bottomRightPos, bottomLeftPos);
  m_mesh->addMaterial(material);
}

} // namespace Raz
