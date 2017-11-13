#pragma once

#ifndef RAZ_MODEL_HPP
#define RAZ_MODEL_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Material.hpp"

namespace Raz {

class Model {
public:
  Model() = default;
  Model(MeshPtr mesh) : m_mesh{ std::move(mesh) } {}

  const Material& getMaterial() const { return m_material; }
  Material& getMaterial() { return m_material; }
  const Mat4f& getPosition() const { return m_position; }
  Mat4f& getPosition() { return m_position; }
  const MeshPtr getMesh() const { return m_mesh; }
  MeshPtr getMesh() { return m_mesh; }

  void setMaterial(const Material& material) { m_material = material; }

  void draw() const;
  void translate(float x, float y, float z);
  void rotate(float angle, float x, float y, float z);
  void scale(float x, float y, float z);
  void scale(float val) { scale(val, val, val); }

private:
  void updatePosition() const;

  Material m_material;
  Mat4f m_position = Mat4f::identity();
  MeshPtr m_mesh;
};

using ModelPtr = std::unique_ptr<Model>;

} // namespace Raz

#endif // RAZ_MODEL_HPP
