#include "catch/catch.hpp"
#include "RaZ/Utils/FloatUtils.hpp"

TEST_CASE("Near-equality absolute") {
  constexpr float floatEpsilon   = std::numeric_limits<float>::epsilon();
  constexpr double doubleEpsilon = std::numeric_limits<double>::epsilon();

  REQUIRE(Raz::FloatUtils::areNearlyEqual(0.f, floatEpsilon));
  REQUIRE(Raz::FloatUtils::areNearlyEqual(0.0, doubleEpsilon));

  REQUIRE_FALSE(Raz::FloatUtils::areNearlyEqual(0.f, floatEpsilon * 2.f));
  REQUIRE_FALSE(Raz::FloatUtils::areNearlyEqual(0.0, doubleEpsilon * 2.0));
}
