#pragma once

#ifndef RAZ_MODEL_HPP
#define RAZ_MODEL_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Material.hpp"

namespace Raz {

class Model {
public:
  Model() = default;
  Model(MeshPtr mesh) : m_mesh{ std::move(mesh) } {}

  const Material& getMaterial() const { return m_material; }
  Material& getMaterial() { return m_material; }
  const Transform& getTransform() const { return m_transform; }
  Transform& getTransform() { return m_transform; }
  const MeshPtr getMesh() const { return m_mesh; }
  MeshPtr getMesh() { return m_mesh; }

  void setMaterial(const Material& material) { m_material = material; }

  void draw() const;
  void translate(float x, float y, float z) { m_transform.translate(x, y, z); }
  void rotate(float angle, float x, float y, float z) { m_transform.rotate(angle, x, y, z); }
  void scale(float x, float y, float z) { m_transform.scale(x, y, z); }
  void scale(float val) { m_transform.scale(val, val, val); }

private:
  void updateTransform() const;

  Material m_material;
  Transform m_transform;
  MeshPtr m_mesh;
};

using ModelPtr = std::unique_ptr<Model>;

} // namespace Raz

#endif // RAZ_MODEL_HPP
