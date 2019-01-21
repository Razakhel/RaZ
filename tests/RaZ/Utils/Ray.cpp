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
