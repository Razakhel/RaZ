#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include <array>
#include <initializer_list>

namespace Raz {

class Triangle {
public:
  Triangle(const std::initializer_list<float>& vertices);

  const std::array<float, 9>& getVertices() { return vertices; }

private:
  std::array<float, 9> vertices;
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
