#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Data/Submesh.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <memory>

namespace Raz {

class FilePath;

enum class SphereMeshType {
  UV = 0, ///< [UV sphere](https://en.wikipedia.org/wiki/UV_mapping).
  ICO     ///< [Icosphere/convex icosahedron](https://en.wikipedia.org/wiki/Geodesic_polyhedron).
};

class Mesh : public Component {
public:
  Mesh() = default;
  Mesh(const Plane& plane, float width, float depth);
  /// Creates a mesh from a Sphere.
  /// \param sphere Sphere to create the mesh with.
  /// \param subdivCount Amount of subdivisions (for an UV sphere, represents both the amount of vertical & horizontal lines to be created).
  /// \param type Type of the sphere mesh to create.
  Mesh(const Sphere& sphere, uint32_t subdivCount, SphereMeshType type);
  Mesh(const Triangle& triangle, const Vec2f& firstTexcoords, const Vec2f& secondTexcoords, const Vec2f& thirdTexcoords);
  explicit Mesh(const Quad& quad);
  explicit Mesh(const AABB& box);
  Mesh(const Mesh&) = delete;
  Mesh(Mesh&&) noexcept = default;

  const std::vector<Submesh>& getSubmeshes() const { return m_submeshes; }
  std::vector<Submesh>& getSubmeshes() { return m_submeshes; }
  const AABB& getBoundingBox() const { return m_boundingBox; }
  std::size_t recoverVertexCount() const;
  std::size_t recoverTriangleCount() const;

  template <typename... Args> Submesh& addSubmesh(Args&&... args) { return m_submeshes.emplace_back(std::forward<Args>(args)...); }
  /// Computes & updates the mesh's bounding box by computing the submeshes' ones.
  /// \return Mesh's bounding box.
  const AABB& computeBoundingBox();

  Mesh& operator=(const Mesh&) = delete;
  Mesh& operator=(Mesh&&) noexcept = default;

private:
  /// Creates an UV sphere mesh from a Sphere.
  ///
  ///          /-----------\
  ///        / / / / | / \ / \
  ///      /-------------------\
  ///     |/ | / | / | / | / | /|
  ///     |---------------------| < latitude/height
  ///     |/ | / | / | / | / | /|
  ///      \-------------------/
  ///        \ / \ / | / / / /
  ///          \-----^-----/
  ///                |
  ///                longitude/width
  ///
  /// \param sphere Sphere to create the mesh with.
  /// \param widthCount Amount of vertical lines to be created (longitude).
  /// \param heightCount Amount of horizontal lines to be created (latitude).
  void createUvSphere(const Sphere& sphere, uint32_t widthCount, uint32_t heightCount);
  /// Creates an icosphere mesh from a Sphere.
  /// \param sphere Sphere to create the mesh with.
  /// \param subdivCount Amount of subdivisions to apply to the mesh.
  void createIcosphere(const Sphere& sphere, uint32_t subdivCount);

  std::vector<Submesh> m_submeshes {};
  AABB m_boundingBox = AABB(Vec3f(), Vec3f());
};

} // namespace Raz

#endif // RAZ_MESH_HPP
