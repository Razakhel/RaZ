#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include <array>
#include <initializer_list>

namespace Raz {

class Triangle {
public:
  Triangle(std::initializer_list<float> vertices);

  const std::array<float, 9>& getVertices() const { return m_vertices; }
  std::array<float, 9>& getVertices() { return m_vertices; }

private:
  std::array<float, 9> m_vertices;
};

class Plane {
public:
  Plane(std::initializer_list<float> vertices);

  const std::array<float, 18>& getVertices() const { return m_vertices; }
  std::array<float, 18>& getVertices() { return m_vertices; }

private:
  std::array<float, 18> m_vertices;
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
