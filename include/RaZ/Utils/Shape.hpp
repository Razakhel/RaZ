#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Shape : public Component {
public:
  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is contained by the shape, false otherwise.
  virtual bool contains(const Vec3f& point) const = 0;
  /// Shapes intersection check.
  /// \param shape Shape to be checked.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const Shape& shape) const { return intersects(*this, shape); }
  /// Point projection onto the shape.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the shape.
  virtual Vec3f computeProjection(const Vec3f& point) const = 0;
  /// Shape centroid computation.
  /// \return Computed centroid.
  virtual Vec3f computeCentroid() const = 0;

private:
  /// Generic shape intersection check.
  /// \tparam S1 First shape's type.
  /// \tparam S2 Second shape's type.
  /// \return True if both shapes intersect each other, false otherwise.
  template <typename S1, typename S2> bool intersects(const S1&, const S2&) const {
    throw std::invalid_argument("Error: Shapes intersection is not implemented for those shapes types.");
  }
};

/// Line segment defined by its two extremities' positions.
class Line {
public:
  Line() = default;
  Line(const Vec3f& beginPos, const Vec3f& endPos) : m_beginPos{ beginPos }, m_endPos{ endPos } {}

  const Vec3f& getBeginPos() const { return m_beginPos; }
  const Vec3f& getEndPos() const { return m_endPos; }

  /// Line length computation. Use this if you really need the actual length; otherwise, prefer computeSquaredLength().
  /// \return Line's length.
  float computeLength() const { return (m_endPos - m_beginPos).computeLength(); }
  /// Line squared length computation; to be preferred over computeLength() for faster operations.
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
