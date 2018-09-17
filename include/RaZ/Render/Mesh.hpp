#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <memory>
#include <string>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Submesh.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace Raz {

class Mesh;
using MeshPtr = std::unique_ptr<Mesh>;

class Mesh {
public:
  Mesh() { m_submeshes.emplace_back(Submesh::create()); }
  explicit Mesh(const std::string& filePath) : Mesh() { import(filePath); }
  explicit Mesh(const Triangle& triangle);
  explicit Mesh(const Quad& quad);
  explicit Mesh(const AABB& box);

  const std::vector<SubmeshPtr>& getSubmeshes() const { return m_submeshes; }
  std::vector<SubmeshPtr>& getSubmeshes() { return m_submeshes; }
  const std::vector<MaterialPtr>& getMaterials() const { return m_materials; }
  std::vector<MaterialPtr>& getMaterials() { return m_materials; }
  std::size_t recoverVertexCount() const;
  std::size_t recoverTriangleCount() const;

  template <typename... Args> static MeshPtr create(Args&&... args) { return std::make_unique<Mesh>(std::forward<Args>(args)...); }
  static void drawUnitQuad();
  static void drawUnitCube();

  void import(const std::string& filePath);
  void setMaterial(MaterialPreset materialPreset, float roughnessFactor);
  void addSubmesh(SubmeshPtr submesh) { m_submeshes.emplace_back(std::move(submesh)); }
  void addMaterial(MaterialPtr material) { m_materials.emplace_back(std::move(material)); }
  void load(const ShaderProgram* program = nullptr) const;
  void draw(const ShaderProgram* program = nullptr) const;
  void save(const std::string& filePath) const;

private:
  void importObj(std::ifstream& file, const std::string& filePath);
  void importOff(std::ifstream& file);
#if defined(FBX_ENABLED)
  void importFbx(const std::string& filePath);
#endif

  void saveObj(std::ofstream& file, const std::string& filePath) const;

  std::vector<SubmeshPtr> m_submeshes {};
  std::vector<MaterialPtr> m_materials {};
};

} // namespace Raz

#endif // RAZ_MESH_HPP
