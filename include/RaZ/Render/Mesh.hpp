#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Submesh.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <memory>
#include <string>

namespace Raz {

class Mesh : public Component {
public:
  Mesh() : m_submeshes(1) {}
  explicit Mesh(const std::string& filePath) : Mesh() { import(filePath); }
  Mesh(const Plane& plane, float width, float depth, RenderMode renderMode = RenderMode::TRIANGLE);
  /// Creates a mesh from a Sphere.
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
  /// \param renderMode Mode in which to render the created mesh.
  explicit Mesh(const Sphere& sphere, uint32_t widthCount, uint32_t heightCount, RenderMode renderMode = RenderMode::TRIANGLE);
  explicit Mesh(const Triangle& triangle, RenderMode renderMode = RenderMode::TRIANGLE);
  explicit Mesh(const Quad& quad, RenderMode renderMode = RenderMode::TRIANGLE);
  explicit Mesh(const AABB& box, RenderMode renderMode = RenderMode::TRIANGLE);

  const std::vector<Submesh>& getSubmeshes() const { return m_submeshes; }
  std::vector<Submesh>& getSubmeshes() { return m_submeshes; }
  const std::vector<MaterialPtr>& getMaterials() const { return m_materials; }
  std::vector<MaterialPtr>& getMaterials() { return m_materials; }
  const AABB& getBoundingBox() const { return m_boundingBox; }
  std::size_t recoverVertexCount() const;
  std::size_t recoverTriangleCount() const;

  static void drawUnitPlane(const Vec3f& normal = Axis::Y);
  static void drawUnitSphere();
  static void drawUnitQuad();
  static void drawUnitCube();

  void import(const std::string& filePath);
  void setRenderMode(RenderMode renderMode);
  void setMaterial(MaterialPreset materialPreset, float roughnessFactor);
  void addSubmesh(Submesh submesh = Submesh()) { m_submeshes.emplace_back(std::move(submesh)); }
  void addMaterial(MaterialPtr material) { m_materials.emplace_back(std::move(material)); }
  /// Computes & updates the mesh's bounding box by computing the submeshes' ones.
  /// \return Mesh's bounding box.
  const AABB& computeBoundingBox();
  void load() const;
  void load(const ShaderProgram& program) const;
  void draw() const;
  void draw(const ShaderProgram& program) const;
  void save(const std::string& filePath) const;

private:
  void importObj(std::ifstream& file, const std::string& filePath);
  void importOff(std::ifstream& file);
#if defined(FBX_ENABLED)
  void importFbx(const std::string& filePath);
#endif

  void saveObj(std::ofstream& file, const std::string& filePath) const;

  std::vector<Submesh> m_submeshes {};
  std::vector<MaterialPtr> m_materials {};
  AABB m_boundingBox {};
};

} // namespace Raz

#endif // RAZ_MESH_HPP
