#include "catch/catch.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace {

// Declaring shapes to be tested
const Raz::Line line1(Raz::Vec3f({ 0.f, 0.f, 0.f }), Raz::Vec3f({ 1.f, 0.f, 0.f }));
const Raz::Line line2(Raz::Vec3f(-1.f), Raz::Vec3f(1.f));

const Raz::Plane plane(1.f, Raz::Axis::Y);

} // namespace

TEST_CASE("Line basic checks") {
  REQUIRE(Raz::FloatUtils::checkNearEquality(line1.computeLength(), 1.f));
  REQUIRE(Raz::FloatUtils::checkNearEquality(line1.computeSquaredLength(), 1.f));

  REQUIRE(Raz::FloatUtils::checkNearEquality(line2.computeLength(), 3.464101615f));
  REQUIRE(Raz::FloatUtils::checkNearEquality(line2.computeSquaredLength(), 12.f));
}

TEST_CASE("Plane basic checks") {
  const Raz::Plane plane2(Raz::Vec3f({ 0.f, 1.f, 0.f }), Raz::Axis::Y);
  const Raz::Plane plane3(Raz::Vec3f({ 1.f, 1.f, 0.f }), Raz::Vec3f({ -1.f, 1.f, -1.f }), Raz::Vec3f({ 0.f, 1.f, 1.f }));

  // Checking that the 3 planes are strictly equal to each other
  REQUIRE(Raz::FloatUtils::checkNearEquality(plane.getDistance(), plane2.getDistance()));
  REQUIRE(Raz::FloatUtils::checkNearEquality(plane.getDistance(), plane3.getDistance()));

  REQUIRE(plane.getNormal() == plane2.getNormal());
  REQUIRE(plane.getNormal() == plane3.getNormal());
}
