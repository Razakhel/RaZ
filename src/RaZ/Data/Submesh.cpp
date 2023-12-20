#include "RaZ/Data/Submesh.hpp"

namespace Raz {

namespace {

constexpr Vec3f computeTangent(const Vertex& firstVert, const Vertex& secondVert, const Vertex& thirdVert) noexcept {
  const Vec3f firstEdge  = secondVert.position - firstVert.position;
  const Vec3f secondEdge = thirdVert.position - firstVert.position;

  const Vec2f firstUvDiff  = secondVert.texcoords - firstVert.texcoords;
  const Vec2f secondUvDiff = thirdVert.texcoords - firstVert.texcoords;

  const float denominator = (firstUvDiff.x() * secondUvDiff.y() - secondUvDiff.x() * firstUvDiff.y());

  if (denominator == 0.f)
    return Vec3f(0.f);

  return (firstEdge * secondUvDiff.y() - secondEdge * firstUvDiff.y()) / denominator;
}

} // namespace

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

void Submesh::computeTangents() {
  for (Vertex& vert : m_vertices)
    vert.tangent = Vec3f(0.f, 0.f, 0.f);

  for (std::size_t i = 0; i < m_triangleIndices.size(); i += 3) {
    Vertex& firstVert  = m_vertices[m_triangleIndices[i    ]];
    Vertex& secondVert = m_vertices[m_triangleIndices[i + 1]];
    Vertex& thirdVert  = m_vertices[m_triangleIndices[i + 2]];

    const Vec3f tangent = computeTangent(firstVert, secondVert, thirdVert);

    // Adding the computed tangent to each vertex; they will be normalized later
    firstVert.tangent  += tangent;
    secondVert.tangent += tangent;
    thirdVert.tangent  += tangent;
  }

  // Normalizing the accumulated tangents
  for (Vertex& vert : m_vertices) {
    // Avoiding NaNs by preventing the normalization of a 0 vector
    if (vert.tangent.strictlyEquals(Vec3f(0.f)))
      continue;

    vert.tangent = (vert.tangent - vert.normal * vert.tangent.dot(vert.normal)).normalize();
  }
}

} // namespace Raz
