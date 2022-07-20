#include "RaZ/Data/Submesh.hpp"

namespace Raz {

const AABB& Submesh::computeBoundingBox() {
  Vec3f minPos(std::numeric_limits<float>::max());
  Vec3f maxPos(std::numeric_limits<float>::lowest());

  for (const Vertex& vert : m_vertices) {
    minPos.x() = std::min(minPos.x(), vert.position.x());
    minPos.y() = std::min(minPos.y(), vert.position.y());
    minPos.z() = std::min(minPos.z(), vert.position.z());

    maxPos.x() = std::max(maxPos.x(), vert.position.x());
    maxPos.y() = std::max(maxPos.y(), vert.position.y());
    maxPos.z() = std::max(maxPos.z(), vert.position.z());
  }

  m_boundingBox = AABB(minPos, maxPos);
  return m_boundingBox;
}

} // namespace Raz
