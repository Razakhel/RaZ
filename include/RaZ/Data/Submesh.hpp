#pragma once

#ifndef RAZ_SUBMESH_HPP
#define RAZ_SUBMESH_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <vector>

namespace Raz {

struct Vertex {
  Vec3f position;
  Vec2f texcoords;
  Vec3f normal;
  Vec3f tangent;

  constexpr bool strictlyEquals(const Vertex& vert) const noexcept { return position.strictlyEquals(vert.position)
                                                                         && texcoords.strictlyEquals(vert.texcoords)
                                                                         && normal.strictlyEquals(vert.normal)
                                                                         && tangent.strictlyEquals(vert.tangent); }

  constexpr bool operator==(const Vertex& vert) const noexcept { return (position == vert.position)
                                                                     && (texcoords == vert.texcoords)
                                                                     && (normal == vert.normal)
                                                                     && (tangent == vert.tangent); }
  constexpr bool operator!=(const Vertex& vert) const noexcept { return !(*this == vert); }
};

inline std::ostream& operator<<(std::ostream& stream, const Vertex& vert) {
  stream << "{\n"
         << "  " << vert.position << '\n'
         << "  " << vert.texcoords << '\n'
         << "  " << vert.normal << '\n'
         << "  " << vert.tangent << '\n'
         << '}';
  return stream;
}

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
  /// Computes the tangents for each of the submesh's vertices.
  void computeTangents();

  Submesh& operator=(const Submesh&) = delete;
  Submesh& operator=(Submesh&&) noexcept = default;

private:
  std::vector<Vertex> m_vertices {};
  std::vector<unsigned int> m_lineIndices {};
  std::vector<unsigned int> m_triangleIndices {};

  AABB m_boundingBox = AABB(Vec3f(0.f), Vec3f(0.f));
};

} // namespace Raz

/// Specialization of std::hash for Vertex.
template <>
struct std::hash<Raz::Vertex> {
  /// Computes the hash of the given vertex.
  /// \param vert Vertex to compute the hash of.
  /// \return Vertex's hash value.
  constexpr std::size_t operator()(const Raz::Vertex& vert) const noexcept {
    return vert.tangent.hash(vert.normal.hash(vert.texcoords.hash(vert.position.hash(0))));
  }
};

#endif // RAZ_SUBMESH_HPP
