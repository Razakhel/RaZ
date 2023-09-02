#pragma once

#ifndef RAZ_SUBMESH_HPP
#define RAZ_SUBMESH_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <vector>

namespace Raz {

struct Vertex {
  Vec3f position {};
  Vec2f texcoords {};
  Vec3f normal {};
  Vec3f tangent {};

  std::size_t operator()() const { return tangent.hash(normal.hash(texcoords.hash(position.hash(0)))); }
  bool operator==(const Vertex& vert) const noexcept { return (position == vert.position)
                                                           && (texcoords == vert.texcoords)
                                                           && (normal == vert.normal)
                                                           && (tangent == vert.tangent); }
};

class Submesh {
public:
  Submesh() noexcept = default;
  Submesh(const Submesh&) = delete;
  Submesh(Submesh&&) noexcept = default;

  const std::vector<Vertex>& getVertices() const { return m_vertices; }
  std::vector<Vertex>& getVertices() { return m_vertices; }
  std::size_t getVertexCount() const { return m_vertices.size(); }
  const std::vector<unsigned int>& getLineIndices() const { return m_lineIndices; }
  std::vector<unsigned int>& getLineIndices() { return m_lineIndices; }
  std::size_t getLineIndexCount() const { return m_lineIndices.size(); }
  const std::vector<unsigned int>& getTriangleIndices() const { return m_triangleIndices; }
  std::vector<unsigned int>& getTriangleIndices() { return m_triangleIndices; }
  std::size_t getTriangleIndexCount() const { return m_triangleIndices.size(); }
  const AABB& getBoundingBox() const { return m_boundingBox; }

  /// Computes & updates the submesh's bounding box.
  /// \return Submesh's bounding box.
  const AABB& computeBoundingBox();

  Submesh& operator=(const Submesh&) = delete;
  Submesh& operator=(Submesh&&) noexcept = default;

private:
  std::vector<Vertex> m_vertices {};
  std::vector<unsigned int> m_lineIndices {};
  std::vector<unsigned int> m_triangleIndices {};

  AABB m_boundingBox = AABB(Vec3f(0.f), Vec3f(0.f));
};

} // namespace Raz

#endif // RAZ_SUBMESH_HPP
