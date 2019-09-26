#pragma once

#ifndef RAZ_RAY_HPP
#define RAZ_RAY_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace Raz {

/// Ray hit used to get informations from a ray intersection.
struct RayHit {
  Vec3f position {};
  Vec3f normal {};
  float distance = std::numeric_limits<float>::max();
};

/// Ray defined by an origin and a normalized direction.
class Ray {
public:
  Ray(const Vec3f& origin, const Vec3f& direction) : m_origin{ origin }, m_direction{ direction } {}

  const Vec3f& getOrigin() const { return m_origin; }
  const Vec3f& getDirection() const { return m_direction; }

  /// Ray-point intersection check.
  /// \param point Point to check if there is an intersection with.
  /// \return True if the ray intersects the point, false otherwise.
  bool intersects(const Vec3f& point) const;
  /// Ray-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the line, false otherwise.
  bool intersects(const Line& line, RayHit* hit = nullptr) const;
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
  /// \return True if the ray intersects the triangle, false otherwise.
  bool intersects(const Triangle& triangle) const;
  /// Ray-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \param hit Ray intersection's information to recover.
  /// \return True if the ray intersects the quad, false otherwise.
  bool intersects(const Quad& quad, RayHit* hit = nullptr) const;
  /// Ray-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if the ray intersects the AABB, false otherwise.
  bool intersects(const AABB& aabb) const;
  /// Computes the projection of a point (closest point) onto the ray.
  /// The projected point is necessarily located between the ray's origin and towards infinity in the ray's direction.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the ray.
  Vec3f computeProjection(const Vec3f& point) const;

private:
  Vec3f m_origin {};
  Vec3f m_direction {};
};

} // namespace Raz

#endif // RAZ_RAY_HPP
