#include "RaZ/Utils/Ray.hpp"

namespace Raz {

namespace {

bool solveQuadratic(float a, float b, float c, float& firstHitDist, float& secondHitDist) {
  const float discriminant = b * b - 4.f * a * c;

  if (discriminant < 0) {
    return false;
  } else if (discriminant == 0) {
    const float hitDist = -0.5f * b / a;

    firstHitDist  = hitDist;
    secondHitDist = hitDist;
  } else {
    const float q = -0.5f * ((b > 0) ? b + std::sqrt(discriminant) : b - std::sqrt(discriminant));

    firstHitDist  = q / a;
    secondHitDist = c / q;
  }

  if (firstHitDist > secondHitDist)
    std::swap(firstHitDist, secondHitDist);

  return true;
}

} // namespace

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

bool Ray::intersects(const Sphere& sphere) const {
  const Vec3f sphereDir = m_origin - sphere.getCenter();

  const float raySqLength = m_direction.dot(m_direction);
  const float rayDiff     = 2.f * m_direction.dot(sphereDir);
  const float sphereDiff  = sphereDir.computeSquaredLength() - sphere.getRadius() * sphere.getRadius();

  float firstHitDist {}, secondHitDist {};

  if (!solveQuadratic(raySqLength, rayDiff, sphereDiff, firstHitDist, secondHitDist))
    return false;

  // If the hit distances are negative, we've hit a sphere located behind the ray's origin
  return (firstHitDist > 0.f || secondHitDist > 0.f);
}

bool Ray::intersects(const Triangle& triangle) const {
  const Vec3f firstEdge   = triangle.getSecondPos() - triangle.getFirstPos();
  const Vec3f secondEdge  = triangle.getThirdPos() - triangle.getFirstPos();
  const Vec3f pVec        = m_direction.cross(secondEdge);
  const float determinant = firstEdge.dot(pVec);

  if (FloatUtils::checkNearEquality(std::abs(determinant), 0.f))
    return false;

  const float invDeterm = 1 / determinant;

  const Vec3f invPlaneDir    = m_origin - triangle.getFirstPos();
  const float firstBaryCoord = invPlaneDir.dot(pVec) * invDeterm;

  if (firstBaryCoord < 0.f || firstBaryCoord > 1.f)
    return false;

  const Vec3f qVec = invPlaneDir.cross(firstEdge);
  const float secondBaryCoord = qVec.dot(m_direction) * invDeterm;

  if (secondBaryCoord < 0.f || firstBaryCoord + secondBaryCoord > 1.f)
    return false;

  const float hitDist = secondEdge.dot(qVec) * invDeterm;

  return (hitDist > 0.f);
}

bool Ray::intersects(const Quad&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Ray::intersects(const AABB& aabb) const {
  const Vec3f invDir({ 1.f / m_direction[0], 1.f / m_direction[1], 1.f / m_direction[2] });
  Vec3f minPos = aabb.getLeftBottomBackPos();
  Vec3f maxPos = aabb.getRightTopFrontPos();

  if (m_direction[0] < 0.f)
    std::swap(minPos[0], maxPos[0]);

  if (m_direction[1] < 0.f)
    std::swap(minPos[1], maxPos[1]);

  if (m_direction[2] < 0.f)
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
