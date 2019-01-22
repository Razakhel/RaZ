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

bool Ray::intersects(const AABB& aabb) const {
  const Vec3f invDir({ 1.f / m_direction[0], 1.f / m_direction[1], 1.f / m_direction[2] });
  Vec3f minPos = aabb.getLeftBottomBackPos();
  Vec3f maxPos = aabb.getRightTopFrontPos();

  if (m_direction[0] < 0)
    std::swap(minPos[0], maxPos[0]);

  if (m_direction[1] < 0)
    std::swap(minPos[1], maxPos[1]);

  if (m_direction[2] < 0)
    std::swap(minPos[2], maxPos[2]);

  const Vec3f minHitPos = (minPos - m_origin) * invDir;
  const Vec3f maxHitPos = (maxPos - m_origin) * invDir;

  const float minHitDist = std::max(minHitPos[0], std::max(minHitPos[1], std::max(minHitPos[2], 0.f)));
  const float maxHitDist = std::min(maxHitPos[0], std::min(maxHitPos[1], maxHitPos[2]));

  return (minHitDist <= maxHitDist);
}

Vec3f Ray::computeProjection(const Vec3f& point) const {
  const float pointDist = m_direction.dot(point - m_origin);
  return (m_origin + m_direction * std::max(pointDist, 0.f));
}

} // namespace Raz
