#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Ray.hpp"

namespace Raz {

enum class ShapeType {
  LINE,
  PLANE,
  SPHERE,
  TRIANGLE,
  QUAD,
  AABB,
  OBB
};

class Line;
class Plane;
class Sphere;
class Triangle;
class Quad;
class AABB;
class OBB;

class Shape {
public:
  Shape(const Shape&) = default;
  Shape(Shape&&) noexcept = default;

  /// Gets the type of the shape.
  /// \return Shape's type.
  virtual ShapeType getType() const noexcept = 0;

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
  /// Shape-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  virtual bool intersects(const OBB& obb) const = 0;
  /// Ray-shape intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the shape, false otherwise.
  virtual bool intersects(const Ray& ray, RayHit* hit) const = 0;
  /// Translates the shape by the given vector.
  /// \param displacement Displacement to be translated by.
  virtual void translate(const Vec3f& displacement) noexcept = 0;
  /// Computes the projection of a point (closest point) onto the shape.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the shape.
  virtual Vec3f computeProjection(const Vec3f& point) const = 0;
  /// Computes the shape's centroid.
  /// \return Computed centroid.
  virtual Vec3f computeCentroid() const = 0;

  Shape& operator=(const Shape&) = default;
  Shape& operator=(Shape&&) noexcept = default;

  virtual ~Shape() = default;

protected:
  Shape() = default;
};

/// Line segment defined by its two extremities' positions.
class Line final : public Shape {
public:
  Line(const Vec3f& beginPos, const Vec3f& endPos) : m_beginPos{ beginPos }, m_endPos{ endPos } {}

  ShapeType getType() const noexcept override { return ShapeType::LINE; }
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
  /// Line-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const OBB& obb) const override;
  /// Ray-line intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the line, false otherwise.
  bool intersects(const Ray&, RayHit*) const override { throw std::runtime_error("Error: Not implemented yet."); }
  /// Translates the line by the given vector.
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) noexcept override;
  /// Computes the projection of a point (closest point) onto the line.
  /// The projected point is necessarily located on the line.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the line.
  Vec3f computeProjection(const Vec3f& point) const override;
  /// Computes the line's centroid, which is the point lying directly between the two extremities.
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return (m_beginPos + m_endPos) * 0.5f; }
  /// Line length computation.
  /// To be used if the actual length is needed; otherwise, prefer computeSquaredLength().
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

/// Plane defined by a distance from [ 0; 0; 0 ] and a normal.
class Plane final : public Shape {
public:
  explicit Plane(float distance, const Vec3f& normal = Axis::Y) : m_distance{ distance }, m_normal{ normal } {}
  explicit Plane(const Vec3f& position, const Vec3f& normal = Axis::Y) : m_distance{ position.computeLength() }, m_normal{ normal } {}
  Plane(const Vec3f& firstPoint, const Vec3f& secondPoint, const Vec3f& thirdPoint)
    : m_distance{ ((firstPoint + secondPoint + thirdPoint) / 3.f).computeLength() },
      m_normal{ (secondPoint - firstPoint).cross(thirdPoint - firstPoint).normalize() } {}

  ShapeType getType() const noexcept override { return ShapeType::PLANE; }
  float getDistance() const { return m_distance; }
  const Vec3f& getNormal() const { return m_normal; }

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is located on the plane, false otherwise.
  bool contains(const Vec3f& point) const override { return FloatUtils::areNearlyEqual(m_normal.dot(point) - m_distance, 0.f); }
  /// Plane-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Line& line) const override { return line.intersects(*this); }
  /// Plane-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both planes intersect each other, false otherwise.
  bool intersects(const Plane& plane) const override;
  /// Plane-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Sphere& sphere) const override;
  /// Plane-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Triangle& triangle) const override;
  /// Plane-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Quad& quad) const override;
  /// Plane-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const AABB& aabb) const override;
  /// Plane-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const OBB& obb) const override;
  /// Ray-plane intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the plane, false otherwise.
  bool intersects(const Ray& ray, RayHit* hit) const override { return ray.intersects(*this, hit); }
  /// Translates the plane by the given vector.
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) noexcept override { m_distance = m_normal.dot(computeCentroid() + displacement); }
  /// Computes the projection of a point (closest point) onto the plane.
  /// The projected point is necessarily located on the plane.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the plane.
  Vec3f computeProjection(const Vec3f& point) const override { return point - m_normal * (m_normal.dot(point) - m_distance); }
  /// Computes the plane's centroid, which is the point lying onto the plane at its distance from the center in its normal direction.
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return m_normal * m_distance; }

private:
  float m_distance {};
  Vec3f m_normal {};
};

/// Sphere defined by its center position and a radius.
class Sphere final : public Shape {
public:
  Sphere(const Vec3f& centerPos, float radius) : m_centerPos{ centerPos }, m_radius{ radius } {}

  ShapeType getType() const noexcept override { return ShapeType::SPHERE; }
  const Vec3f& getCenter() const { return m_centerPos; }
  float getRadius() const { return m_radius; }

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is contained in the sphere, false otherwise.
  bool contains(const Vec3f& point) const override;
  /// Sphere-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Line& line) const override { return line.intersects(*this); }
  /// Sphere-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Plane& plane) const override { return plane.intersects(*this); }
  /// Sphere-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both spheres intersect each other, false otherwise.
  bool intersects(const Sphere& sphere) const override;
  /// Sphere-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Triangle& triangle) const override;
  /// Sphere-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Quad& quad) const override;
  /// Sphere-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const AABB& aabb) const override;
  /// Sphere-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const OBB& obb) const override;
  /// Ray-sphere intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the sphere, false otherwise.
  bool intersects(const Ray& ray, RayHit* hit) const override { return ray.intersects(*this, hit); }
  /// Translates the sphere by the given vector.
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) noexcept override { m_centerPos += displacement; }
  /// Computes the projection of a point (closest point) onto the sphere.
  /// The projected point may be inside the sphere itself or on its surface.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto/into the sphere.
  Vec3f computeProjection(const Vec3f& point) const override { return (point - m_centerPos).normalize() * m_radius + m_centerPos; }
  /// Computes the sphere's centroid, which is its center. Strictly equivalent to getCenterPos().
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return m_centerPos; }

private:
  Vec3f m_centerPos {};
  float m_radius {};
};

/// Triangle defined by its three vertices' positions, presumably in counter-clockwise order.
class Triangle final : public Shape {
public:
  Triangle(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos)
    : m_firstPos{ firstPos }, m_secondPos{ secondPos }, m_thirdPos{ thirdPos } {}

  ShapeType getType() const noexcept override { return ShapeType::TRIANGLE; }
  const Vec3f& getFirstPos() const { return m_firstPos; }
  const Vec3f& getSecondPos() const { return m_secondPos; }
  const Vec3f& getThirdPos() const { return m_thirdPos; }

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is located on the triangle, false otherwise.
  bool contains(const Vec3f& point) const override { return (computeProjection(point) == point); }
  /// Triangle-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Line& line) const override { return line.intersects(*this); }
  /// Triangle-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Plane& plane) const override { return plane.intersects(*this); }
  /// Triangle-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Sphere& sphere) const override { return sphere.intersects(*this); }
  /// Triangle-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both triangles intersect each other, false otherwise.
  bool intersects(const Triangle& triangle) const override;
  /// Triangle-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Quad& quad) const override;
  /// Triangle-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const AABB& aabb) const override;
  /// Triangle-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const OBB& obb) const override;
  /// Ray-triangle intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the triangle, false otherwise.
  bool intersects(const Ray& ray, RayHit* hit) const override { return ray.intersects(*this, hit); }
  /// Translates the triangle by the given vector.
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) noexcept override;
  /// Computes the projection of a point (closest point) onto the triangle.
  /// The projected point is necessarily located on the triangle's surface.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the triangle.
  Vec3f computeProjection(const Vec3f& point) const override;
  /// Computes the triangle's centroid, which is the point lying directly between its three points.
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return (m_firstPos + m_secondPos + m_thirdPos) / 3.f; }
  /// Computes the triangle's normal from its points.
  /// \return Computed normal.
  Vec3f computeNormal() const;
  /// Checks if the triangle's points are defined in a counter-clockwise manner around a normal.
  /// \param normal Normal from which to determinate the ordering.
  /// \return True if the triangle is counter-clockwise, false otherwise.
  bool isCounterClockwise(const Vec3f& normal) const { return (normal.dot(computeNormal()) > 0.f); }
  /// Makes the triangle counter-clockwise around a normal. Does nothing if it already is.
  /// \param normal Normal from which to determinate the ordering.
  void makeCounterClockwise(const Vec3f& normal);

private:
  Vec3f m_firstPos {};
  Vec3f m_secondPos {};
  Vec3f m_thirdPos {};
};

/// Quad defined by its four vertices' positions, presumably in counter-clockwise order.
class Quad final : public Shape {
public:
  Quad(const Vec3f& leftTopPos, const Vec3f& rightTopPos, const Vec3f& rightBottomPos, const Vec3f& leftBottomPos)
    : m_leftTopPos{ leftTopPos }, m_rightTopPos{ rightTopPos }, m_rightBottomPos{ rightBottomPos }, m_leftBottomPos{ leftBottomPos } {}

  ShapeType getType() const noexcept override { return ShapeType::QUAD; }
  const Vec3f& getLeftTopPos() const { return m_leftTopPos; }
  const Vec3f& getRightTopPos() const { return m_rightTopPos; }
  const Vec3f& getRightBottomPos() const { return m_rightBottomPos; }
  const Vec3f& getLeftBottomPos() const { return m_leftBottomPos; }

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is located on the quad, false otherwise.
  bool contains(const Vec3f&) const override { throw std::runtime_error("Error: Not implemented yet."); }
  /// Quad-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Line& line) const override { return line.intersects(*this); }
  /// Quad-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Plane& plane) const override { return plane.intersects(*this); }
  /// Quad-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Sphere& sphere) const override { return sphere.intersects(*this); }
  /// Quad-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Triangle& triangle) const override { return triangle.intersects(*this); }
  /// Quad-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both quads intersect each other, false otherwise.
  bool intersects(const Quad& quad) const override;
  /// Quad-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const AABB& aabb) const override;
  /// Quad-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const OBB& obb) const override;
  /// Ray-quad intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the quad, false otherwise.
  bool intersects(const Ray&, RayHit*) const override { throw std::runtime_error("Error: Not implemented yet."); }
  /// Translates the quad by the given vector.
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) noexcept override;
  /// Computes the projection of a point (closest point) onto the quad.
  /// The projected point is necessarily located on the quad's surface.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the quad.
  Vec3f computeProjection(const Vec3f& point) const override;
  /// Computes the quad's centroid, which is the point lying directly between its four points.
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return (m_leftTopPos + m_rightTopPos + m_rightBottomPos + m_leftBottomPos) * 0.25f; }

private:
  Vec3f m_leftTopPos {};
  Vec3f m_rightTopPos {};
  Vec3f m_rightBottomPos {};
  Vec3f m_leftBottomPos {};
};

/// Axis-aligned bounding box defined by its minimal and maximal vertices' positions.
///
///          _______________________
///         /|                    /|
///        / |                   / |
///       |---------------------| < maxPos
///       |  |                  |  |
///       |  |                  |  |
///       |  |                  |  |
///       |  |                  |  |
///       | /-------------------|-/
///       |/ ^ minPos           |/
///       -----------------------
///
/// The min position designates the point in [ -X; -Y; -Z ], and the max the point in [ +X; +Y; +Z ].
class AABB final : public Shape {
public:
  AABB(const Vec3f& minPos, const Vec3f& maxPos) noexcept : m_minPos{ minPos }, m_maxPos{ maxPos } {}

  ShapeType getType() const noexcept override { return ShapeType::AABB; }
  const Vec3f& getMinPosition() const { return m_minPos; }
  const Vec3f& getMaxPosition() const { return m_maxPos; }

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is contained in the AABB, false otherwise.
  bool contains(const Vec3f& point) const override;
  /// AABB-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Line& line) const override { return line.intersects(*this); }
  /// AABB-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Plane& plane) const override { return plane.intersects(*this); }
  /// AABB-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Sphere& sphere) const override { return sphere.intersects(*this); }
  /// AABB-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Triangle& triangle) const override { return triangle.intersects(*this); }
  /// AABB-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Quad& quad) const override { return quad.intersects(*this); }
  /// AABB-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both AABBs intersect each other, false otherwise.
  bool intersects(const AABB& aabb) const override;
  /// AABB-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const OBB& obb) const override;
  /// Ray-AABB intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the AABB, false otherwise.
  bool intersects(const Ray& ray, RayHit* hit) const override { return ray.intersects(*this, hit); }
  /// Translates the AABB by the given vector.
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) noexcept override;
  /// Computes the projection of a point (closest point) onto the AABB.
  /// The projected point may be inside the AABB itself or on its surface.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the shape.
  Vec3f computeProjection(const Vec3f& point) const override;
  /// Computes the AABB's centroid, which is the point lying directly between its two extremities.
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return (m_maxPos + m_minPos) * 0.5f; }
  /// Computes the half extents of the box, starting from its centroid.
  ///
  ///          _______________________
  ///         /|          ^         /|
  ///        / |          |        / |
  ///       |---------------------|  |
  ///       |  |          |       |  |
  ///       |  |          --------|->|
  ///       |  |         /        |  |
  ///       |  |        v         |  |
  ///       | /-------------------|-/
  ///       |/                    |/
  ///       -----------------------
  /// \return AABB's half extents.
  Vec3f computeHalfExtents() const { return (m_maxPos - m_minPos) * 0.5f; }

private:
  Vec3f m_minPos {};
  Vec3f m_maxPos {};
};

/// Oriented bounding box defined by its minimal and maximal vertices' positions, as well as a rotation.
///
///          _______________________
///         /|                    /|
///        / |                   / |
///       |---------------------| < maxPos
///       |  |                  |  |
///       |  |                  |  |
///       |  |                  |  |
///       |  |                  |  |
///       | /-------------------|-/
///       |/ ^ minPos           |/
///       -----------------------
///
/// The min position designates the point in [ -X; -Y; -Z ], and the max the point in [ +X; +Y; +Z ].
///
/// Beyond that, an OBB differs from an AABB in that it contains a rotation giving its orientation.
///
///                        /\-----------------|
///                      /    \               / |
///                    /        \           /     |
///                  /            \       /         |
///                /                \   /             |
///              /                    \                 |
///            /                     /  \-----------------|
///          /                     /    /                 /
///        /                     /    /                 /
///        \--------------------|   /                 /
///          \                   \/                 /
///            \                /  \              /
///              \            /      \          /
///                \        /          \      /
///                  \    /              \  /
///                    \/-----------------/
///
class OBB final : public Shape {
public:
  OBB(const Vec3f& minPos, const Vec3f& maxPos, const Mat3f& rotation = Mat3f::identity()) : m_aabb(minPos, maxPos), m_rotation{ rotation } {}
  explicit OBB(const AABB& aabb, const Mat3f& rotation = Mat3f::identity()) : m_aabb{ aabb }, m_rotation{ rotation } {}

  ShapeType getType() const noexcept override { return ShapeType::OBB; }
  const Vec3f& getMinPosition() const { return m_aabb.getMinPosition(); }
  const Vec3f& getMaxPosition() const { return m_aabb.getMaxPosition(); }
  const Mat3f& getRotation() const { return m_rotation; }

  void setRotation(const Mat3f& rotation);

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is contained in the OBB, false otherwise.
  bool contains(const Vec3f& point) const override;
  /// OBB-line intersection check.
  /// \param line Line to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Line& line) const override { return line.intersects(*this); }
  /// OBB-plane intersection check.
  /// \param plane Plane to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Plane& plane) const override { return plane.intersects(*this); }
  /// OBB-sphere intersection check.
  /// \param sphere Sphere to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Sphere& sphere) const override { return sphere.intersects(*this); }
  /// OBB-triangle intersection check.
  /// \param triangle Triangle to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Triangle& triangle) const override { return triangle.intersects(*this); }
  /// OBB-quad intersection check.
  /// \param quad Quad to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const Quad& quad) const override { return quad.intersects(*this); }
  /// OBB-AABB intersection check.
  /// \param aabb AABB to check if there is an intersection with.
  /// \return True if both shapes intersect each other, false otherwise.
  bool intersects(const AABB& aabb) const override { return aabb.intersects(*this); }
  /// OBB-OBB intersection check.
  /// \param obb OBB to check if there is an intersection with.
  /// \return True if both OBBs intersect each other, false otherwise.
  bool intersects(const OBB& obb) const override;
  /// Ray-OBB intersection check.
  /// \param ray Ray to check if there is an intersection with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return True if the ray intersects the OBB, false otherwise.
  bool intersects(const Ray&, RayHit*) const override { throw std::runtime_error("Error: Not implemented yet."); }
  /// Translates the OBB by the given vector.
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) noexcept override { m_aabb.translate(displacement); }
  /// Computes the projection of a point (closest point) onto the OBB.
  /// The projected point may be inside the AABB itself or on its surface.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the shape.
  Vec3f computeProjection(const Vec3f& point) const override;
  /// Computes the OBB's centroid, which is the point lying directly between its two extremities.
  /// \return Computed centroid.
  Vec3f computeCentroid() const override { return m_aabb.computeCentroid(); }
  /// Computes the half extents of the box, starting from its centroid.
  /// These half extents are oriented according to the box's rotation.
  ///
  ///          _______________________
  ///         /|          ^         /|
  ///        / |          |        / |
  ///       |---------------------|  |
  ///       |  |          |       |  |
  ///       |  |          --------|->|
  ///       |  |         /        |  |
  ///       |  |        v         |  |
  ///       | /-------------------|-/
  ///       |/                    |/
  ///       -----------------------
  /// \return OBB's half extents.
  Vec3f computeHalfExtents() const { return m_aabb.computeHalfExtents() * m_rotation; }

private:
  AABB m_aabb;
  Mat3f m_rotation {};
  Mat3f m_invRotation {};
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
