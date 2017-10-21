#include <cassert>
#include <initializer_list>

#include "RaZ/Render/Shape.hpp"

namespace Raz {

Triangle::Triangle(std::initializer_list<float> verticesList) {
  assert(("Error: Too many/few parameters to describe a triangle.", verticesList.size() == m_vertices.size()));

  auto vertex = verticesList.begin();

  for (std::size_t vertIndex = 0; vertex != verticesList.end(); ++vertex, ++vertIndex)
    m_vertices[vertIndex] = *vertex;
}

Plane::Plane(std::initializer_list<float> verticesList) {
  assert(("Error: Too many/few parameters to describe a rectangle.", verticesList.size() == m_vertices.size()));

  auto vertex = verticesList.begin();

  for (std::size_t vertIndex = 0; vertex != verticesList.end(); ++vertex, ++vertIndex)
    m_vertices[vertIndex] = *vertex;
}

} // namespace Raz
