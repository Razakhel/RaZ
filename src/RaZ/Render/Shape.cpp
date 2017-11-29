#include <cassert>
#include <initializer_list>

#include "RaZ/Render/Shape.hpp"

namespace Raz {

Triangle::Triangle(const Vec3f& vert1, const Vec3f& vert2, const Vec3f& vert3) {
  m_vertices = { vert1, vert2, vert3 };

  // Process texcoords

  // Process normals

}

Plane::Plane(const Vec3f& topLeft, const Vec3f& topRight, const Vec3f& bottomRight, const Vec3f& bottomLeft) {
  m_vertices = { topLeft, topRight, bottomRight, bottomLeft };

  // Process texcoords

  // Process normals

}

} // namespace Raz
