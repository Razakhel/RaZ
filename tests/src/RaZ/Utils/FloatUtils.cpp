#include "RaZ/Utils/FloatUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Near-equality absolute", "[utils]") {
  constexpr float floatEpsilon            = std::numeric_limits<float>::epsilon();
  constexpr double doubleEpsilon          = std::numeric_limits<double>::epsilon();
  constexpr long double longDoubleEpsilon = std::numeric_limits<long double>::epsilon();

  // An epsilon difference is considered nearly equal
  CHECK(Raz::FloatUtils::areNearlyEqual(0.f,  floatEpsilon));
  CHECK(Raz::FloatUtils::areNearlyEqual(0.0,  doubleEpsilon));
  CHECK(Raz::FloatUtils::areNearlyEqual(0.0L, longDoubleEpsilon));

  // A two-epsilon difference is however not considered nearly equal
  CHECK_FALSE(Raz::FloatUtils::areNearlyEqual(0.f,  floatEpsilon * 2.f));
  CHECK_FALSE(Raz::FloatUtils::areNearlyEqual(0.0,  doubleEpsilon * 2.0));
  CHECK_FALSE(Raz::FloatUtils::areNearlyEqual(0.0L, longDoubleEpsilon * 2.0L));
}
