#include "RaZ/Utils/FloatUtils.hpp"
#include "RaZ/Utils/Ray.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace Raz {

namespace {

constexpr bool solveQuadratic(float a, float b, float c, float& firstHitDist, float& secondHitDist) {
  const float discriminant = b * b - 4.f * a * c;

  if (discriminant < 0.f) {
    return false;
  } else if (discriminant > 0.f) {
    const float q = -0.5f * ((b > 0) ? b + std::sqrt(discriminant) : b - std::sqrt(discriminant));

    firstHitDist  = q / a;
    secondHitDist = c / q;
  } else { // discriminant == 0
    const float hitDist = -0.5f * b / a;

    firstHitDist  = hitDist;
    secondHitDist = hitDist;
  }

  if (firstHitDist > secondHitDist)
    std::swap(firstHitDist, secondHitDist);

  return true;
}

} // namespace

bool Ray::intersects(const Vec3f& point, RayHit* hit) const {
  if (point == m_origin) {
    if (hit) {
      hit->position = point;
      hit->distance = 0.f;
    }

    return true;
  }

  const Vec3f pointDir       = point - m_origin;
  const Vec3f normedPointDir = pointDir.normalize();

  if (!FloatUtils::areNearlyEqual(normedPointDir.dot(m_direction), 1.f))
    return false;

  if (hit) {
    hit->position = point;
    hit->normal   = -normedPointDir;
    hit->distance = pointDir.computeLength();
  }

  return true;
}

bool Ray::intersects(const Plane& plane, RayHit* hit) const {
  const float dirAngle = m_direction.dot(plane.getNormal());

  if (dirAngle >= 0.f) // The plane is facing in the same direction as the ray
    return false;

  const float origAngle = m_origin.dot(plane.getNormal());
  const float hitDist   = (plane.getDistance() - origAngle) / dirAngle;

  if (hitDist <= 0.f) // The plane is behind the ray
    return false;

  if (hit) {
    hit->position = m_origin + m_direction * hitDist;
    hit->normal   = plane.getNormal();
    hit->distance = hitDist;
  }

  return true;
}

bool Ray::intersects(const Sphere& sphere, RayHit* hit) const {
  const Vec3f sphereDir = m_origin - sphere.getCenter();

  const float raySqLength = m_direction.dot(m_direction);
  const float rayDiff     = 2.f * m_direction.dot(sphereDir);
  const float sphereDiff  = sphereDir.computeSquaredLength() - sphere.getRadius() * sphere.getRadius();

  float firstHitDist {};
  float secondHitDist {};

  if (!solveQuadratic(raySqLength, rayDiff, sphereDiff, firstHitDist, secondHitDist))
    return false;

  // If the hit distances are negative, we've hit a sphere located behind the ray's origin
  if (firstHitDist < 0.f) {
    firstHitDist = secondHitDist;

    if (firstHitDist < 0.f)
      return false;
  }

  if (hit) {
    const Vec3f hitPos = m_origin + m_direction * firstHitDist;

    hit->position = hitPos;
    hit->normal   = (hitPos - sphere.getCenter()).normalize();
    hit->distance = firstHitDist;
  }

  return true;
}

bool Ray::intersects(const Triangle& triangle, RayHit* hit) const {
  const Vec3f firstEdge   = triangle.getSecondPos() - triangle.getFirstPos();
  const Vec3f secondEdge  = triangle.getThirdPos() - triangle.getFirstPos();
  const Vec3f pVec        = m_direction.cross(secondEdge);
  const float determinant = firstEdge.dot(pVec);

  if (FloatUtils::areNearlyEqual(std::abs(determinant), 0.f))
    return false;

  const float invDeterm = 1.f / determinant;

  const Vec3f invPlaneDir    = m_origin - triangle.getFirstPos();
  const float firstBaryCoord = invPlaneDir.dot(pVec) * invDeterm;

  if (firstBaryCoord < 0.f || firstBaryCoord > 1.f)
    return false;

  const Vec3f qVec = invPlaneDir.cross(firstEdge);
  const float secondBaryCoord = qVec.dot(m_direction) * invDeterm;

  if (secondBaryCoord < 0.f || firstBaryCoord + secondBaryCoord > 1.f)
    return false;

  const float hitDist = secondEdge.dot(qVec) * invDeterm;

  if (hitDist <= 0.f)
    return false;

  if (hit) {
    hit->position = m_origin + m_direction * hitDist;

    const Vec3f normal = firstEdge.cross(secondEdge).normalize();

    // We want the normal facing the ray, not the opposite direction (no culling)
    // This may not be the ideal behavior; this may change when a real use case will be available
    hit->normal = (normal.dot(m_direction) > 0.f ? -normal : normal);

    hit->distance = hitDist;
  }

  return true;
}

bool Ray::intersects(const AABB& aabb, RayHit* hit) const {
  // Branchless algorithm based on Tavianator's:
  //  - https://tavianator.com/fast-branchless-raybounding-box-intersections/
  //  - https://tavianator.com/cgit/dimension.git/tree/libdimension/bvh/bvh.c#n196

  const Vec3f minDist = (aabb.getMinPosition() - m_origin) * m_invDirection;
  const Vec3f maxDist = (aabb.getMaxPosition() - m_origin) * m_invDirection;

  const float minDistX = std::min(minDist.x(), maxDist.x());
  const float maxDistX = std::max(minDist.x(), maxDist.x());

  const float minDistY = std::min(minDist.y(), maxDist.y());
  const float maxDistY = std::max(minDist.y(), maxDist.y());

  const float minDistZ = std::min(minDist.z(), maxDist.z());
  const float maxDistZ = std::max(minDist.z(), maxDist.z());

  const float minHitDist = std::max(minDistX, std::max(minDistY, minDistZ));
  const float maxHitDist = std::min(maxDistX, std::min(maxDistY, maxDistZ));

  if (maxHitDist < std::max(minHitDist, 0.f))
    return false;

  // If reaching here with a negative distance (minHitDist < 0), this means that the ray's origin is inside the box
  // Currently, in this case, the computed hit position represents the intersection behind the ray

  if (hit) {
    hit->position = m_origin + m_direction * minHitDist;

    // Normal computing method based on John Novak's: https://blog.johnnovak.net/2016/10/22/the-nim-ray-tracer-project-part-4-calculating-box-normals/
    const Vec3f hitDir = (hit->position - aabb.computeCentroid()) / aabb.computeHalfExtents();
    hit->normal = Vec3f(std::trunc(hitDir.x()), std::trunc(hitDir.y()), std::trunc(hitDir.z())).normalize();

    hit->distance = minHitDist;
  }

  return true;
}

Vec3f Ray::computeProjection(const Vec3f& point) const {
  const float pointDist = m_direction.dot(point - m_origin);
  return (m_origin + m_direction * std::max(pointDist, 0.f));
}

} // namespace Raz
