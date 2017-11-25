#pragma once

#ifndef RAZ_MODEL_HPP
#define RAZ_MODEL_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Movable.hpp"
#include "RaZ/Render/Material.hpp"

namespace Raz {

class Model : public Movable {
public:
  Model() = default;
  explicit Model(MeshPtr mesh) : m_mesh{ std::move(mesh) } {}

  const Material& getMaterial() const { return m_material; }
  Material& getMaterial() { return m_material; }
  const MeshPtr getMesh() const { return m_mesh; }
  MeshPtr getMesh() { return m_mesh; }

  void setMaterial(const Material& material) { m_material = material; }

  void draw() const;

private:
  Material m_material;
  MeshPtr m_mesh;
};

using ModelPtr = std::unique_ptr<Model>;

} // namespace Raz

#endif // RAZ_MODEL_HPP
