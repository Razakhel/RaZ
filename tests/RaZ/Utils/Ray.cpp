#include "catch/catch.hpp"
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

TEST_CASE("Ray-point intersection checks") {
  REQUIRE(ray1.intersects(ray1.getOrigin()));

  const Raz::Vec3f topPoint({ 0.f, 2.f, 0.f });
  const Raz::Vec3f topRightPoint({ 2.f, 2.f, 0.f });

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //        ^
  //        |
  //        x < [ 0; 0 ]
  REQUIRE(ray1.intersects(topPoint));
  REQUIRE_FALSE(ray1.intersects(topRightPoint));

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //          ^
  //         /
  //        x < [ 0; 0 ]
  REQUIRE_FALSE(ray2.intersects(topPoint));
  REQUIRE(ray2.intersects(topRightPoint));

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //             x < [ 1; 1 ]
  //            /
  //           v
  REQUIRE_FALSE(ray3.intersects(topPoint));
  REQUIRE_FALSE(ray3.intersects(topRightPoint));
}

TEST_CASE("Ray-sphere intersection") {
  const Raz::Sphere sphere1(Raz::Vec3f(0.f), 1.f);
  const Raz::Sphere sphere2(Raz::Vec3f({ 5.f, 10.f, 0.f }), 5.f);
  const Raz::Sphere sphere3(Raz::Vec3f({ -10.f, -10.f, 0.f }), 1.f);

  REQUIRE(ray1.intersects(sphere1));
  REQUIRE(ray2.intersects(sphere1));
  REQUIRE(ray3.intersects(sphere1));

  REQUIRE(ray1.intersects(sphere2));
  REQUIRE(ray2.intersects(sphere2));
  REQUIRE_FALSE(ray3.intersects(sphere2));

  REQUIRE_FALSE(ray1.intersects(sphere3));
  REQUIRE_FALSE(ray2.intersects(sphere3));
  REQUIRE(ray3.intersects(sphere3));
}

TEST_CASE("Point projection checks") {
  const Raz::Vec3f topPoint({ 0.f, 2.f, 0.f });
  const Raz::Vec3f topRightPoint({ 2.f, 2.f, 0.f });

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //        ^
  //        |
  //        x < [ 0; 0 ]
  REQUIRE(ray1.computeProjection(topPoint) == topPoint);
  REQUIRE(ray1.computeProjection(topRightPoint) == topPoint);

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //          ^
  //         /
  //        x < [ 0; 0 ]
  REQUIRE(ray2.computeProjection(topPoint) == Raz::Vec3f({ 1.f, 1.f, 0.f }));
  REQUIRE(ray2.computeProjection(topRightPoint) == topRightPoint);

  //     topPoint  topRightPoint
  //     [ 0; 2 ]    [ 2; 2 ]
  //
  //             x < [ 1; 1 ]
  //            /
  //           v
  REQUIRE(ray3.computeProjection(topPoint) == ray3.getOrigin());
  REQUIRE(ray3.computeProjection(topRightPoint) == ray3.getOrigin());
}
