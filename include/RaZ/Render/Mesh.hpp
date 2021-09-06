#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Submesh.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <memory>

namespace Raz {

class FilePath;

enum class SphereMeshType {
  UV = 0, ///< [UV sphere](https://en.wikipedia.org/wiki/UV_mapping).
  ICO     ///< [Icosphere/convex icosahedron](https://en.wikipedia.org/wiki/Geodesic_polyhedron).
};

class Mesh final : public Component {
public:
  Mesh() : m_submeshes(1) { m_materials.emplace_back(MaterialCookTorrance::create()); }
  explicit Mesh(const FilePath& filePath) { import(filePath); }
  Mesh(const Plane& plane, float width, float depth, RenderMode renderMode = RenderMode::TRIANGLE);
  /// Creates a mesh from a Sphere.
  /// \param sphere Sphere to create the mesh with.
  /// \param subdivCount Amount of subdivisions (for an UV sphere, represents both the amount of vertical & horizontal lines to be created).
  /// \param type Type of the sphere mesh to create.
  /// \param renderMode Mode in which to render the created mesh.
  explicit Mesh(const Sphere& sphere, uint32_t subdivCount, SphereMeshType type, RenderMode renderMode = RenderMode::TRIANGLE);
  explicit Mesh(const Triangle& triangle, RenderMode renderMode = RenderMode::TRIANGLE);
  explicit Mesh(const Quad& quad, RenderMode renderMode = RenderMode::TRIANGLE);
  explicit Mesh(const AABB& box, RenderMode renderMode = RenderMode::TRIANGLE);

  const std::vector<Submesh>& getSubmeshes() const { return m_submeshes; }
  std::vector<Submesh>& getSubmeshes() { return m_submeshes; }
  const std::vector<MaterialPtr>& getMaterials() const { return m_materials; }
  std::vector<MaterialPtr>& getMaterials() { return m_materials; }
  const AABB& getBoundingBox() const { return m_boundingBox; }

  void setSkeleton(Skeleton&& skeleton) noexcept;

  static void drawUnitPlane(const Vec3f& normal = Axis::Y);
  static void drawUnitSphere();
  static void drawUnitQuad();
  static void drawUnitCube();

  void import(const FilePath& filePath);
  void setRenderMode(RenderMode renderMode);
  void setMaterial(MaterialPtr&& material);
  void setMaterial(MaterialPreset materialPreset, float roughnessFactor);
  Submesh& addSubmesh(Submesh&& submesh = Submesh()) { return m_submeshes.emplace_back(std::move(submesh)); }
  Material& addMaterial(MaterialPtr&& material) { return *m_materials.emplace_back(std::move(material)); }
  std::size_t recoverVertexCount() const;
  std::size_t recoverTriangleCount() const;
  /// Computes & updates the mesh's bounding box by computing the submeshes' ones.
  /// \return Mesh's bounding box.
  const AABB& computeBoundingBox();
  void load() const;
  void load(const ShaderProgram& program) const;
  void draw() const;
  void draw(const ShaderProgram& program) const;
  void save(const FilePath& filePath) const;

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

  void importObj(std::ifstream& file, const FilePath& filePath);
  void importOff(std::ifstream& file);
#if defined(FBX_ENABLED)
  void importFbx(const FilePath& filePath);
#endif

  void saveObj(std::ofstream& file, const FilePath& filePath) const;

  std::vector<Submesh> m_submeshes {};
  std::optional<Skeleton> m_skeleton {};
  std::vector<MaterialPtr> m_materials {};
  AABB m_boundingBox = AABB(Vec3f(), Vec3f());
};

} // namespace Raz

#endif // RAZ_MESH_HPP
