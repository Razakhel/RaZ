#include "RaZ/Utils/Ray.hpp"
#include "RaZ/Utils/Shape.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

// Declaring rays to be tested
//
//        Ray 1      |       Ray 2       |      Ray 3
//                   |                   |
//          ^        |           ^       |      [ 1; 1 ]
//          |        |          /        |         /
//          |        |         /         |        /
//      [ 0; 0 ]     |    [ -1; -1 ]     |       v

constexpr Raz::Ray ray1(Raz::Vec3f(0.f, 0.f, 0.f), Raz::Axis::Y);
constexpr Raz::Ray ray2(Raz::Vec3f(-1.f, -1.f, 0.f), Raz::Vec3f(0.707106769f, 0.707106769f, 0.f));
constexpr Raz::Ray ray3(Raz::Vec3f(1.f, 1.f, 0.f), Raz::Vec3f(-0.707106769f, -0.707106769f, 0.f));

} // namespace

TEST_CASE("Ray-point intersection", "[utils]") {
  Raz::RayHit hit;

  CHECK(ray1.intersects(ray1.getOrigin(), &hit));

  CHECK(hit.position == ray1.getOrigin());
  CHECK(hit.normal   == Raz::Vec3f(0.f));
  CHECK(hit.distance == 0.f);

  const Raz::Vec3f topPoint(0.f, 2.f, 0.f);
  const Raz::Vec3f topRightPoint(2.f, 2.f, 0.f);

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //        ^
  //        |
  //        x < [ 0; 0 ]

  CHECK(ray1.intersects(topPoint, &hit));

  CHECK(hit.position == topPoint);
  CHECK(hit.normal   == -ray1.getDirection());
  CHECK(hit.distance == 2.f);

  CHECK_FALSE(ray1.intersects(topRightPoint));

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //          ^
  //         /
  //        x < [ -1; -1 ]

  CHECK_FALSE(ray2.intersects(topPoint));

  CHECK(ray2.intersects(topRightPoint, &hit));

  CHECK(hit.position == topRightPoint);
  CHECK(hit.normal   == -ray2.getDirection());
  CHECK(hit.distance == Raz::Vec3f(3.f, 3.f, 0.f).computeLength()); // 4.2426405f

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //             x < [ 1; 1 ]
  //            /
  //           v

  CHECK_FALSE(ray3.intersects(topPoint));
  CHECK_FALSE(ray3.intersects(topRightPoint));
}

TEST_CASE("Ray-plane intersection", "[utils]") {
  //       Plane 1      |      Plane 2      |      Plane 3      |      Plane 4
  //                    |                   |                   |
  //   \      normal    |    \              |              /    |
  //     \      ^       |      \            |            /      |        normal
  //       \   /        |        \          |          /        |          ^
  //         \/         |        / \        |        /  \       |          |
  //           \        |       /    \      |      /     \      |    ______|______
  //             \      |      v       \    |    /        v     |
  //               \    |   normal          |          normal   |     [ 0; 0.5 ]

  const Raz::Vec3f initPos(0.f, 0.5f, 0.f);
  const Raz::Plane plane1(initPos, Raz::Vec3f(1.f, 1.f, 0.f).normalize());
  const Raz::Plane plane2(initPos, Raz::Vec3f(-1.f, -1.f, 0.f).normalize());
  const Raz::Plane plane3(initPos, Raz::Vec3f(1.f, -1.f, 0.f).normalize());
  const Raz::Plane plane4(initPos, Raz::Axis::Y);

  CHECK_FALSE(ray1.intersects(plane1));
  CHECK_FALSE(ray2.intersects(plane1));
  CHECK(ray3.intersects(plane1));

  CHECK_FALSE(ray1.intersects(plane2));
  CHECK(ray2.intersects(plane2));
  CHECK_FALSE(ray3.intersects(plane2));

  CHECK_FALSE(ray1.intersects(plane3));
  CHECK_FALSE(ray2.intersects(plane3));
  CHECK_FALSE(ray3.intersects(plane3));

  CHECK_FALSE(ray1.intersects(plane4));
  CHECK_FALSE(ray2.intersects(plane4));
  CHECK(ray3.intersects(plane4));
}

TEST_CASE("Ray-sphere intersection", "[utils]") {
  // See: https://www.geogebra.org/m/sn7ajcj2

  const Raz::Sphere sphere1(Raz::Vec3f(0.f), 1.f);
  const Raz::Sphere sphere2(Raz::Vec3f(5.f, 10.f, 0.f), 5.f);
  const Raz::Sphere sphere3(Raz::Vec3f(-10.f, -10.f, 0.f), 1.f);

  Raz::RayHit hit;

  CHECK(ray1.intersects(sphere1, &hit));
  // TODO: this result is inconsistent with the ray-AABB intersection, which returns the one "behind" the ray's origin;
  //  one of the two should probably be fixed for consistency
  CHECK_THAT(hit.position, IsNearlyEqualToVector(Raz::Vec3f(0.f, 1.f, 0.f)));
  CHECK_THAT(hit.normal, IsNearlyEqualToVector(Raz::Axis::Y));
  CHECK_THAT(hit.distance, IsNearlyEqualTo(1.f));

  CHECK(ray2.intersects(sphere1, &hit));
  CHECK_THAT(hit.position, IsNearlyEqualToVector(Raz::Vec3f(-0.70710677f, -0.70710677f, 0.f)));
  CHECK_THAT(hit.normal, IsNearlyEqualToVector(Raz::Vec3f(-0.70710683f, -0.70710683f, 0.f)));
  CHECK_THAT(hit.distance, IsNearlyEqualTo(0.4142136f));

  CHECK(ray3.intersects(sphere1, &hit));
  CHECK_THAT(hit.position, IsNearlyEqualToVector(Raz::Vec3f(0.70710677f, 0.70710677f, 0.f)));
  CHECK_THAT(hit.normal, IsNearlyEqualToVector(Raz::Vec3f(0.70710683f, 0.70710683f, 0.f)));
  CHECK_THAT(hit.distance, IsNearlyEqualTo(0.4142136f));

  CHECK(ray1.intersects(sphere2, &hit));
  CHECK_THAT(hit.position, IsNearlyEqualToVector(Raz::Vec3f(0.f, 10.f, 0.f)));
  CHECK_THAT(hit.normal, IsNearlyEqualToVector(-Raz::Axis::X));
  CHECK_THAT(hit.distance, IsNearlyEqualTo(10.f));

  CHECK(ray2.intersects(sphere2, &hit));
  CHECK_THAT(hit.position, IsNearlyEqualToVector(Raz::Vec3f(5.0000005f, 5.0000005f, 0.f)));
  CHECK_THAT(hit.normal, IsNearlyEqualToVector(-Raz::Axis::Y));
  CHECK_THAT(hit.distance, IsNearlyEqualTo(8.4852819f));

  CHECK_FALSE(ray3.intersects(sphere2));

  CHECK_FALSE(ray1.intersects(sphere3));

  CHECK_FALSE(ray2.intersects(sphere3));

  CHECK(ray3.intersects(sphere3, &hit));
  CHECK_THAT(hit.position, IsNearlyEqualToVector(Raz::Vec3f(-9.2928934f, -9.2928934f, 0.f)));
  CHECK_THAT(hit.normal, IsNearlyEqualToVector(Raz::Vec3f(0.70710683f, 0.70710683f, 0.f)));
  CHECK_THAT(hit.distance, IsNearlyEqualTo(14.5563498f));
}

TEST_CASE("Ray-triangle intersection", "[utils]") {
  // See: https://www.geogebra.org/m/e8uqvjwh

  // These triangles are defined so that:
  //  - triangle1 is laying flat slightly above 0
  //  - triangle2 is standing, parallel to the Y/Z plane (facing the X direction)
  //  - triangle3 is crooked, its head pointing to [ -X; +Y ], slightly below 0
  const Raz::Triangle triangle1(Raz::Vec3f(-3.f, 0.5f, 3.f), Raz::Vec3f(3.f, 0.5f, 3.f), Raz::Vec3f(0.f, 0.5f, -6.f));
  const Raz::Triangle triangle2(Raz::Vec3f(0.5f, -0.5f, 3.f), Raz::Vec3f(0.5f, -0.5f, -3.f), Raz::Vec3f(0.5f, 3.f, 0.f));
  const Raz::Triangle triangle3(Raz::Vec3f(0.f, -1.f, 1.f), Raz::Vec3f(-1.5f, -1.5f, 0.f), Raz::Vec3f(0.f, -1.75f, -1.f));

  Raz::RayHit hit;

  CHECK(ray1.intersects(triangle1, &hit));
  CHECK(hit.position == Raz::Vec3f(0.f, 0.5f, 0.f));
  CHECK(hit.normal   == Raz::Axis::Y);
  CHECK(hit.distance == 0.5f);

  CHECK(ray2.intersects(triangle1, &hit));
  CHECK(hit.position == Raz::Vec3f(0.5f, 0.5f, 0.f));
  CHECK(hit.normal   == Raz::Axis::Y);
  CHECK_THAT(hit.distance, IsNearlyEqualTo(2.1213205f));

  CHECK(ray3.intersects(triangle1, &hit));
  CHECK(hit.position == Raz::Vec3f(0.5f, 0.5f, 0.f));
  CHECK(hit.normal   == Raz::Axis::Y);
  CHECK_THAT(hit.distance, IsNearlyEqualTo(0.7071068f));

  CHECK_FALSE(ray1.intersects(triangle2));

  CHECK(ray2.intersects(triangle2, &hit));
  CHECK(hit.position == Raz::Vec3f(0.5f, 0.5f, 0.f));
  CHECK(hit.normal   == Raz::Axis::X);
  CHECK_THAT(hit.distance, IsNearlyEqualTo(2.1213202f));

  CHECK(ray3.intersects(triangle2, &hit));
  CHECK(hit.position == Raz::Vec3f(0.5f, 0.5f, 0.f));
  CHECK(hit.normal   == Raz::Axis::X);
  CHECK_THAT(hit.distance, IsNearlyEqualTo(0.7071068f));

  CHECK_FALSE(ray1.intersects(triangle3));

  CHECK_FALSE(ray2.intersects(triangle3));

  CHECK(ray3.intersects(triangle3, &hit));
  // The second point is almost aligned with the ray; see https://www.geogebra.org/m/g4pumzwu
  CHECK_THAT(hit.position, IsNearlyEqualToVector(Raz::Vec3f(-1.5000002f, -1.5000002f, 0.f)));
  CHECK_THAT(hit.normal, IsNearlyEqualToVector(Raz::Vec3f(0.077791f, -0.9334918f, 0.3500594f)));
  CHECK_THAT(hit.distance, IsNearlyEqualTo(3.5355341f));
}

TEST_CASE("Ray-AABB intersection", "[utils]") {
  //         _______________________
  //        /|                    /|
  //       / |                   / | / 1 -> [ 0.5; 0.5; 0.5 ]
  //      |---------------------| < {  2 -> [   5;   5;   5 ]
  //      |  |                  |  | \ 3 -> [  -6;  -5;   5 ]
  //      |  |                  |  |
  //      |  |                  |  |
  //      |  |                  |  |
  //      | /-------------------|-/
  //      |/ ^                  |/
  //      ---|-------------------
  //         |
  //  1 -> [ -0.5; -0.5; -0.5 ]
  //  2 -> [    2;    3;   -5 ]
  //  3 -> [  -10;  -10;   -5 ]

  // See: https://www.geogebra.org/m/uwrt4ecn

  const Raz::AABB aabb1(Raz::Vec3f(-0.5f), Raz::Vec3f(0.5f));
  const Raz::AABB aabb2(Raz::Vec3f(2.f, 3.f, -5.f), Raz::Vec3f(5.f));
  const Raz::AABB aabb3(Raz::Vec3f(-10.f, -10.f, -5.f), Raz::Vec3f(-6.f, -5.f, 5.f));

  Raz::RayHit hit;

  CHECK(ray1.intersects(aabb1, &hit));
  // Since the ray is inside the box, the point returned is the intersection behind with a negative distance
  CHECK(hit.position == Raz::Vec3f(0.f, -0.5f, 0.f));
  CHECK(hit.normal   == -Raz::Axis::Y);
  CHECK(hit.distance == -0.5f);

  CHECK(ray2.intersects(aabb1, &hit));
  CHECK(hit.position == Raz::Vec3f(-0.5f, -0.5f, 0.f));
  CHECK(hit.normal   == (-Raz::Axis::X - Raz::Axis::Y).normalize()); // This is (literally) an edge case, the normal being perfectly diagonal
  CHECK_THAT(hit.distance, IsNearlyEqualTo(0.7071068f));

  CHECK(ray3.intersects(aabb1, &hit));
  CHECK(hit.position == Raz::Vec3f(0.5f, 0.5f, 0.f));
  CHECK(hit.normal   == (Raz::Axis::X + Raz::Axis::Y).normalize()); // Same edge case, but in the opposite direction
  CHECK_THAT(hit.distance, IsNearlyEqualTo(0.7071068f));

  CHECK_FALSE(ray1.intersects(aabb2));

  CHECK(ray2.intersects(aabb2, &hit));
  CHECK(hit.position == Raz::Vec3f(3.f, 3.f, 0.f));
  CHECK(hit.normal   == -Raz::Axis::Y);
  CHECK_THAT(hit.distance, IsNearlyEqualTo(5.6568542f));

  CHECK_FALSE(ray3.intersects(aabb2));

  CHECK_FALSE(ray1.intersects(aabb3));

  CHECK_FALSE(ray2.intersects(aabb3));

  CHECK(ray3.intersects(aabb3, &hit));
  CHECK(hit.position == Raz::Vec3f(-6.f, -6.f, 0.f));
  CHECK(hit.normal   == Raz::Axis::X);
  CHECK_THAT(hit.distance, IsNearlyEqualTo(9.8994951f));

  // TODO:
  //  A check must be made with a ray's origin lying precisely on a slab
  //  When this happens, with a naive implementation, only NaNs are returned; however, this ray must be considered intersecting the box properly
  //  See: https://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/

  //const Raz::Ray slabRay(Raz::Vec3f(-0.5f, -0.5f, 0.f), Raz::Axis::Y);
  //CHECK(slabRay.intersects(aabb1, &hit));
  //CHECK(hit.position == slabRay.getOrigin());
  //CHECK(hit.normal   == -Raz::Axis::Y);
  //CHECK(hit.distance == 0.f);
}

TEST_CASE("Point projection", "[utils]") {
  const Raz::Vec3f topPoint(0.f, 2.f, 0.f);
  const Raz::Vec3f topRightPoint(2.f, 2.f, 0.f);

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //        ^
  //        |
  //        x < [ 0; 0 ]
  CHECK(ray1.computeProjection(topPoint) == topPoint);
  CHECK(ray1.computeProjection(topRightPoint) == topPoint);

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //          ^
  //         /
  //        x < [ 0; 0 ]
  CHECK(ray2.computeProjection(topPoint) == Raz::Vec3f(1.f, 1.f, 0.f));
  CHECK(ray2.computeProjection(topRightPoint) == topRightPoint);

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //             x < [ 1; 1 ]
  //            /
  //           v
  CHECK(ray3.computeProjection(topPoint) == ray3.getOrigin());
  CHECK(ray3.computeProjection(topRightPoint) == ray3.getOrigin());
}
