#pragma once

#ifndef RAZ_RAY_HPP
#define RAZ_RAY_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace Raz {

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
  bool intersects(const Line& line) const;
  bool intersects(const Plane& plane) const;
  /// Ray-sphere intersection check.
  /// The intersection is checked by calculating a quadratic equation to determine the hits distances.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if the ray intersects the sphere, false otherwise.
  bool intersects(const Sphere& sphere) const;
  bool intersects(const Triangle& triangle) const;
  bool intersects(const Quad& quad) const;
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
