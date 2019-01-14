#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Line;
class Plane;
class Sphere;
class Triangle;
class Quad;
class AABB;

class Shape : public Component {
public:
  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is contained by the shape, false otherwise.
  virtual bool contains(const Vec3f& point) const = 0;
  /// Shape-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const Line& line) const = 0;
  /// Shape-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const Plane& plane) const = 0;
  /// Shape-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const Sphere& sphere) const = 0;
  /// Shape-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const Triangle& triangle) const = 0;
  /// Shape-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const Quad& quad) const = 0;
  /// Shape-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const AABB& aabb) const = 0;
  /// Computes the projection of a point (closest point) onto the shape.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the shape.
  virtual Vec3f computeProjection(const Vec3f& point) const = 0;
  /// Computes the shape's centroid.
  /// \return Computed centroid.
  virtual Vec3f computeCentroid() const = 0;
};

/// Line segment defined by its two extremities' positions.
class Line : public Shape {
public:
  Line() = default;
  Line(const Vec3f& beginPos, const Vec3f& endPos) : m_beginPos{ beginPos }, m_endPos{ endPos } {}

  const Vec3f& getBeginPos() const { return m_beginPos; }
  const Vec3f& getEndPos() const { return m_endPos; }

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is located on the line, false otherwise.
  bool contains(const Vec3f& point) const override { return computeProjection(point) == point; }
  /// Line-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both lines intersect each other, false otherwise.
  bool intersects(const Line& line) const override;
  /// Line-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Plane& plane) const override;
  /// Line-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Sphere& sphere) const override;
  /// Line-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Triangle& triangle) const override;
  /// Line-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Quad& quad) const override;
  /// Line-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const AABB& aabb) const override;
  /// Computes the projection of a point (closest point) onto the line.
  /// The projected point is necessarily located on the line.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the line.
  Vec3f computeProjection(const Vec3f& point) const override;
  /// Computes the line's centroid, which is the point lying directly between the two extremities.
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return (m_beginPos + m_endPos) / 2.f; }
  /// Line length computation.
  /// To be used if actual length is needed; otherwise, prefer computeSquaredLength().
  /// \return Line's length.
  float computeLength() const { return (m_endPos - m_beginPos).computeLength(); }
  /// Line squared length computation.
  /// To be preferred over computeLength() for faster operations.
  /// \return Line's squared length.
  float computeSquaredLength() const { return (m_endPos - m_beginPos).computeSquaredLength(); }

private:
  Vec3f m_beginPos {};
  Vec3f m_endPos {};
};

struct Triangle {
  Triangle() = default;
  Triangle(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos)
    : firstPos{ firstPos }, secondPos{ secondPos }, thirdPos{ thirdPos } {}

  Vec3f firstPos {};
  Vec3f secondPos {};
  Vec3f thirdPos {};
};

struct Quad {
  Quad() = default;
  Quad(const Vec3f& leftTopPos, const Vec3f& rightTopPos, const Vec3f& rightBottomPos, const Vec3f& leftBottomPos)
    : leftTopPos{ leftTopPos }, rightTopPos{ rightTopPos }, rightBottomPos{ rightBottomPos }, leftBottomPos{ leftBottomPos } {}

  Vec3f leftTopPos {};
  Vec3f rightTopPos {};
  Vec3f rightBottomPos {};
  Vec3f leftBottomPos {};
};

struct AABB {
  AABB() = default;
  AABB(const Vec3f& rightTopFrontPos, const Vec3f& leftBottomBackPos)
    : rightTopFrontPos{ rightTopFrontPos }, leftBottomBackPos{ leftBottomBackPos } {}

  Vec3f rightTopFrontPos {};
  Vec3f leftBottomBackPos {};
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
