#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include <array>
#include <initializer_list>

#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Triangle {
public:
  Triangle(const Vec3f& vert1, const Vec3f& vert2, const Vec3f& vert3);

  const std::array<Vec3f, 3>& getVertices() const { return m_vertices; }
  std::array<Vec3f, 3>& getVertices() { return m_vertices; }
  const std::array<Vec2f, 3>& getTexcoords() const { return m_texcoords; }
  std::array<Vec2f, 3>& getTexcoords() { return m_texcoords; }
  const std::array<Vec3f, 3>& getNormals() const { return m_normals; }
  std::array<Vec3f, 3>& getNormals() { return m_normals; }

private:
  std::array<Vec3f, 3> m_vertices {};
  std::array<Vec2f, 3> m_texcoords {};
  std::array<Vec3f, 3> m_normals {};
};

class Plane {
public:
  Plane(const Vec3f& topLeft, const Vec3f& topRight, const Vec3f& bottomRight, const Vec3f& bottomLeft);

  const std::array<Vec3f, 4>& getVertices() const { return m_vertices; }
  std::array<Vec3f, 4>& getVertices() { return m_vertices; }
  const std::array<Vec2f, 4>& getTexcoords() const { return m_texcoords; }
  std::array<Vec2f, 4>& getTexcoords() { return m_texcoords; }
  const std::array<Vec3f, 4>& getNormals() const { return m_normals; }
  std::array<Vec3f, 4>& getNormals() { return m_normals; }

private:
  std::array<Vec3f, 4> m_vertices {};
  std::array<Vec2f, 4> m_texcoords {};
  std::array<Vec3f, 4> m_normals {};
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
