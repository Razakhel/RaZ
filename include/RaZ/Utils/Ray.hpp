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

  bool intersects(const Vec3f& point) const;
  bool intersects(const Line& line) const;
  bool intersects(const Plane& plane) const;
  bool intersects(const Sphere& sphere) const;
  bool intersects(const Triangle& triangle) const;
  bool intersects(const Quad& quad) const;
  bool intersects(const AABB& aabb) const;
  Vec3f computeProjection(const Vec3f& point) const;

private:
  Vec3f m_origin {};
  Vec3f m_direction {};
};

} // namespace Raz

#endif // RAZ_RAY_HPP
