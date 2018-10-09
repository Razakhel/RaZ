#include "catch/catch.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace {

// Declaring vectors to be tested
const Raz::Vec3f vec31({ 3.18f, 42.f, 0.874f });
const Raz::Vec3f vec32({ 541.41f, 47.25f, 6.321f });

const Raz::Vec4f vec41({ 84.47f, 2.f, 0.001f, 847.12f });

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

TEST_CASE("Vector/scalar operations") {
  REQUIRE((vec31 * 3.f) == Raz::Vec3f({ 9.54f, 126.f, 2.622f }));
  REQUIRE((vec31 * 4.152f) == Raz::Vec3f({ 13.20336f, 174.384f, 3.628848f }));

  REQUIRE((vec41 * 7.5f) == Raz::Vec4f({ 633.525f, 15.f, 0.0075f, 6353.4f }));
  REQUIRE((vec41 * 8.0002f) == Raz::Vec4f({ 675.776894f, 16.0004f, 0.0080002f, 6777.129424f }));
  REQUIRE((vec41 * 0.f) == Raz::Vec4f({ 0.f, 0.f, 0.f, 0.f }));
}

TEST_CASE("Vector/vector operations") {
  REQUIRE((vec31 - vec31) == Raz::Vec3f(0.f));
  REQUIRE((vec31 * vec32) == Raz::Vec3f({ 1721.6838f, 1984.5f, 5.524554f }));

  REQUIRE(vec31.dot(vec31) == vec31.computeSquaredLength());
  REQUIRE(compareFloatingPoint(vec31.dot(vec31), 1774.876276f));
  REQUIRE(compareFloatingPoint(vec31.dot(vec32), 3711.708354f));
  REQUIRE(vec31.dot(vec32) == vec32.dot(vec31)); // A · B == B · A

  REQUIRE(vec31.cross(vec32) == Raz::Vec3f({ 224.1855f, 453.09156f, -22588.965f }));
  REQUIRE(vec31.cross(vec32) == -vec32.cross(vec31)); // A x B == -(B x A)
}
