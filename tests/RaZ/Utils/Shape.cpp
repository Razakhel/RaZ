#include "catch/catch.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace {

// Declaring shapes to be tested
const Raz::Line line1(Raz::Vec3f({ 0.f, 0.f, 0.f }), Raz::Vec3f({ 1.f, 0.f, 0.f }));
const Raz::Line line2(Raz::Vec3f(-1.f), Raz::Vec3f(1.f));

} // namespace

TEST_CASE("Line basic checks") {
  REQUIRE(Raz::FloatUtils::checkNearEquality(line1.computeLength(), 1.f));
  REQUIRE(Raz::FloatUtils::checkNearEquality(line1.computeSquaredLength(), 1.f));

  REQUIRE(Raz::FloatUtils::checkNearEquality(line2.computeLength(), 3.464101615f));
  REQUIRE(Raz::FloatUtils::checkNearEquality(line2.computeSquaredLength(), 12.f));
}
