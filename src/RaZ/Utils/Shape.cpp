#include "RaZ/Utils/Shape.hpp"

namespace Raz {

// Line functions

bool Line::intersects(const Line&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Line::intersects(const Plane&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Line::intersects(const Sphere& sphere) const {
  const Vec3f projPoint = computeProjection(sphere.getCenter());
  return sphere.contains(projPoint);
}

bool Line::intersects(const Triangle&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Line::intersects(const Quad&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Line::intersects(const AABB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f Line::computeProjection(const Vec3f& point) const {
  const Vec3f lineVec   = m_endPos - m_beginPos;
  const float pointDist = lineVec.dot(point - m_beginPos) / lineVec.dot(lineVec);

  // Clamping pointDist between 0 & 1, since it can be outside these bounds if not directly projectable
  //
  //         <0        |        ==0        |        >0
  // __________________________________________________________
  //                   |                   |
  // P                 |         P         |                  P
  // |                 |         |         |                  |
  // v                 |         v         |                  v
  //    A----------B   |   A----------B    |   A----------B

  return m_beginPos + lineVec * std::min(1.f, std::max(pointDist, 0.f));
}

} // namespace Raz
