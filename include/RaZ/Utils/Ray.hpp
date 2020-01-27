#pragma once

#ifndef RAZ_RAY_HPP
#define RAZ_RAY_HPP

#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Line;
class Plane;
class Sphere;
class Triangle;
class Quad;
class AABB;
class OBB;

/// Ray hit used to get informations from a ray intersection.
struct RayHit {
  Vec3f position {};
  Vec3f normal {};
  float distance = std::numeric_limits<float>::max();
};

/// Ray defined by an origin and a normalized direction.
class Ray {
public:
  Ray(const Vec3f& origin, const Vec3f& direction) : m_origin{ origin },
                                                     m_direction{ direction },
                                                     m_invDirection{ 1.f / direction[0], 1.f / direction[1], 1.f / direction[2] } {}

  const Vec3f& getOrigin() const { return m_origin; }
  const Vec3f& getDirection() const { return m_direction; }
  const Vec3f& getInverseDirection() const { return m_invDirection; }

  /// Ray-point intersection check.
  /// \param point Point to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the point, false otherwise.
  bool intersects(const Vec3f& point, RayHit* hit = nullptr) const;
  /// Ray-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the line, false otherwise.
  //bool intersects(const Line& line, RayHit* hit = nullptr) const;
  /// Ray-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the plane, false otherwise.
  bool intersects(const Plane& plane, RayHit* hit = nullptr) const;
  /// Ray-sphere intersection check.
  /// The intersection is checked by calculating a quadratic equation to determine the hits distances.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the sphere, false otherwise.
  bool intersects(const Sphere& sphere, RayHit* hit = nullptr) const;
  /// Ray-triangle intersection check.
  /// The intersection is checked by calculating the barycentic coordinates at the intersection point.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \note The hit normal will always be oriented towards the ray.
  /// \return True if the ray intersects the triangle, false otherwise.
  bool intersects(const Triangle& triangle, RayHit* hit = nullptr) const;
  /// Ray-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the quad, false otherwise.
  //bool intersects(const Quad& quad, RayHit* hit = nullptr) const;
  /// Ray-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \note If returns true with a negative hit distance, the ray is located inside the box & the hit position is the intersection point found behind the ray.
  /// \return True if the ray intersects the AABB, false otherwise.
  bool intersects(const AABB& aabb, RayHit* hit = nullptr) const;
  /// Ray-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the OBB, false otherwise.
  //bool intersects(const OBB& obb, RayHit* hit = nullptr) const;
  /// Computes the projection of a point (closest point) onto the ray.
  /// The projected point is necessarily located between the ray's origin and towards infinity in the ray's direction.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the ray.
  Vec3f computeProjection(const Vec3f& point) const;

private:
  Vec3f m_origin {};
  Vec3f m_direction {};
  Vec3f m_invDirection {};
};

} // namespace Raz

#endif // RAZ_RAY_HPP
