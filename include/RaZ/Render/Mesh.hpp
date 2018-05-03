#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <memory>
#include <string>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Submesh.hpp"

namespace Raz {

class Mesh {
public:
  Mesh() { m_submeshes.emplace_back(std::make_unique<Submesh>()); }

  const std::vector<SubmeshPtr>& getSubmeshes() const { return m_submeshes; }
  std::vector<SubmeshPtr>& getSubmeshes() { return m_submeshes; }
  const std::vector<MaterialPtr>& getMaterials() const { return m_materials; }
  std::vector<MaterialPtr>& getMaterials() { return m_materials; }

  static std::unique_ptr<Mesh> createTriangle(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos);
  static std::unique_ptr<Mesh> createQuad(const Vec3f& leftTopPos, const Vec3f& rightTopPos,
                                          const Vec3f& rightBottomPos, const Vec3f& leftBottomPos);
  static std::unique_ptr<Mesh> createAABB(const Vec3f& rightTopFrontPos, const Vec3f& leftBottomBackPos);

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

using MeshPtr = std::unique_ptr<Mesh>;

} // namespace Raz

#endif // RAZ_MESH_HPP
