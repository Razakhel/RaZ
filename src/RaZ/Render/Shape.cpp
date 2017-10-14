#include <cassert>
#include <initializer_list>

#include "RaZ/Render/Shape.hpp"

namespace Raz {

Triangle::Triangle(std::initializer_list<float> verticesList) {
  assert(("Error: Too many/few parameters to describe a triangle.", verticesList.size() == vertices.size()));

  auto vertex = verticesList.begin();

  for (std::size_t vertIndex = 0; vertex != verticesList.end(); ++vertex, ++vertIndex)
    vertices[vertIndex] = *vertex;
}

Rectangle::Rectangle(std::initializer_list<float> verticesList) {
  assert(("Error: Too many/few parameters to describe a rectangle.", verticesList.size() == vertices.size()));

  auto vertex = verticesList.begin();

  for (std::size_t vertIndex = 0; vertex != verticesList.end(); ++vertex, ++vertIndex)
    vertices[vertIndex] = *vertex;
}

} // namespace Raz
