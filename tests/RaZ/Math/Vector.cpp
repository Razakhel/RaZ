#include "Catch.hpp"

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/FloatUtils.hpp"

#include <unordered_map>

namespace {

// Declaring vectors to be tested
constexpr Raz::Vec3f vec31(3.18f, 42.f, 0.874f);
constexpr Raz::Vec3f vec32(541.41f, 47.25f, 6.321f);

constexpr Raz::Vec4f vec41(84.47f, 2.f, 0.001f, 847.12f);
constexpr Raz::Vec4f vec42(13.01f, 0.15f, 84.8f, 72.f);

} // namespace

TEST_CASE("Vector deduction guides") {
  constexpr Raz::Vector vec2i(1, 2);
  constexpr Raz::Vector vec3b(static_cast<uint8_t>(1), 2, 3);
  constexpr Raz::Vector vec3f(1.f, 2.f, 3.f);
  constexpr Raz::Vector vec4u(1u, 2u, 3u, 4u);

  CHECK(std::is_same_v<std::decay_t<decltype(vec2i)>, Raz::Vec2i>);
  CHECK(std::is_same_v<std::decay_t<decltype(vec3b)>, Raz::Vec3b>);
  CHECK(std::is_same_v<std::decay_t<decltype(vec3f)>, Raz::Vec3f>);
  CHECK(std::is_same_v<std::decay_t<decltype(vec4u)>, Raz::Vec4u>);
}

TEST_CASE("Vector indexing") {
  CHECK(vec31[0] == 3.18f);
  CHECK(vec31[1] == 42.f);
  CHECK(vec31[2] == 0.874f);

  CHECK(vec31.x() == 3.18f);
  CHECK(vec31.y() == 42.f);
  CHECK(vec31.z() == 0.874f);

  Raz::Vec4f vecCopy = vec41;

  vecCopy[0] += 1.f;
  vecCopy[1] += 1.f;
  vecCopy[2] += 1.f;
  vecCopy[3] += 1.f;

  vecCopy.x() += 1.f;
  vecCopy.y() += 1.f;
  vecCopy.z() += 1.f;
  vecCopy.w() += 1.f;

  CHECK(vecCopy[0] == vec41[0] + 2.f);
  CHECK(vecCopy[1] == vec41[1] + 2.f);
  CHECK(vecCopy[2] == vec41[2] + 2.f);
  CHECK(vecCopy[3] == vec41[3] + 2.f);

  CHECK(vecCopy.x() == vec41.x() + 2.f);
  CHECK(vecCopy.y() == vec41.y() + 2.f);
  CHECK(vecCopy.z() == vec41.z() + 2.f);
  CHECK(vecCopy.w() == vec41.w() + 2.f);
}

TEST_CASE("Vector/scalar operations") {
  CHECK((vec31 + 3.5f) == Raz::Vec3f(6.68f, 45.5f, 4.374f));
  CHECK((vec32 - 74.42f) == Raz::Vec3f(466.99f, -27.17f, -68.099f));
  CHECK((vec31 + 8.2f) == (8.2f + vec31));

  CHECK((vec31 * 3.f) == Raz::Vec3f(9.54f, 126.f, 2.622f));
  CHECK((vec31 * 4.152f) == Raz::Vec3f(13.20336f, 174.384f, 3.628848f));
  CHECK((vec31 * 6.31f) == (6.31f * vec31));

  CHECK((vec41 * 7.5f) == Raz::Vec4f(633.525f, 15.f, 0.0075f, 6353.4f));
  CHECK((vec41 * 8.0002f) == Raz::Vec4f(675.776894f, 16.0004f, 0.0080002f, 6777.129424f));
  CHECK((vec41 * 0.f) == Raz::Vec4f(0.f, 0.f, 0.f, 0.f));

  CHECK((vec31 / 2.f) == Raz::Vec3f(1.59f, 21.f, 0.437f));
  CHECK((vec42 / 7.32f) == Raz::Vec4f(1.777322404f, 0.0204918f, 11.58469945f, 9.83606557f));

  // If IEEE 754 is supported, check that division by 0 behaves as expected
  if constexpr (std::numeric_limits<float>::is_iec559) {
    const Raz::Vec3f vecInf = vec31 / 0.f;
    CHECK(vecInf[0] == std::numeric_limits<float>::infinity());
    CHECK(vecInf[1] == std::numeric_limits<float>::infinity());
    CHECK(vecInf[2] == std::numeric_limits<float>::infinity());

    const Raz::Vec3f vecNaN = Raz::Vec3f(0.f) / 0.f;
    CHECK(std::isnan(vecNaN[0]));
    CHECK(std::isnan(vecNaN[1]));
    CHECK(std::isnan(vecNaN[2]));
  }
}

TEST_CASE("Vector/vector operations") {
  CHECK((vec31 - vec31) == Raz::Vec3f(0.f));
  CHECK((vec31 * vec32) == Raz::Vec3f(1721.6838f, 1984.5f, 5.524554f));

  CHECK(vec31.dot(vec31) == vec31.computeSquaredLength());
  CHECK_THAT(vec31.dot(vec31), IsNearlyEqualTo(1774.876276f));
  CHECK_THAT(vec31.dot(vec32), IsNearlyEqualTo(3711.708354f));
  CHECK(vec31.dot(vec32) == vec32.dot(vec31)); // A · B == B · A

  CHECK(vec31.cross(vec32) == Raz::Vec3f(224.1855f, 453.09156f, -22588.965f));
  CHECK(vec31.cross(vec32) == -vec32.cross(vec31)); // A x B == -(B x A)
}

TEST_CASE("Vector/matrix operations") {
  const Raz::Mat3f mat3( 4.12f,  25.1f,  30.7842f,
                         3.04f,  5.f,   -64.5f,
                        -1.f,   -7.54f,  8.41f);
  CHECK((vec31 * mat3) == Raz::Vec3f(139.9076f, 283.22804f, -2603.755904f));
  CHECK((vec32 * mat3) == Raz::Vec3f(2367.9282f, 13'777.980'66f, 13'672.408'332f));

  const Raz::Mat4f mat4(-3.2f,    53.032f,  832.451f,  74.2f,
                         10.01f,  3.15f,   -91.41f,    187.46f,
                        -6.f,    -7.78f,    90.f,      38.f,
                         123.f,  -74.8f,    147.0001f, 748.6f);
  CHECK((vec41 * mat4) == Raz::Vec4f(103'945.47f, -58878.67074f, 194'661.130'682f, 640'796.664f));
  CHECK((vec42 * mat4) == Raz::Vec4f(8307.0695f, -5354.92518f, 29032.48321f, 58115.061f));

  CHECK((vec31 * Raz::Mat3f::identity()) == vec31);
  CHECK((vec41 * Raz::Mat4f::identity()) == vec41);
}

TEST_CASE("Vector manipulations") {
  CHECK_THAT(vec31.normalize().computeLength(), IsNearlyEqualTo(1.f));
  CHECK_THAT(vec41.normalize().computeSquaredLength(), IsNearlyEqualTo(1.f));
  CHECK_THAT(Raz::Vec3f(0.f, 1.f, 0.f).computeLength(), IsNearlyEqualTo(1.f));

  // Testing Vector::reflect():
  //
  // IncVec  N  Reflection
  //     \   |   /
  //      \  |  /
  //       \ | /
  //________\|/___________
  //
  CHECK(Raz::Vec3f(1.f, -1.f, 0.f).reflect(Raz::Vec3f(0.f, 1.f, 0.f)) == Raz::Vec3f(1.f, 1.f, 0.f));
  CHECK(vec31.reflect(Raz::Vec3f(0.f, 1.f, 0.f)) == Raz::Vec3f(3.18f, -42.f, 0.874f));
  CHECK(vec31.reflect(vec32) == Raz::Vec3f(-4'019'108.859'878'28f, -350'714.439'453f, -46'922.543'011'268f));
}

TEST_CASE("Vector interpolation") {
  // lerp() doesn't normalize the resulting vector
  CHECK(vec31.lerp(vec32, 0.f) == vec31);
  CHECK(vec31.lerp(vec32, 0.25f) == Raz::Vec3f(137.73749f, 43.3125f, 2.23575f)); // (vec31 * 3 + vec32) / 4
  CHECK(vec31.lerp(vec32, 0.5f) == (vec31 + vec32) / 2);
  CHECK(vec31.lerp(vec32, 0.75f) == Raz::Vec3f(406.85248f, 45.9375f, 4.95925f)); // (vec31 + vec32 * 3) / 4
  CHECK(vec31.lerp(vec32, 1.f) == vec32);

  // nlerp() does normalize the resulting vector; it is strictly equal to vec.lerp(...).normalize()
  CHECK(vec41.nlerp(vec42, 0.f) == vec41.normalize());
  CHECK(vec41.nlerp(vec42, 0.25f) == Raz::Vec4f(0.10136666f, 0.00233993f, 0.0322656f, 0.994323f)); // (vec41 * 3 + vec42).normalize()
  CHECK(vec41.nlerp(vec42, 0.5f) == (vec41 + vec42).normalize());
  CHECK(vec41.nlerp(vec42, 0.75f) == Raz::Vec4f(0.11226334f, 0.00222709f, 0.23125429f, 0.966392f)); // (vec41 + vec42 * 3).normalize()
  CHECK(vec41.nlerp(vec42, 1.f) == vec42.normalize());
}

TEST_CASE("Vector hash") {
  CHECK(vec31.hash() == vec31.hash());
  CHECK_FALSE(vec31.hash() == vec32.hash());

  CHECK(vec41.hash() == vec41.hash());
  CHECK_FALSE(vec41.hash() == vec42.hash());

  constexpr Raz::Vec3f vec31Swizzled(vec31[2], vec31[0], vec31[1]);
  CHECK_FALSE(vec31.hash() == vec31Swizzled.hash());

  constexpr Raz::Vec3f vec31Epsilon = vec31 + std::numeric_limits<float>::epsilon();
  CHECK_FALSE(vec31.hash() == vec31Epsilon.hash());

  // Checking that it behaves as expected when used in a hashmap
  std::unordered_map<Raz::Vec3f, int> map;
  map.try_emplace(vec31, 1);
  map.try_emplace(vec31Swizzled, 2);
  map.try_emplace(vec31Epsilon, 3);
  map.try_emplace(vec32, 4);

  CHECK(map.size() == 4);

  CHECK(map.find(vec31)->second == 1);
  CHECK(map.find(vec31Swizzled)->second == 2);
  CHECK(map.find(vec31Epsilon)->second == 3);
  CHECK(map.find(vec32)->second == 4);

  map.erase(vec31Epsilon);
  CHECK(map.find(vec31Epsilon) == map.cend());
}

TEST_CASE("Vector near-equality") {
  CHECK_FALSE(vec31 == vec32);

  constexpr Raz::Vec3f baseVec(1.f);
  Raz::Vec3f compVec = baseVec;

  CHECK(baseVec[0] == compVec[0]); // Copied, strict equality
  CHECK(baseVec[1] == compVec[1]);
  CHECK(baseVec[2] == compVec[2]);

  compVec += 0.0000001f; // Adding a tiny offset

  CHECK_FALSE(baseVec[0] == compVec[0]); // Values not strictly equal
  CHECK_FALSE(baseVec[1] == compVec[1]);
  CHECK_FALSE(baseVec[2] == compVec[2]);

  CHECK_THAT(baseVec[0], IsNearlyEqualTo(compVec[0])); // Near-equality components check
  CHECK_THAT(baseVec[1], IsNearlyEqualTo(compVec[1]));
  CHECK_THAT(baseVec[2], IsNearlyEqualTo(compVec[2]));

  CHECK_THAT(baseVec, IsNearlyEqualToVector(compVec)); // Simpler check in a single call

  CHECK(baseVec == compVec); // Vector::operator== does a near-equality check on floating point types
}

TEST_CASE("Vector strict equality") {
  CHECK(vec31.strictlyEquals(vec31));
  CHECK(vec41.strictlyEquals(vec41));

  CHECK(std::equal_to<Raz::Vec3f>()(vec31, vec31));
  CHECK(std::equal_to<Raz::Vec4f>()(vec41, vec41));

  constexpr Raz::Vec3f vec31Swizzled(vec31[2], vec31[0], vec31[1]);
  CHECK_FALSE(vec31.strictlyEquals(vec31Swizzled));
  CHECK_FALSE(std::equal_to<Raz::Vec3f>()(vec31, vec31Swizzled));

  constexpr Raz::Vec3f vec31Epsilon = vec31 + std::numeric_limits<float>::epsilon();
  CHECK(vec31 == vec31Epsilon); // Near-equality check
  CHECK_FALSE(vec31.strictlyEquals(vec31Epsilon)); // Strict-equality checks
  CHECK_FALSE(std::equal_to<Raz::Vec3f>()(vec31, vec31Epsilon));

  constexpr std::array<Raz::Vec3f, 3> vectors = { vec31Swizzled, vec31Epsilon, vec31 };

  // When using a simple find(), which uses operator==, vec31Epsilon is found instead of vec31
  const auto foundIter = std::find(vectors.cbegin(), vectors.cend(), vec31);
  CHECK_FALSE(std::distance(vectors.cbegin(), foundIter) == 2);

  // To search for a specific element, find_if() must be used with a strict equality check
  const auto foundIfIter = std::find_if(vectors.cbegin(), vectors.cend(), [&] (const Raz::Vec3f& compVec) { return vec31.strictlyEquals(compVec); });
  CHECK(std::distance(vectors.cbegin(), foundIfIter) == 2);

  constexpr std::array<Raz::Vec3f, 3> swappedVectors = { vec31Swizzled, vec31, vec31Epsilon };

  // Trying to compare for equality fails with a simple std::equal on the vectors
  CHECK(std::equal(vectors.cbegin(), vectors.cend(), swappedVectors.cbegin()));

  // When using the std::equal_to specialization, vectors are properly found to be unequal to each other
  CHECK_FALSE(std::equal(vectors.cbegin(), vectors.cend(), swappedVectors.cbegin(), std::equal_to<Raz::Vec3f>()));
}

TEST_CASE("Vector printing") {
  std::stringstream stream;

  stream << vec31;
  CHECK(stream.str() == "[ 3.18; 42; 0.874 ]");

  stream.str(std::string()); // Resetting the stream
  stream << vec32;
  CHECK(stream.str() == "[ 541.41; 47.25; 6.321 ]");

  stream.str(std::string());
  stream << vec41;
  CHECK(stream.str() == "[ 84.47; 2; 0.001; 847.12 ]");
}
