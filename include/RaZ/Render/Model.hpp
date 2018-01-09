#pragma once

#ifndef RAZ_MODEL_HPP
#define RAZ_MODEL_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Material.hpp"

namespace Raz {

class Model : public Transform {
public:
  Model(const Vec3f& topLeftPos, const Vec3f& topRightPos,
        const Vec3f& bottomRightPos, const Vec3f& bottomLeftPos,
        MaterialPtr material);
  explicit Model(MeshPtr mesh) : m_mesh{ std::move(mesh) } {}

  const MeshPtr getMesh() const { return m_mesh; }

  std::unique_ptr<Model> clone() const { return std::make_unique<Model>(*this); }
  void load() const { m_mesh->load(); }
  void draw() const { m_mesh->draw(); }

private:
  MeshPtr m_mesh;
};

using ModelPtr = std::unique_ptr<Model>;

} // namespace Raz

#endif // RAZ_MODEL_HPP
