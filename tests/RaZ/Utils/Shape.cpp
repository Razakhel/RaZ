#include "catch/catch.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace {

//       Line 1         |      Line 2       |       Line 3
//                      |                   |
//                      |     [ 0; 1 ]      |              [ 1; 1 ]
//                      |         |         |                /
//   ----------------   |         |         |              /
//   ^              ^   |         |         |            /
//  [ 0; 0 ]  [ 1; 0 ]  |         |         |          /
//                      |         |         |        /
//                      |     [ 0; 0 ]      |   [ -1; -1 ]

const Raz::Line line1(Raz::Vec3f({ 0.f, 0.f, 0.f }), Raz::Vec3f({ 1.f, 0.f, 0.f }));
const Raz::Line line2(Raz::Vec3f({ 0.f, 0.f, 0.f }), Raz::Vec3f({ 0.f, 1.f, 0.f }));
const Raz::Line line3(Raz::Vec3f(-1.f), Raz::Vec3f(1.f));

//      Plane 1      |       Plane 2      |      Plane 3
//                   |                    |
//       normal      |   \      normal    |    normal      /
//         ^         |     \      ^       |       ^      /
//         |         |       \   /        |        \   /
//   ______|______   |         \/         |         \/
//                   |           \        |        /
//                   |             \      |      /
//     [ 0; 0 ]      |    [ 0; 0 ]   \    |    /   [ 0; 0 ]

const Raz::Plane plane1(1.f, Raz::Axis::Y);
const Raz::Plane plane2(0.5f, Raz::Vec3f({ 1.f, 1.f, 0.f }).normalize());
const Raz::Plane plane3(0.5f, Raz::Vec3f({ -1.f, 1.f, 0.f }).normalize());

// These triangles are defined so that:
//  - triangle1 is laying flat slightly above 0
//  - triangle2 is standing, parallel to the Y/Z plane (facing the X direction)
//  - triangle3 is crooked, its head pointing to [ -X; +Y ], slightly below 0

const Raz::Triangle triangle1(Raz::Vec3f({ -3.f, 0.5f, 3.f }), Raz::Vec3f({ 0.f, 0.5f, -6.f }), Raz::Vec3f({ 3.f, 0.5f, 3.f }));
const Raz::Triangle triangle2(Raz::Vec3f({ 0.5f, -0.5f, 3.f }), Raz::Vec3f({ 0.5f, -0.5f, -3.f }), Raz::Vec3f({ 0.5f, 3.f, 0.f }));
const Raz::Triangle triangle3(Raz::Vec3f({ 0.f, -1.f, 1.f }), Raz::Vec3f({ -1.5f, -1.5f, 0.f }), Raz::Vec3f({ 0.f, -1.75f, -1.f }));

//         _______________________
//        /|                    /|
//       / |                   / | / 1 -> [  1;  1; 1 ]
//      |---------------------| < {  2 -> [  5;  5; 5 ]
//      |  |                  |  | \ 3 -> [ -5; -5; 5 ]
//      |  |                  |  |
//      |  |                  |  |
//      |  |                  |  |
//      | /-------------------|-/
//      |/ ^                  |/
//      ---|-------------------
//         |
//  1 -> [  -1;  -1; -1 ]
//  2 -> [   3;   3; -5 ]
//  3 -> [ -10; -10; -5 ]

const Raz::AABB aabb1(Raz::Vec3f(1.f), Raz::Vec3f(-1.f));
const Raz::AABB aabb2(Raz::Vec3f(5.f), Raz::Vec3f({ 3.f, 3.f, -5.f }));
const Raz::AABB aabb3(Raz::Vec3f({ -5.f, -5.f, 5.f }), Raz::Vec3f({ -10.f, -10.f, -5.f }));

} // namespace

TEST_CASE("Line basic") {
  REQUIRE(line1.computeCentroid() == Raz::Vec3f({ 0.5f, 0.f, 0.f }));
  REQUIRE(line2.computeCentroid() == Raz::Vec3f({ 0.f, 0.5f, 0.f }));
  REQUIRE(line3.computeCentroid() == Raz::Vec3f(0.f));

  REQUIRE(Raz::FloatUtils::checkNearEquality(line1.computeLength(), 1.f));
  REQUIRE(Raz::FloatUtils::checkNearEquality(line1.computeSquaredLength(), 1.f));

  REQUIRE(Raz::FloatUtils::checkNearEquality(line2.computeLength(), 1.f));
  REQUIRE(Raz::FloatUtils::checkNearEquality(line2.computeSquaredLength(), 1.f));

  REQUIRE(Raz::FloatUtils::checkNearEquality(line3.computeLength(), 3.464101615f));
  REQUIRE(Raz::FloatUtils::checkNearEquality(line3.computeSquaredLength(), 12.f));
}

TEST_CASE("Line-plane intersection") {
  REQUIRE_FALSE(line1.intersects(plane1));
  REQUIRE(line1.intersects(plane2));
  REQUIRE_FALSE(line1.intersects(plane3));

  REQUIRE(line2.intersects(plane1));
  REQUIRE(line2.intersects(plane2));
  REQUIRE(line2.intersects(plane3));

  REQUIRE(line3.intersects(plane1));
  REQUIRE(line3.intersects(plane2));
  REQUIRE_FALSE(line3.intersects(plane3));
}

TEST_CASE("Plane basic") {
  const Raz::Plane testPlane1(Raz::Vec3f({ 0.f, 1.f, 0.f }), Raz::Axis::Y);
  const Raz::Plane testPlane2(Raz::Vec3f({ 1.f, 1.f, 0.f }), Raz::Vec3f({ -1.f, 1.f, -1.f }), Raz::Vec3f({ 0.f, 1.f, 1.f }));

  // Checking that the 3 planes are strictly equal to each other
  REQUIRE(Raz::FloatUtils::checkNearEquality(testPlane1.getDistance(), testPlane2.getDistance()));
  REQUIRE(Raz::FloatUtils::checkNearEquality(testPlane1.getDistance(), testPlane2.getDistance()));

  REQUIRE(testPlane1.getNormal() == testPlane2.getNormal());
  REQUIRE(testPlane1.getNormal() == testPlane2.getNormal());
}

TEST_CASE("Plane-plane intersection") {
  const Raz::Plane testPlane(2.f, -Raz::Axis::Y);

  REQUIRE_FALSE(plane1.intersects(testPlane));
  REQUIRE(plane2.intersects(testPlane));
  REQUIRE(plane3.intersects(testPlane));

  REQUIRE(plane1.intersects(plane2));
  REQUIRE(plane1.intersects(plane3));
  REQUIRE(plane2.intersects(plane3));

  // A plane should not intersect itself
  REQUIRE_FALSE(plane1.intersects(plane1));
  REQUIRE_FALSE(plane2.intersects(plane2));
  REQUIRE_FALSE(plane3.intersects(plane3));
}

TEST_CASE("Triangle basic") {
  REQUIRE(triangle1.computeCentroid() == Raz::Vec3f({ 0.f, 0.5f, 0.f }));
  REQUIRE(triangle2.computeCentroid() == Raz::Vec3f({ 0.5f, 0.666666666f, 0.f }));
  REQUIRE(triangle3.computeCentroid() == Raz::Vec3f({ -0.5f, -1.416666666f, 0.f }));
}

TEST_CASE("Triangle clockwiseness") {
  REQUIRE(triangle1.isCounterClockwise(-Raz::Axis::Y));
  REQUIRE(triangle2.isCounterClockwise(Raz::Axis::X));
  REQUIRE(triangle3.isCounterClockwise((-Raz::Axis::X - Raz::Axis::Y).normalize())); // Pointing roughly towards [ -X; -Y ]

  // Creating two triangles with the same points but in a different ordering
  Raz::Triangle testTriangle1(Raz::Vec3f({ -1.f, 0.f, 0.f }), Raz::Vec3f({ 0.f, 1.f, 0.f }), Raz::Vec3f({ 1.f, 0.f, 0.f }));
  Raz::Triangle testTriangle2(Raz::Vec3f({ 1.f, 0.f, 0.f }), Raz::Vec3f({ 0.f, 1.f, 0.f }), Raz::Vec3f({ -1.f, 0.f, 0.f }));

  REQUIRE_FALSE(testTriangle1.isCounterClockwise(Raz::Axis::Z));
  REQUIRE(testTriangle2.isCounterClockwise(Raz::Axis::Z));

  testTriangle1.makeCounterClockwise(Raz::Axis::Z);
  REQUIRE(testTriangle1.isCounterClockwise(Raz::Axis::Z));
}

TEST_CASE("AABB basic") {
  REQUIRE(aabb1.computeCentroid() == Raz::Vec3f(0.f));
  REQUIRE(aabb2.computeCentroid() == Raz::Vec3f({ 4.f, 4.f, 0.f }));
  REQUIRE(aabb3.computeCentroid() == Raz::Vec3f({ -7.5f, -7.5f, 0.f }));

  REQUIRE(aabb1.computeHalfExtents() == Raz::Vec3f(1.f));
  REQUIRE(aabb2.computeHalfExtents() == Raz::Vec3f({ 1.f, 1.f, 5.f }));
  REQUIRE(aabb3.computeHalfExtents() == Raz::Vec3f({ 2.5f, 2.5f, 5.f }));
}
