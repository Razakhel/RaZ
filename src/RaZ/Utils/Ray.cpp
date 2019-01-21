#include "RaZ/Utils/Ray.hpp"

namespace Raz {

bool Ray::intersects(const Vec3f& point) const {
  if (point == m_origin)
    return true;

  const Vec3f pointDir = (point - m_origin).normalize();

  return FloatUtils::checkNearEquality(pointDir.dot(m_direction), 1.f);
}

bool Ray::intersects(const Line&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Ray::intersects(const Plane&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Ray::intersects(const Sphere&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Ray::intersects(const Triangle&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Ray::intersects(const Quad&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Ray::intersects(const AABB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f Ray::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

} // namespace Raz
