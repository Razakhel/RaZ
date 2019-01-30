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

// Plane functions

bool Plane::intersects(const Plane& plane) const {
  const float planesAngle = m_normal.dot(plane.getNormal());
  return !FloatUtils::checkNearEquality(std::abs(planesAngle), 1.f);
}

bool Plane::intersects(const Sphere& sphere) const {
  const Vec3f projPoint = computeProjection(sphere.getCenter());
  return sphere.contains(projPoint);
}

bool Plane::intersects(const Triangle&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Plane::intersects(const Quad&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Plane::intersects(const AABB& aabb) const {
  const Vec3f halfExtents = aabb.computeHalfExtents();

  const float topBoxDist = halfExtents[0] * std::abs(m_normal[0])
                         + halfExtents[1] * std::abs(m_normal[1])
                         + halfExtents[2] * std::abs(m_normal[2]);
  const float boxDist = m_normal.dot(aabb.computeCentroid()) - m_distance;

  return (std::abs(boxDist) <= topBoxDist);
}

// Sphere functions

bool Sphere::contains(const Raz::Vec3f& point) const {
  const float pointSqDist = (m_centerPos - point).computeSquaredLength();
  return (pointSqDist <= (m_radius * m_radius));
}

bool Sphere::intersects(const Sphere& sphere) const {
  const float sqDist  = (m_centerPos - sphere.getCenter()).computeSquaredLength();
  const float sqRadii = (m_radius * m_radius) + (sphere.getRadius() * sphere.getRadius());

  return (sqDist <= sqRadii);
}

bool Sphere::intersects(const Triangle& triangle) const {
  const Vec3f projPoint = triangle.computeProjection(m_centerPos);
  return contains(projPoint);
}

bool Sphere::intersects(const Quad& quad) const {
  const Vec3f projPoint = quad.computeProjection(m_centerPos);
  return contains(projPoint);
}

bool Sphere::intersects(const AABB& aabb) const {
  const Vec3f projPoint = aabb.computeProjection(m_centerPos);
  return contains(projPoint);
}

// Triangle functions

bool Triangle::intersects(const Triangle&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Triangle::intersects(const Quad&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Triangle::intersects(const AABB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f Triangle::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

// Quad functions

bool Quad::intersects(const Quad&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Quad::intersects(const AABB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f Quad::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

// AABB functions

bool AABB::contains(const Vec3f& point) const {
  const bool isInBoundsX = point[0] >= m_leftBottomBackPos[0] && point[0] <= m_rightTopFrontPos[0];
  const bool isInBoundsY = point[1] >= m_leftBottomBackPos[1] && point[1] <= m_rightTopFrontPos[1];
  const bool isInBoundsZ = point[2] >= m_leftBottomBackPos[2] && point[2] <= m_rightTopFrontPos[2];

  return (isInBoundsX && isInBoundsY && isInBoundsZ);
}

bool AABB::intersects(const AABB& aabb) const {
  const Vec3f& minPoint1 = m_leftBottomBackPos;
  const Vec3f& maxPoint1 = m_rightTopFrontPos;

  const Vec3f& minPoint2 = aabb.getLeftBottomBackPos();
  const Vec3f& maxPoint2 = aabb.getRightTopFrontPos();

  // We determine for each axis if there are extremities that are overlapping
  // If the max point of one AABB is further on an axis than the min point of the other, they intersect each other on this axis
  //
  //                         max1
  //                          v
  //    -----------------------
  //    |                     |
  //    |                -------------------
  //    |                |    |            |
  //    |                |    |            |
  //    |                |    |            |
  //    |                |    |            |
  //    -----------------|-----            |
  //                     |                 |
  //                     -------------------
  //                     ^
  //                   min2

  const bool intersectsX = (minPoint1[0] <= maxPoint2[0]) && (maxPoint1[0] >= minPoint2[0]);
  const bool intersectsY = (minPoint1[1] <= maxPoint2[1]) && (maxPoint1[1] >= minPoint2[1]);
  const bool intersectsZ = (minPoint1[2] <= maxPoint2[2]) && (maxPoint1[2] >= minPoint2[2]);

  return (intersectsX && intersectsY && intersectsZ);
}

Vec3f AABB::computeProjection(const Vec3f& point) const {
  const float closestX = std::max(std::min(point[0], m_rightTopFrontPos[0]), m_leftBottomBackPos[0]);
  const float closestY = std::max(std::min(point[1], m_rightTopFrontPos[1]), m_leftBottomBackPos[1]);
  const float closestZ = std::max(std::min(point[2], m_rightTopFrontPos[2]), m_leftBottomBackPos[2]);

  return Vec3f({ closestX, closestY, closestZ });
}

} // namespace Raz
