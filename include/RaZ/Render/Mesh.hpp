#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <memory>
#include <string>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Submesh.hpp"

namespace Raz {

class Mesh;
using MeshPtr = std::unique_ptr<Mesh>;

class Mesh {
public:
  Mesh() { m_submeshes.emplace_back(Submesh::create()); }

  const std::vector<SubmeshPtr>& getSubmeshes() const { return m_submeshes; }
  std::vector<SubmeshPtr>& getSubmeshes() { return m_submeshes; }
  const std::vector<MaterialPtr>& getMaterials() const { return m_materials; }
  std::vector<MaterialPtr>& getMaterials() { return m_materials; }
  std::size_t recoverVertexCount() const;
  std::size_t recoverTriangleCount() const;

  template <typename... Args> static MeshPtr create(Args&&... args) { return std::make_unique<Mesh>(std::forward<Args>(args)...); }
  static MeshPtr createTriangle(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos);
  static MeshPtr createQuad(const Vec3f& leftTopPos, const Vec3f& rightTopPos,
                            const Vec3f& rightBottomPos, const Vec3f& leftBottomPos);
  static MeshPtr createAABB(const Vec3f& rightTopFrontPos, const Vec3f& leftBottomBackPos);
  static void drawQuad();
  static void drawCube();

  void setMaterial(MaterialPreset materialPreset, float roughnessFactor);
  void addSubmesh(SubmeshPtr submesh) { m_submeshes.emplace_back(std::move(submesh)); }
  void addMaterial(MaterialPtr material) { m_materials.emplace_back(std::move(material)); }
  void load(const ShaderProgram* program = nullptr) const;
  void draw(const ShaderProgram* program = nullptr) const;

private:
  std::vector<SubmeshPtr> m_submeshes {};
  std::vector<MaterialPtr> m_materials {};
};

} // namespace Raz

#endif // RAZ_MESH_HPP
