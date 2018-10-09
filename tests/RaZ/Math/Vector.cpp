#include "catch/catch.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace {

// Declaring vectors to be tested
const Raz::Vec3f vec31({ 3.18f, 42.f, 0.874f });
const Raz::Vec3f vec32({ 541.41f, 47.25f, 6.321f });

// Near-equality floating point check
template <typename T>
bool compareFloatingPoint(T val1, T val2) {
  static_assert(std::is_floating_point<T>::value, "Error: Values' type must be floating point.");

  return std::abs(val1 - val2) <= std::numeric_limits<T>::epsilon() * std::max({ static_cast<T>(1), std::abs(val1), std::abs(val2) });
}

} // namespace

TEST_CASE("Vector near-equality") {
  REQUIRE_FALSE(vec31 == vec32);

  const Raz::Vec3f baseVec(1.f);
  Raz::Vec3f compVec = baseVec;

  REQUIRE(baseVec[0] == compVec[0]); // Copied, strict equality
  REQUIRE(baseVec[1] == compVec[1]);
  REQUIRE(baseVec[2] == compVec[2]);

  compVec += 0.0000001f; // Adding a tiny offset

  REQUIRE_FALSE(baseVec[0] == compVec[0]); // Values not strictly equal
  REQUIRE_FALSE(baseVec[1] == compVec[1]);
  REQUIRE_FALSE(baseVec[2] == compVec[2]);

  REQUIRE(compareFloatingPoint(baseVec[0], compVec[0])); // Near-equality components check
  REQUIRE(compareFloatingPoint(baseVec[1], compVec[1]));
  REQUIRE(compareFloatingPoint(baseVec[2], compVec[2]));

  REQUIRE(baseVec == compVec); // Vector::operator== does a near-equality check on floating point types
}
