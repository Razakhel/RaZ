#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Ray.hpp"

namespace Raz {

class Line;
class Plane;
class Sphere;
class Triangle;
class Quad;
class AABB;
class OBB;

struct BoxCorners {
  Vec3f minMinMin; ///< Point in [Min X; Min Y; Min Z].
  Vec3f minMinMax; ///< Point in [Min X; Min Y; Max Z].
  Vec3f minMaxMin; ///< Point in [Min X; Max Y; Min Z].
  Vec3f minMaxMax; ///< Point in [Min X; Max Y; Max Z].
  Vec3f maxMinMin; ///< Point in [Max X; Min Y; Min Z].
  Vec3f maxMinMax; ///< Point in [Max X; Min Y; Max Z].
  Vec3f maxMaxMin; ///< Point in [Max X; Max Y; Min Z].
  Vec3f maxMaxMax; ///< Point in [Max X; Max Y; Max Z].
};

enum class ShapeType {
  LINE,
  PLANE,
  SPHERE,
  TRIANGLE,
  QUAD,
  AABB,
  OBB
};

class Shape {
public:
  constexpr Shape(const Shape&) = default;
  constexpr Shape(Shape&&) noexcept = default;

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
  /// Computes the shape's bounding box.
  /// \return Computed bounding box.
  virtual AABB computeBoundingBox() const = 0;

  constexpr Shape& operator=(const Shape&) = default;
  constexpr Shape& operator=(Shape&&) noexcept = default;

  constexpr virtual ~Shape() = default;

protected:
  constexpr Shape() = default;
};

/// Line segment defined by its two extremities' positions.
class Line final : public Shape {
public:
  constexpr Line(const Vec3f& beginPos, const Vec3f& endPos) noexcept : m_beginPos{ beginPos }, m_endPos{ endPos } {}

  constexpr ShapeType getType() const noexcept override { return ShapeType::LINE; }
  constexpr const Vec3f& getBeginPos() const { return m_beginPos; }
  constexpr const Vec3f& getEndPos() const { return m_endPos; }

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
  constexpr Vec3f computeCentroid() const override { return (m_beginPos + m_endPos) * 0.5f; }
  AABB computeBoundingBox() const override;
  /// Line squared length computation.
  /// To be preferred over computeLength() for faster operations.
  /// \return Line's squared length.
  constexpr float computeSquaredLength() const { return (m_endPos - m_beginPos).computeSquaredLength(); }
  /// Line length computation.
  /// To be used if the actual length is needed; otherwise, prefer computeSquaredLength().
  /// \return Line's length.
  constexpr float computeLength() const { return (m_endPos - m_beginPos).computeLength(); }

  /// Checks if the current line is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param line Line to be compared with.
  /// \return True if the lines are nearly equal to each other, false otherwise.
  constexpr bool operator==(const Line& line) const noexcept { return (m_beginPos == line.m_beginPos && m_endPos == line.m_endPos); }
  /// Checks if the current line is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param line Line to be compared with.
  /// \return True if the lines are different from each other, false otherwise.
  constexpr bool operator!=(const Line& line) const noexcept { return !(*this == line); }

private:
  Vec3f m_beginPos;
  Vec3f m_endPos;
};

/// Plane defined by a distance from [ 0; 0; 0 ] and a normal.
class Plane final : public Shape {
public:
  constexpr explicit Plane(float distance, const Vec3f& normal = Axis::Up) noexcept : m_distance{ distance }, m_normal{ normal } {}
  constexpr explicit Plane(const Vec3f& position, const Vec3f& normal = Axis::Up) noexcept : m_distance{ position.computeLength() }, m_normal{ normal } {}
  constexpr Plane(const Vec3f& firstPoint, const Vec3f& secondPoint, const Vec3f& thirdPoint) noexcept
    : m_distance{ ((firstPoint + secondPoint + thirdPoint) / 3.f).computeLength() },
      m_normal{ (secondPoint - firstPoint).cross(thirdPoint - firstPoint).normalize() } {}

  constexpr ShapeType getType() const noexcept override { return ShapeType::PLANE; }
  constexpr float getDistance() const { return m_distance; }
  constexpr const Vec3f& getNormal() const { return m_normal; }

  /// Point containment check.
  /// \param point Point to be checked.
  /// \return True if the point is located on the plane, false otherwise.
  constexpr bool contains(const Vec3f& point) const override { return FloatUtils::areNearlyEqual(m_normal.dot(point) - m_distance, 0.f); }
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
  constexpr void translate(const Vec3f& displacement) noexcept override { m_distance = m_normal.dot(computeCentroid() + displacement); }
  /// Computes the projection of a point (closest point) onto the plane.
  /// The projected point is necessarily located on the plane.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto the plane.
  constexpr Vec3f computeProjection(const Vec3f& point) const override { return point - m_normal * (m_normal.dot(point) - m_distance); }
  /// Computes the plane's centroid, which is the point lying onto the plane at its distance from the center in its normal direction.
  /// \return Computed centroid.
  constexpr Vec3f computeCentroid() const override { return m_normal * m_distance; }
  AABB computeBoundingBox() const override;

  /// Checks if the current plane is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param plane Plane to be compared with.
  /// \return True if the planes are nearly equal to each other, false otherwise.
  constexpr bool operator==(const Plane& plane) const noexcept { return (FloatUtils::areNearlyEqual(m_distance, plane.m_distance)
                                                                      && m_normal == plane.m_normal); }
  /// Checks if the current plane is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param plane Plane to be compared with.
  /// \return True if the planes are different from each other, false otherwise.
  constexpr bool operator!=(const Plane& plane) const noexcept { return !(*this == plane); }

private:
  float m_distance {};
  Vec3f m_normal;
};

/// Sphere defined by its center position and a radius.
class Sphere final : public Shape {
public:
  constexpr Sphere(const Vec3f& centerPos, float radius) noexcept : m_centerPos{ centerPos }, m_radius{ radius } {}

  constexpr ShapeType getType() const noexcept override { return ShapeType::SPHERE; }
  constexpr const Vec3f& getCenter() const { return m_centerPos; }
  constexpr float getRadius() const { return m_radius; }

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
  constexpr void translate(const Vec3f& displacement) noexcept override { m_centerPos += displacement; }
  /// Computes the projection of a point (closest point) onto the sphere.
  /// The projected point may be inside the sphere itself or on its surface.
  /// \param point Point to compute the projection from.
  /// \return Point projected onto/into the sphere.
  constexpr Vec3f computeProjection(const Vec3f& point) const override { return (point - m_centerPos).normalize() * m_radius + m_centerPos; }
  /// Computes the sphere's centroid, which is its center. Strictly equivalent to getCenterPos().
  /// \return Computed centroid.
  constexpr Vec3f computeCentroid() const override { return m_centerPos; }
  AABB computeBoundingBox() const override;

  /// Checks if the current sphere is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param sphere Sphere to be compared with.
  /// \return True if the spheres are nearly equal to each other, false otherwise.
  constexpr bool operator==(const Sphere& sphere) const noexcept { return (m_centerPos == sphere.m_centerPos
                                                                        && FloatUtils::areNearlyEqual(m_radius, sphere.m_radius)); }
  /// Checks if the current sphere is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param sphere Sphere to be compared with.
  /// \return True if the spheres are different from each other, false otherwise.
  constexpr bool operator!=(const Sphere& sphere) const noexcept { return !(*this == sphere); }

private:
  Vec3f m_centerPos;
  float m_radius {};
};

/// Triangle defined by its three vertices' positions, presumably in counter-clockwise order.
class Triangle final : public Shape {
public:
  constexpr Triangle(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos) noexcept
    : m_firstPos{ firstPos }, m_secondPos{ secondPos }, m_thirdPos{ thirdPos } {}

  constexpr ShapeType getType() const noexcept override { return ShapeType::TRIANGLE; }
  constexpr const Vec3f& getFirstPos() const { return m_firstPos; }
  constexpr const Vec3f& getSecondPos() const { return m_secondPos; }
  constexpr const Vec3f& getThirdPos() const { return m_thirdPos; }

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
  constexpr Vec3f computeCentroid() const override { return (m_firstPos + m_secondPos + m_thirdPos) / 3.f; }
  AABB computeBoundingBox() const override;
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

  /// Checks if the current triangle is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param triangle Triangle to be compared with.
  /// \return True if the triangles are nearly equal to each other, false otherwise.
  constexpr bool operator==(const Triangle& triangle) const noexcept { return (m_firstPos == triangle.m_firstPos
                                                                            && m_secondPos == triangle.m_secondPos
                                                                            && m_thirdPos == triangle.m_thirdPos); }
  /// Checks if the current triangle is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param triangle Triangle to be compared with.
  /// \return True if the triangles are different from each other, false otherwise.
  constexpr bool operator!=(const Triangle& triangle) const noexcept { return !(*this == triangle); }

private:
  Vec3f m_firstPos;
  Vec3f m_secondPos;
  Vec3f m_thirdPos;
};

/// Quad defined by its four vertices' positions, presumably in counter-clockwise order.
class Quad final : public Shape {
public:
  constexpr Quad(const Vec3f& leftTopPos, const Vec3f& rightTopPos, const Vec3f& rightBottomPos, const Vec3f& leftBottomPos) noexcept
    : m_leftTopPos{ leftTopPos }, m_rightTopPos{ rightTopPos }, m_rightBottomPos{ rightBottomPos }, m_leftBottomPos{ leftBottomPos } {}

  constexpr ShapeType getType() const noexcept override { return ShapeType::QUAD; }
  constexpr const Vec3f& getLeftTopPos() const { return m_leftTopPos; }
  constexpr const Vec3f& getRightTopPos() const { return m_rightTopPos; }
  constexpr const Vec3f& getRightBottomPos() const { return m_rightBottomPos; }
  constexpr const Vec3f& getLeftBottomPos() const { return m_leftBottomPos; }

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
  constexpr Vec3f computeCentroid() const override { return (m_leftTopPos + m_rightTopPos + m_rightBottomPos + m_leftBottomPos) * 0.25f; }
  AABB computeBoundingBox() const override;

  /// Checks if the current quad is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param quad Quad to be compared with.
  /// \return True if the quads are nearly equal to each other, false otherwise.
  constexpr bool operator==(const Quad& quad) const noexcept { return (m_leftTopPos == quad.m_leftTopPos
                                                                    && m_rightTopPos == quad.m_rightTopPos
                                                                    && m_rightBottomPos == quad.m_rightBottomPos
                                                                    && m_leftBottomPos == quad.m_leftBottomPos); }
  /// Checks if the current quad is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param quad Quad to be compared with.
  /// \return True if the quads are different from each other, false otherwise.
  constexpr bool operator!=(const Quad& quad) const noexcept { return !(*this == quad); }

private:
  Vec3f m_leftTopPos;
  Vec3f m_rightTopPos;
  Vec3f m_rightBottomPos;
  Vec3f m_leftBottomPos;
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
  constexpr AABB(const Vec3f& minPos, const Vec3f& maxPos) noexcept : m_minPos{ minPos }, m_maxPos{ maxPos } {}

  constexpr ShapeType getType() const noexcept override { return ShapeType::AABB; }
  constexpr const Vec3f& getMinPosition() const { return m_minPos; }
  constexpr const Vec3f& getMaxPosition() const { return m_maxPos; }

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
  constexpr Vec3f computeCentroid() const override { return (m_maxPos + m_minPos) * 0.5f; }
  constexpr AABB computeBoundingBox() const override { return *this; }
  /// Computes the half-extents of the box, starting from its centroid.
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
  /// \return AABB's half-extents.
  constexpr Vec3f computeHalfExtents() const noexcept { return (m_maxPos - m_minPos) * 0.5f; }
  /// Computes the coordinates of the box's 8 corners.
  /// \return Corners of the box.
  BoxCorners computeCorners() const;

  /// Checks if the current AABB is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param aabb AABB to be compared with.
  /// \return True if the AABBs are nearly equal to each other, false otherwise.
  constexpr bool operator==(const AABB& aabb) const noexcept { return (m_minPos == aabb.m_minPos && m_maxPos == aabb.m_maxPos); }
  /// Checks if the current AABB is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param aabb AABB to be compared with.
  /// \return True if the AABBs are different from each other, false otherwise.
  constexpr bool operator!=(const AABB& aabb) const noexcept { return !(*this == aabb); }

private:
  Vec3f m_minPos;
  Vec3f m_maxPos;
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
  constexpr OBB(const Vec3f& minPos, const Vec3f& maxPos) noexcept : m_aabb(minPos, maxPos) {}
  constexpr OBB(const Vec3f& minPos, const Vec3f& maxPos, const Quaternionf& rotation) noexcept : m_aabb(minPos, maxPos) { setRotation(rotation); }
  constexpr explicit OBB(const AABB& aabb) noexcept : m_aabb{ aabb } {}
  constexpr OBB(const AABB& aabb, const Quaternionf& rotation) noexcept : m_aabb{ aabb } { setRotation(rotation); }

  constexpr ShapeType getType() const noexcept override { return ShapeType::OBB; }
  constexpr const AABB& getOriginalBox() const { return m_aabb; }
  constexpr const Quaternionf& getRotation() const { return m_rotation; }
  constexpr const Quaternionf& getInverseRotation() const { return m_invRotation; }

  constexpr void setRotation(const Quaternionf& rotation) {
    m_rotation    = rotation;
    m_invRotation = m_rotation.inverse();
  }

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
  constexpr Vec3f computeCentroid() const override { return m_aabb.computeCentroid(); }
  AABB computeBoundingBox() const override;
  /// Computes the rotated minimum position of the box.
  /// \return Rotated minimum position.
  constexpr Vec3f computeRotatedMinPosition() const {
    const Vec3f centroid = computeCentroid();
    return (centroid + m_rotation * (m_aabb.getMinPosition() - centroid));
  }
  /// Computes the rotated maximum position of the box.
  /// \return Rotated maximum position.
  constexpr Vec3f computeRotatedMaxPosition() const {
    const Vec3f centroid = computeCentroid();
    return (centroid + m_rotation * (m_aabb.getMaxPosition() - centroid));
  }
  /// Computes the rotated half-extents of the box, starting from its centroid.
  /// \return Rotated half-extents.
  constexpr Vec3f computeRotatedHalfExtents() const noexcept { return m_rotation * m_aabb.computeHalfExtents(); }

  /// Checks if the current OBB is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param obb OBB to be compared with.
  /// \return True if the OBBs are nearly equal to each other, false otherwise.
  constexpr bool operator==(const OBB& obb) const noexcept { return (m_aabb == obb.m_aabb && m_rotation == obb.m_rotation); }
  /// Checks if the current OBB is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param obb OBB to be compared with.
  /// \return True if the OBBs are different from each other, false otherwise.
  constexpr bool operator!=(const OBB& obb) const noexcept { return !(*this == obb); }

private:
  AABB m_aabb;
  Quaternionf m_rotation    = Quaternionf::identity();
  Quaternionf m_invRotation = Quaternionf::identity();
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
