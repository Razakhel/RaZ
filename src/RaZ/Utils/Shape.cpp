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

bool Line::intersects(const AABB& aabb) const {
  const Ray lineRay(m_beginPos, (m_endPos - m_beginPos).normalize());
  RayHit hit;

  if (!lineRay.intersects(aabb, &hit))
    return false;

  // Some implementations check for the hit distance to be positive or 0. However, since our ray-AABB intersection check returns true
  //  with a negative distance when the ray's origin is inside the box, this check would be meaningless
  // Actually, if reaching here, none of the potential cases should require to check that the hit distance is non-negative

  // In certain cases, it's even harmful to do so. Given a line segment defined by points A & B, one being in a box & the other outside:
  //
  // ----------
  // |        |
  // |   A x-----x B
  // |        |
  // ----------
  //
  // Depending on the order of the points, the result would not be symmetrical: B->A would return a positive distance, telling there's an
  //  intersection, and A->B a negative distance, telling there's none

  return (hit.distance * hit.distance <= computeSquaredLength());
}

bool Line::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

void Line::translate(const Vec3f& displacement) noexcept {
  m_beginPos += displacement;
  m_endPos   += displacement;
}

Vec3f Line::computeProjection(const Vec3f& point) const {
  const Vec3f lineVec   = m_endPos - m_beginPos;
  const float pointDist = lineVec.dot(point - m_beginPos) / lineVec.computeSquaredLength();

  // Clamping pointDist between 0 & 1, since it can be outside these bounds if not directly projectable
  //
  //        < 0        |    >= 0 & <= 1    |        > 1
  // __________________________________________________________
  //                   |                   |
  // P                 |         P         |                  P
  // |                 |         |         |                  |
  // v                 |         v         |                  v
  //    A----------B   |   A----------B    |   A----------B

  return m_beginPos + lineVec * std::clamp(pointDist, 0.f, 1.f);
}

AABB Line::computeBoundingBox() const {
  const auto [xMin, xMax] = std::minmax(m_beginPos.x(), m_endPos.x());
  const auto [yMin, yMax] = std::minmax(m_beginPos.y(), m_endPos.y());
  const auto [zMin, zMax] = std::minmax(m_beginPos.z(), m_endPos.z());

  return AABB(Vec3f(xMin, yMin, zMin), Vec3f(xMax, yMax, zMax));
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

  const float topBoxDist = halfExtents.x() * std::abs(m_normal.x())
                         + halfExtents.y() * std::abs(m_normal.y())
                         + halfExtents.z() * std::abs(m_normal.z());
  const float boxDist = m_normal.dot(aabb.computeCentroid()) - m_distance;

  return (std::abs(boxDist) <= topBoxDist);
}

bool Plane::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

AABB Plane::computeBoundingBox() const {
  throw std::runtime_error("Error: Not implemented yet.");
}

// Sphere functions

bool Sphere::contains(const Vec3f& point) const {
  const float pointSqDist = (m_centerPos - point).computeSquaredLength();
  return (pointSqDist <= (m_radius * m_radius));
}

bool Sphere::intersects(const Sphere& sphere) const {
  const float sqDist   = (m_centerPos - sphere.getCenter()).computeSquaredLength();
  const float sumRadii = m_radius + sphere.getRadius();

  return (sqDist <= sumRadii * sumRadii);
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

AABB Sphere::computeBoundingBox() const {
  return AABB(m_centerPos - m_radius, m_centerPos + m_radius);
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

void Triangle::translate(const Vec3f& displacement) noexcept {
  m_firstPos  += displacement;
  m_secondPos += displacement;
  m_thirdPos  += displacement;
}

Vec3f Triangle::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

AABB Triangle::computeBoundingBox() const {
  const auto [xMin, xMax] = std::minmax({ m_firstPos.x(), m_secondPos.x(), m_thirdPos.x() });
  const auto [yMin, yMax] = std::minmax({ m_firstPos.y(), m_secondPos.y(), m_thirdPos.y() });
  const auto [zMin, zMax] = std::minmax({ m_firstPos.z(), m_secondPos.z(), m_thirdPos.z() });

  return AABB(Vec3f(xMin, yMin, zMin), Vec3f(xMax, yMax, zMax));
}

Vec3f Triangle::computeNormal() const {
  const Vec3f firstEdge  = m_secondPos - m_firstPos;
  const Vec3f secondEdge = m_thirdPos - m_firstPos;

  return firstEdge.cross(secondEdge).normalize();
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

void Quad::translate(const Vec3f& displacement) noexcept {
  m_leftTopPos     += displacement;
  m_rightTopPos    += displacement;
  m_rightBottomPos += displacement;
  m_leftBottomPos  += displacement;
}

Vec3f Quad::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

AABB Quad::computeBoundingBox() const {
  const auto [xMin, xMax] = std::minmax({ m_leftTopPos.x(), m_rightTopPos.x(), m_rightBottomPos.x(), m_leftBottomPos.x() });
  const auto [yMin, yMax] = std::minmax({ m_leftTopPos.y(), m_rightTopPos.y(), m_rightBottomPos.y(), m_leftBottomPos.y() });
  const auto [zMin, zMax] = std::minmax({ m_leftTopPos.z(), m_rightTopPos.z(), m_rightBottomPos.z(), m_leftBottomPos.z() });

  return AABB(Vec3f(xMin, yMin, zMin), Vec3f(xMax, yMax, zMax));
}

// AABB functions

bool AABB::contains(const Vec3f& point) const {
  const Vec3f localPoint  = point - computeCentroid();
  const Vec3f halfExtents = computeHalfExtents();

  return (std::abs(localPoint.x()) <= halfExtents.x() + std::numeric_limits<float>::epsilon()
       && std::abs(localPoint.y()) <= halfExtents.y() + std::numeric_limits<float>::epsilon()
       && std::abs(localPoint.z()) <= halfExtents.z() + std::numeric_limits<float>::epsilon());
}

bool AABB::intersects(const AABB& aabb) const {
  const Vec3f& minPoint1 = m_minPos;
  const Vec3f& maxPoint1 = m_maxPos;

  const Vec3f& minPoint2 = aabb.getMinPosition();
  const Vec3f& maxPoint2 = aabb.getMaxPosition();

  // We determine for each axis if there are extremities that are overlapping
  // If the max point of one AABB is further on an axis than the min point of the other, they intersect each other on this axis
  //
  //            max1
  //             v
  //    ----------
  //    |        |
  //    |     ----------
  //    |     |  |     |
  //    ------|---     |
  //          |        |
  //          ----------
  //          ^
  //        min2

  const bool intersectsX = (minPoint1.x() <= maxPoint2.x()) && (maxPoint1.x() >= minPoint2.x());
  const bool intersectsY = (minPoint1.y() <= maxPoint2.y()) && (maxPoint1.y() >= minPoint2.y());
  const bool intersectsZ = (minPoint1.z() <= maxPoint2.z()) && (maxPoint1.z() >= minPoint2.z());

  return (intersectsX && intersectsY && intersectsZ);
}

bool AABB::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

void AABB::translate(const Vec3f& displacement) noexcept {
  m_minPos += displacement;
  m_maxPos += displacement;
}

Vec3f AABB::computeProjection(const Vec3f& point) const {
  const float closestX = std::max(std::min(point.x(), m_maxPos.x()), m_minPos.x());
  const float closestY = std::max(std::min(point.y(), m_maxPos.y()), m_minPos.y());
  const float closestZ = std::max(std::min(point.z(), m_maxPos.z()), m_minPos.z());

  return Vec3f(closestX, closestY, closestZ);
}

// OBB functions

bool OBB::contains(const Vec3f& point) const {
  const Vec3f localPoint  = m_invRotation * (point - computeCentroid());
  const Vec3f halfExtents = m_aabb.computeHalfExtents();

  return (std::abs(localPoint.x()) <= halfExtents.x() + std::numeric_limits<float>::epsilon()
       && std::abs(localPoint.y()) <= halfExtents.y() + std::numeric_limits<float>::epsilon()
       && std::abs(localPoint.z()) <= halfExtents.z() + std::numeric_limits<float>::epsilon());

}

bool OBB::intersects(const OBB&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

Vec3f OBB::computeProjection(const Vec3f&) const {
  throw std::runtime_error("Error: Not implemented yet.");
}

AABB OBB::computeBoundingBox() const {
  throw std::runtime_error("Error: Not implemented yet.");
}

} // namespace Raz
