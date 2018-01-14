#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <memory>
#include <string>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Submesh.hpp"
#include "RaZ/Render/Material.hpp"

namespace Raz {

class Mesh {
public:
  Mesh() { m_submeshes.emplace_back(std::make_unique<Submesh>()); }
  Mesh(const Vec3f& leftPos, const Vec3f& topPos, const Vec3f& rightPos);
  Mesh(const Vec3f& topLeftPos, const Vec3f& topRightPos, const Vec3f& bottomRightPos, const Vec3f& bottomLeftPos);

  const std::vector<SubmeshPtr>& getSubmeshes() const { return m_submeshes; }
  std::vector<SubmeshPtr>& getSubmeshes() { return m_submeshes; }
  const std::vector<MaterialPtr>& getMaterials() const { return m_materials; }
  std::vector<MaterialPtr>& getMaterials() { return m_materials; }

  void addSubmesh(SubmeshPtr submesh) { m_submeshes.emplace_back(std::move(submesh)); }
  void addMaterial(MaterialPtr material) { m_materials.emplace_back(std::move(material)); }
  void load() const;
  void draw() const;

private:
  std::vector<SubmeshPtr> m_submeshes {};
  std::vector<MaterialPtr> m_materials {};
};

using MeshPtr = std::shared_ptr<Mesh>;

} // namespace Raz

#endif // RAZ_MESH_HPP
