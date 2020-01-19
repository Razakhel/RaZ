#include "Catch.hpp"

#include "RaZ/Utils/Ray.hpp"

namespace {

// Declaring rays to be tested
//
//        Ray 1      |       Ray 2       |      Ray 3
//                   |                   |
//          ^        |           ^       |      [ 1; 1 ]
//          |        |          /        |         /
//          |        |         /         |        /
//      [ 0; 0 ]     |    [ -1; -1 ]     |       v

const Raz::Ray ray1(Raz::Vec3f({ 0.f, 0.f, 0.f }), Raz::Axis::Y);
const Raz::Ray ray2(Raz::Vec3f({ -1.f, -1.f, 0.f }), Raz::Vec3f({ 1.f, 1.f, 0.f }).normalize());
const Raz::Ray ray3(Raz::Vec3f({ 1.f, 1.f, 0.f }), Raz::Vec3f({ -1.f, -1.f, 0.f }).normalize());

} // namespace

TEST_CASE("Ray-point intersection") {
  Raz::RayHit hit;

  CHECK(ray1.intersects(ray1.getOrigin(), &hit));

  CHECK(hit.position == ray1.getOrigin());
  CHECK(hit.normal   == Raz::Vec3f(0.f));
  CHECK(hit.distance == 0.f);

  const Raz::Vec3f topPoint({ 0.f, 2.f, 0.f });
  const Raz::Vec3f topRightPoint({ 2.f, 2.f, 0.f });

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
  CHECK(hit.distance == Raz::Vec3f({ 3.f, 3.f, 0.f }).computeLength()); // 4.2426405f

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //             x < [ 1; 1 ]
  //            /
  //           v

  CHECK_FALSE(ray3.intersects(topPoint));
  CHECK_FALSE(ray3.intersects(topRightPoint));
}

TEST_CASE("Ray-plane intersection") {
  //       Plane 1      |      Plane 2      |      Plane 3      |      Plane 4
  //                    |                   |                   |
  //   \      normal    |    \              |              /    |
  //     \      ^       |      \            |            /      |        normal
  //       \   /        |        \          |          /        |          ^
  //         \/         |        / \        |        /  \       |          |
  //           \        |       /    \      |      /     \      |    ______|______
  //             \      |      v       \    |    /        v     |
  //               \    |   normal          |          normal   |     [ 0; 0.5 ]

  const Raz::Vec3f initPos({ 0.f, 0.5f, 0.f });
  const Raz::Plane plane1(initPos, Raz::Vec3f({ 1.f, 1.f, 0.f }).normalize());
  const Raz::Plane plane2(initPos, Raz::Vec3f({ -1.f, -1.f, 0.f }).normalize());
  const Raz::Plane plane3(initPos, Raz::Vec3f({ 1.f, -1.f, 0.f }).normalize());
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

TEST_CASE("Ray-sphere intersection") {
  const Raz::Sphere sphere1(Raz::Vec3f(0.f), 1.f);
  const Raz::Sphere sphere2(Raz::Vec3f({ 5.f, 10.f, 0.f }), 5.f);
  const Raz::Sphere sphere3(Raz::Vec3f({ -10.f, -10.f, 0.f }), 1.f);

  CHECK(ray1.intersects(sphere1));
  CHECK(ray2.intersects(sphere1));
  CHECK(ray3.intersects(sphere1));

  CHECK(ray1.intersects(sphere2));
  CHECK(ray2.intersects(sphere2));
  CHECK_FALSE(ray3.intersects(sphere2));

  CHECK_FALSE(ray1.intersects(sphere3));
  CHECK_FALSE(ray2.intersects(sphere3));
  CHECK(ray3.intersects(sphere3));
}

TEST_CASE("Ray-triangle intersection") {
  // These triangles are defined so that:
  //  - triangle1 is laying flat slightly above 0
  //  - triangle2 is standing, parallel to the Y/Z plane (facing the X direction)
  //  - triangle3 is crooked, its head pointing to [ -X; +Y ], slightly below 0
  const Raz::Triangle triangle1(Raz::Vec3f({ -3.f, 0.5f, 3.f }), Raz::Vec3f({ 0.f, 0.5f, -3.f }), Raz::Vec3f({ 3.f, 0.5f, 3.f }));
  const Raz::Triangle triangle2(Raz::Vec3f({ 0.5f, -0.5f, 3.f }), Raz::Vec3f({ 0.5f, -0.5f, -3.f }), Raz::Vec3f({ 0.5f, 3.f, 0.f }));
  const Raz::Triangle triangle3(Raz::Vec3f({ 0.f, -1.f, 1.f }), Raz::Vec3f({ -1.5f, -1.5f, 0.f }), Raz::Vec3f({ 0.f, -1.75f, -1.f }));

  CHECK(ray1.intersects(triangle1));
  CHECK(ray2.intersects(triangle1));
  CHECK(ray3.intersects(triangle1));

  CHECK_FALSE(ray1.intersects(triangle2));
  CHECK(ray2.intersects(triangle2));
  CHECK(ray3.intersects(triangle2));

  CHECK_FALSE(ray1.intersects(triangle3));
  CHECK_FALSE(ray2.intersects(triangle3));
  CHECK(ray3.intersects(triangle3));
}

TEST_CASE("Ray-AABB intersection") {
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

  const Raz::AABB aabb1(Raz::Vec3f(-1.f), Raz::Vec3f(1.f));
  const Raz::AABB aabb2(Raz::Vec3f({ 3.f, 3.f, -5.f }), Raz::Vec3f(5.f));
  const Raz::AABB aabb3(Raz::Vec3f({ -10.f, -10.f, -5.f }), Raz::Vec3f({ -5.f, -5.f, 5.f }));

  CHECK(ray1.intersects(aabb1));
  CHECK(ray2.intersects(aabb1));
  CHECK(ray3.intersects(aabb1));

  CHECK_FALSE(ray1.intersects(aabb2));
  CHECK(ray2.intersects(aabb2));
  CHECK_FALSE(ray3.intersects(aabb2));

  CHECK_FALSE(ray1.intersects(aabb3));
  CHECK_FALSE(ray2.intersects(aabb3));
  CHECK(ray3.intersects(aabb3));
}

TEST_CASE("Point projection") {
  const Raz::Vec3f topPoint({ 0.f, 2.f, 0.f });
  const Raz::Vec3f topRightPoint({ 2.f, 2.f, 0.f });

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
  CHECK(ray2.computeProjection(topPoint) == Raz::Vec3f({ 1.f, 1.f, 0.f }));
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
