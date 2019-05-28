#include "RaZ/Utils/Shape.hpp"

namespace Raz {

// Line functions

bool Line::intersects(const Line&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Line::intersects(const Plane& plane) const {
  const Vec3f lineVec = m_endPos - m_beginPos;
  const float lineVecPlaneAngle = lineVec.dot(plane.getNormal());

  // If near 0, the line & the plane are parallel to each other
  if (FloatUtils::areNearlyEqual(lineVecPlaneAngle, 0.f))
    return false;

  const float lineStartPlaneAngle = m_beginPos.dot(plane.getNormal());

  // Calculating the relative distance along the line where it is intersected by the plane
  // If this distance is below 0 or above 1, the intersection isn't between the line's two extremities
  const float intersectDist = (plane.getDistance() - lineStartPlaneAngle) / lineVecPlaneAngle;
  return ((intersectDist >= 0.f) && (intersectDist <= 1.f));
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

bool Line::intersects(const OBB&) const {
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
  return !FloatUtils::areNearlyEqual(std::abs(planesAngle), 1.f);
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

bool Plane::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

// Sphere functions

bool Sphere::contains(const Vec3f& point) const {
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

bool Sphere::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
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

bool Triangle::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f Triangle::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f Triangle::computeNormal() const {
  const Vec3f firstEdge  = m_secondPos - m_firstPos;
  const Vec3f secondEdge = m_thirdPos - m_firstPos;

  return firstEdge.cross(secondEdge);
}

void Triangle::makeCounterClockwise(const Vec3f& normal) {
  if (isCounterClockwise(normal))
    return;

  // It doesn't matter which ones are swapped, as long as two of them are
  // The 3 points being adjacent, the ordering will be reversed all the same
  std::swap(m_firstPos, m_secondPos);
}

// Quad functions

bool Quad::intersects(const Quad&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Quad::intersects(const AABB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool Quad::intersects(const OBB&) const {
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

bool AABB::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f AABB::computeProjection(const Vec3f& point) const {
  const float closestX = std::max(std::min(point[0], m_rightTopFrontPos[0]), m_leftBottomBackPos[0]);
  const float closestY = std::max(std::min(point[1], m_rightTopFrontPos[1]), m_leftBottomBackPos[1]);
  const float closestZ = std::max(std::min(point[2], m_rightTopFrontPos[2]), m_leftBottomBackPos[2]);

  return Vec3f({ closestX, closestY, closestZ });
}

// OBB functions

void OBB::setRotation(const Mat3f& rotation) {
  m_rotation    = rotation;
  m_invRotation = m_rotation.inverse();
}

bool OBB::contains(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

bool OBB::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f OBB::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

} // namespace Raz
