#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include <array>
#include <initializer_list>

namespace Raz {

class Triangle {
public:
  Triangle(std::initializer_list<float> vertices);

  const std::array<float, 9>& getVertices() const { return vertices; }
  std::array<float, 9>& getVertices() { return vertices; }

private:
  std::array<float, 9> vertices;
};

class Rectangle {
public:
  Rectangle(std::initializer_list<float> vertices);

  const std::array<float, 18>& getVertices() const { return vertices; }
  std::array<float, 18>& getVertices() { return vertices; }

private:
  std::array<float, 18> vertices;
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
