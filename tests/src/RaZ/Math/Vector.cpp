#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/FloatUtils.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

#include <unordered_map>
#include <utility>

namespace {

// Declaring vectors to be tested

constexpr Raz::Vec3b vec3b1(31, 8, 12);
constexpr Raz::Vec3b vec3b2(48, 255, 0);

constexpr Raz::Vec3i vec3i1(-48, 0, 18742);
constexpr Raz::Vec3i vec3i2(740, -1058, 18);

constexpr Raz::Vec3f vec3f1(3.18f, 42.f, 0.874f);
constexpr Raz::Vec3f vec3f2(541.41f, 47.25f, 6.321f);

constexpr Raz::Vec4f vec4f1(84.47f, 2.f, 0.001f, 847.12f);
constexpr Raz::Vec4f vec4f2(13.01f, 0.15f, 84.8f, 72.f);

constexpr Raz::Vec3d vec3d1(-18.1, 4752.001, -842.0);
constexpr Raz::Vec3d vec3d2(13.01, -0.00000001, 351.025465545);

} // namespace

TEST_CASE("Vector deduction guides", "[math]") {
  constexpr Raz::Vector vec3b(static_cast<uint8_t>(1), 2, 3);
  constexpr Raz::Vector vec2i(1, 2);
  constexpr Raz::Vector vec4u(1u, 2u, 3u, 4u);
  constexpr Raz::Vector vec3f(1.f, 2.f, 3.f);
  constexpr Raz::Vector vec5d(1.0, 2.0, 3.0, 4.0, 5.0);

  CHECK(std::is_same_v<std::decay_t<decltype(vec3b)>, Raz::Vec3b>);
  CHECK(std::is_same_v<std::decay_t<decltype(vec2i)>, Raz::Vec2i>);
  CHECK(std::is_same_v<std::decay_t<decltype(vec4u)>, Raz::Vec4u>);
  CHECK(std::is_same_v<std::decay_t<decltype(vec3f)>, Raz::Vec3f>);
  CHECK(std::is_same_v<std::decay_t<decltype(vec5d)>, Raz::Vector<double, 5>>);
}

TEST_CASE("Vector indexing", "[math]") {
  CHECK(vec3f1[0] == 3.18f);
  CHECK(vec3f1[1] == 42.f);
  CHECK(vec3f1[2] == 0.874f);

  CHECK(vec3f1.x() == 3.18f);
  CHECK(vec3f1.y() == 42.f);
  CHECK(vec3f1.z() == 0.874f);

  Raz::Vec4f vecCopy = vec4f1;

  vecCopy[0] += 1.f;
  vecCopy[1] += 1.f;
  vecCopy[2] += 1.f;
  vecCopy[3] += 1.f;

  vecCopy.x() += 1.f;
  vecCopy.y() += 1.f;
  vecCopy.z() += 1.f;
  vecCopy.w() += 1.f;

  CHECK(vecCopy[0] == vec4f1[0] + 2.f);
  CHECK(vecCopy[1] == vec4f1[1] + 2.f);
  CHECK(vecCopy[2] == vec4f1[2] + 2.f);
  CHECK(vecCopy[3] == vec4f1[3] + 2.f);

  CHECK(vecCopy.x() == vec4f1.x() + 2.f);
  CHECK(vecCopy.y() == vec4f1.y() + 2.f);
  CHECK(vecCopy.z() == vec4f1.z() + 2.f);
  CHECK(vecCopy.w() == vec4f1.w() + 2.f);
}

TEST_CASE("Vector dot", "[math]") {
  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3b>().dot(std::declval<Raz::Vec3b>())), uint64_t>);
  CHECK(vec3b1.dot(vec3b2) == 3528);

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3i>().dot(std::declval<Raz::Vec3i>())), uint64_t>);
  CHECK(vec3i1.dot(vec3i2) == 301836);

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3f>().dot(std::declval<Raz::Vec3f>())), float>);
  CHECK(vec3f1.dot(vec3f1) == vec3f1.computeSquaredLength());
  CHECK_THAT(vec3f1.dot(vec3f1), IsNearlyEqualTo(1774.876276f));
  CHECK_THAT(vec3f1.dot(vec3f2), IsNearlyEqualTo(3711.708354f));
  CHECK(vec3f1.dot(vec3f2) == vec3f2.dot(vec3f1)); // A · B == B · A

  CHECK_THAT(vec4f1.dot(vec4f2), IsNearlyEqualTo(62091.98f));

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3d>().dot(std::declval<Raz::Vec3d>())), double>);
  CHECK_THAT(vec3d1.dot(vec3d2), IsNearlyEqualTo(-295798.92303641));
}

TEST_CASE("Vector cross", "[math]") {
  // The cross product cannot be computed for unsigned types or sizes other than 3

  CHECK(vec3i1.cross(vec3i1) == Raz::Vec3i(0));
  CHECK(vec3i1.cross(vec3i2) == Raz::Vec3i(19829036, 13869944, 50784));

  CHECK_THAT(vec3f1.cross(vec3f1), IsNearlyEqualToVector(Raz::Vec3f(0.f), 0.00001f)); // Clang in Release gives values above the default tolerance
  CHECK_THAT(vec3f1.cross(vec3f2), IsNearlyEqualToVector(Raz::Vec3f(224.1855f, 453.09156f, -22588.965f)));
  CHECK(vec3f1.cross(vec3f2) == -vec3f2.cross(vec3f1)); // A x B == -(B x A)

  CHECK_THAT(vec3d1.cross(vec3d1), IsNearlyEqualToVector(Raz::Vec3d(0.0), 0.00001f));
  CHECK_THAT(vec3d1.cross(vec3d2), IsNearlyEqualToVector(Raz::Vec3d(1668073.3632868854, -4600.859073635499, -61823.533009819)));
}

TEST_CASE("Vector/scalar operations", "[math]") {
  CHECK((vec3b1 + 4) == Raz::Vec3b(35, 12, 16));
  CHECK((vec3b2 - 8) == Raz::Vec3b(40, 247, 248)); // Underflowing from 0 to 248 (255 - 7)
  CHECK((vec3b1 * 2) == Raz::Vec3b(62, 16, 24));
  CHECK((vec3b2 / 3) == Raz::Vec3b(16, 85, 0));

  CHECK((vec3i1 + 72) == Raz::Vec3i(24, 72, 18814));
  CHECK((vec3i2 - 20) == Raz::Vec3i(720, -1078, -2));
  CHECK((vec3i1 * 5) == Raz::Vec3i(-240, 0, 93710));
  CHECK((vec3i2 / 10) == Raz::Vec3i(74, -105, 1));

  // Floating-point vectors

  CHECK((vec3f1 + 3.5f) == Raz::Vec3f(6.68f, 45.5f, 4.374f));
  CHECK((vec3f2 - 74.42f) == Raz::Vec3f(466.99f, -27.17f, -68.099f));
  CHECK((vec3f1 + 8.2f) == (8.2f + vec3f1));

  CHECK((vec3f1 * 3.f) == Raz::Vec3f(9.54f, 126.f, 2.622f));
  CHECK((vec3f1 * 4.152f) == Raz::Vec3f(13.20336f, 174.384f, 3.628848f));
  CHECK((vec3f1 * 6.31f) == (6.31f * vec3f1));

  CHECK((vec4f1 * 7.5f) == Raz::Vec4f(633.525f, 15.f, 0.0075f, 6353.4f));
  CHECK((vec4f1 * 8.0002f) == Raz::Vec4f(675.776894f, 16.0004f, 0.0080002f, 6777.129424f));
  CHECK((vec4f1 * 0.f) == Raz::Vec4f(0.f, 0.f, 0.f, 0.f));

  CHECK((vec3f1 / 2.f) == Raz::Vec3f(1.59f, 21.f, 0.437f));
  CHECK((vec4f2 / 7.32f) == Raz::Vec4f(1.777322404f, 0.0204918f, 11.58469945f, 9.83606557f));

  // If IEEE 754 is supported, check that division by 0 behaves as expected

  if constexpr (std::numeric_limits<float>::is_iec559) {
    const Raz::Vec3f vecInf = vec3f1 / 0.f;
    CHECK(vecInf[0] == std::numeric_limits<float>::infinity());
    CHECK(vecInf[1] == std::numeric_limits<float>::infinity());
    CHECK(vecInf[2] == std::numeric_limits<float>::infinity());

    const Raz::Vec3f vecNaN = Raz::Vec3f(0.f) / 0.f;
    CHECK(std::isnan(vecNaN[0]));
    CHECK(std::isnan(vecNaN[1]));
    CHECK(std::isnan(vecNaN[2]));
  }

  if constexpr (std::numeric_limits<double>::is_iec559) {
    const Raz::Vec3d vecInf = vec3d1 / 0.0;
    CHECK(vecInf[0] == -std::numeric_limits<double>::infinity());
    CHECK(vecInf[1] == std::numeric_limits<double>::infinity());
    CHECK(vecInf[2] == -std::numeric_limits<double>::infinity());

    const Raz::Vec3d vecNaN = Raz::Vec3d(0.0) / 0.0;
    CHECK(std::isnan(vecNaN[0]));
    CHECK(std::isnan(vecNaN[1]));
    CHECK(std::isnan(vecNaN[2]));
  }
}

TEST_CASE("Vector/vector operations", "[math]") {
  CHECK((vec3b1 + vec3b2) == Raz::Vec3b(79, 7, 12)); // Values are overflowed
  CHECK((vec3b1 - vec3b2) == Raz::Vec3b(239, 9, 12)); // Values are underflowed
  CHECK((vec3b1 * vec3b2) == Raz::Vec3b(208, 248, 0)); // Values are overflowed
  CHECK((vec3b1 / (vec3b2 + Raz::Vec3b(0, 0, 1))) == Raz::Vec3b(0, 0, 12)); // Adding 1 to Z to avoid dividing by 0

  CHECK(-vec3i1 == Raz::Vec3i(48, 0, -18742));
  CHECK((vec3i1 + vec3i2) == Raz::Vec3i(692, -1058, 18760));
  CHECK((vec3i1 - vec3i2) == Raz::Vec3i(-788, 1058, 18724));
  CHECK((vec3i1 * vec3i2) == Raz::Vec3i(-35520, 0, 337356));
  CHECK((vec3i1 / vec3i2) == Raz::Vec3i(0, 0, 1041));

  CHECK((-vec3f1).strictlyEquals(Raz::Vec3f(-3.18f, -42.f, -0.874f)));
  CHECK_THAT(vec3f1 + vec3f1, IsNearlyEqualToVector(vec3f1 * 2));
  CHECK_THAT(vec3f1 - vec3f1, IsNearlyEqualToVector(Raz::Vec3f(0.f)));
  CHECK_THAT(vec3f1 * vec3f2, IsNearlyEqualToVector(Raz::Vec3f(1721.6838f, 1984.5f, 5.524554f)));
  CHECK_THAT(vec3f1 / vec3f2, IsNearlyEqualToVector(Raz::Vec3f(0.00587355f, 0.888889f, 0.1382692f)));

  CHECK_THAT(vec4f1 * vec4f2, IsNearlyEqualToVector(Raz::Vec4f(1098.9547f, 0.3, 0.0848, 60992.64)));
  CHECK_THAT(vec4f1 / vec4f2, IsNearlyEqualToVector(Raz::Vec4f(6.4926977, 13.333333, 0.0000117, 11.765555)));

  CHECK((-vec3d1).strictlyEquals(Raz::Vec3d(18.1, -4752.001, 842.0)));
  CHECK_THAT(vec3d1 + vec3d1, IsNearlyEqualToVector(vec3d1 * 2));
  CHECK_THAT(vec3d1 - vec3d1, IsNearlyEqualToVector(Raz::Vec3d(0.0)));
  CHECK_THAT(vec3d1 * vec3d2, IsNearlyEqualToVector(Raz::Vec3d(-235.481, -0.0000475200099999, -295563.44198889)));
  CHECK_THAT(vec3d1 / vec3d2, IsNearlyEqualToVector(Raz::Vec3d(-1.391237509607994, -475200100000.0, -2.3986863707814358)));
}

TEST_CASE("Vector/matrix operations", "[math]") {
  const Raz::Mat3f mat3( 4.12f,  25.1f,  30.7842f,
                         3.04f,  5.f,   -64.5f,
                        -1.f,   -7.54f,  8.41f);
  CHECK((vec3f1 * mat3) == Raz::Vec3f(139.9076f, 283.22804f, -2603.755904f));
  CHECK((vec3f2 * mat3) == Raz::Vec3f(2367.9282f, 13'777.980'66f, 13'672.408'332f));

  const Raz::Mat4f mat4(-3.2f,    53.032f,  832.451f,  74.2f,
                         10.01f,  3.15f,   -91.41f,    187.46f,
                        -6.f,    -7.78f,    90.f,      38.f,
                         123.f,  -74.8f,    147.0001f, 748.6f);
  CHECK((vec4f1 * mat4) == Raz::Vec4f(103'945.47f, -58878.67074f, 194'661.130'682f, 640'796.664f));
  CHECK((vec4f2 * mat4) == Raz::Vec4f(8307.0695f, -5354.92518f, 29032.48321f, 58115.061f));

  CHECK((vec3f1 * Raz::Mat3f::identity()) == vec3f1);
  CHECK((vec4f1 * Raz::Mat4f::identity()) == vec4f1);
}

TEST_CASE("Vector reflection", "[math]") {
  // IncVec  N  Reflection
  //      \  ^  ^
  //       \ | /
  // _______v|/_______

  // The reflected vector cannot be computed for unsigned types

  CHECK(Raz::Vec3f(1.f, -1.f, 0.f).reflect(Raz::Vec3f(0.f, 1.f, 0.f)) == Raz::Vec3f(1.f, 1.f, 0.f));
  CHECK_THAT(vec3f1.reflect(Raz::Vec3f(0.f, 1.f, 0.f)), IsNearlyEqualToVector(Raz::Vec3f(3.18f, -42.f, 0.874f)));
  CHECK_THAT(vec3f1.reflect(vec3f2), IsNearlyEqualToVector(Raz::Vec3f(-4019108.85987828f, -350714.439453f, -46922.543011268f)));

  CHECK(Raz::Vec3d(0.0, 1.0, 0.0).reflect(Raz::Vec3d(-1.0, -1.0, 0.0)) == Raz::Vec3d(-2.0, -1.0, 0.0));
  CHECK_THAT(vec3d1.reflect(Raz::Vec3d(0.0, 1.0, 0.0)), IsNearlyEqualToVector(Raz::Vec3d(-18.1, -4752.001, -842.0)));
  CHECK_THAT(vec3d1.reflect(vec3d2), IsNearlyEqualToVector(Raz::Vec3d(7696669.877407388, 4751.995084021539, 207665067.33313086)));
}

TEST_CASE("Vector length", "[math]") {
  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3b>().computeSquaredLength()), uint64_t>);
  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3b>().computeLength()), float>);
  CHECK(vec3b1.computeSquaredLength() == 1169);
  CHECK(vec3b1.computeLength() == 34.190643311f);

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3i>().computeSquaredLength()), uint64_t>);
  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3i>().computeLength()), float>);
  CHECK(vec3i1.computeSquaredLength() == 351264868);
  CHECK(vec3i1.computeLength() == 18742.060546875f);

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3f>().computeSquaredLength()), float>);
  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3f>().computeLength()), float>);
  CHECK(vec3f1.computeSquaredLength() == 1774.876342773f);
  CHECK(vec3f1.computeLength() == 42.12928009f);

  CHECK_THAT(vec4f1.computeSquaredLength(), IsNearlyEqualTo(724751.5f));
  CHECK_THAT(vec4f1.computeLength(), IsNearlyEqualTo(851.323364258f));

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3d>().computeSquaredLength()), double>);
  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3d>().computeLength()), double>);
  CHECK(vec3d1.computeSquaredLength() == 23290805.114001002);
  CHECK(vec3d1.computeLength() == 4826.054818793607);
}

TEST_CASE("Vector normalization", "[math]") {
  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3b>().normalize()), Raz::Vec3f>);
  CHECK_THAT(vec3b1.normalize(), IsNearlyEqualToVector(Raz::Vec3f(0.9066808f, 0.2339821f, 0.3509732f)));

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3i>().normalize()), Raz::Vec3f>);
  CHECK_THAT(vec3i1.normalize(), IsNearlyEqualToVector(Raz::Vec3f(-0.00256108f, 0.f, 0.9999967f)));

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3f>().normalize()), Raz::Vec3f>);
  CHECK_THAT(vec3f1.normalize(), IsNearlyEqualToVector(Raz::Vec3f(0.0754819f, 0.9969313f, 0.0207457f)));
  CHECK_THAT(vec4f1.normalize(), IsNearlyEqualToVector(Raz::Vec4f(0.0992219f, 0.0023492f, 0.0000011f, 0.9950625f)));

  CHECK_THAT(vec4f1.normalize().computeSquaredLength(), IsNearlyEqualTo(1.f));
  CHECK_THAT(vec3f1.normalize().computeLength(), IsNearlyEqualTo(1.f));

  CHECK(std::is_same_v<decltype(std::declval<Raz::Vec3d>().normalize()), Raz::Vec3d>);
  CHECK_THAT(vec3d1.normalize(), IsNearlyEqualToVector(Raz::Vec3d(-0.0037504754254997, 0.9846554128425506, -0.174469630291203)));

  // Normalizing a 0 vector returns it as is
  CHECK(Raz::Vec3b(0).normalize() == Raz::Vec3f(0.f));
  CHECK(Raz::Vec3i(0).normalize() == Raz::Vec3f(0.f));
  CHECK(Raz::Vec3f(0.f).normalize() == Raz::Vec3f(0.f));
  CHECK(Raz::Vec3d(0.0).normalize() == Raz::Vec3d(0.0));
}

TEST_CASE("Vector interpolation", "[math]") {
  CHECK(vec3b1.lerp(vec3b2, 0.f) == vec3b1);
  CHECK(vec3b1.lerp(vec3b2, 0.25f) == Raz::Vec3b(35, 69, 9)); // Results are truncated, not rounded
  CHECK(vec3b1.lerp(vec3b2, 0.5f) == Raz::Vec3b(39, 131, 6));
  CHECK(vec3b1.lerp(vec3b2, 0.75f) == Raz::Vec3b(43, 193, 3));
  CHECK(vec3b1.lerp(vec3b2, 1.f) == vec3b2);

  // The resulting interpolated vector's type can be manually chosen
  CHECK(vec3b1.lerp<float>(vec3b2, 0.f) == Raz::Vec3f(vec3b1));
  CHECK(vec3b1.lerp<float>(vec3b2, 0.25f) == Raz::Vec3f(35.25f, 69.75f, 9.f));
  CHECK(vec3b1.lerp<float>(vec3b2, 0.5f) == Raz::Vec3f(39.5f, 131.5f, 6.f));
  CHECK(vec3b1.lerp<float>(vec3b2, 0.75f) == Raz::Vec3f(43.75f, 193.25f, 3.f));
  CHECK(vec3b1.lerp<float>(vec3b2, 1.f) == Raz::Vec3f(vec3b2));

  // Computing the lerp as-is then normalizing the result truncates the values, as lerp()'s returned vector is of the original type by default
  CHECK_THAT(vec3b1.lerp(vec3b2, 0.5f).normalize(), IsNearlyEqualToVector(Raz::Vec3f(0.285059f, 0.9575061f, 0.0438552f)));
  // Computing the normalized linear interpolation does the lerp with the same result type as it uses itself; no truncation happens
  CHECK_THAT(vec3b1.nlerp(vec3b2, 0.5f), IsNearlyEqualToVector(Raz::Vec3f(0.2874076f, 0.956813f, 0.0436569f)));

  CHECK(vec3i1.lerp(vec3i2, 0.f) == vec3i1);
  CHECK(vec3i1.lerp(vec3i2, 0.25f) == Raz::Vec3i(149, -264, 14061));
  CHECK(vec3i1.lerp(vec3i2, 0.5f) == Raz::Vec3i(346, -529, 9380));
  CHECK(vec3i1.lerp(vec3i2, 0.75f) == Raz::Vec3i(543, -793, 4699));
  CHECK(vec3i1.lerp(vec3i2, 1.f) == vec3i2);
  CHECK(vec3i1.nlerp(vec3i2, 0.5f) == Raz::Vec3f(0.0368035f, -0.056269f, 0.997737f));

  // lerp() doesn't normalize the resulting vector
  CHECK(vec3f1.lerp(vec3f2, 0.f) == vec3f1);
  CHECK(vec3f1.lerp(vec3f2, 0.25f) == Raz::Vec3f(137.73749f, 43.3125f, 2.23575f)); // (vec3f1 * 3 + vec3f2) / 4
  CHECK(vec3f1.lerp(vec3f2, 0.5f) == (vec3f1 + vec3f2) / 2);
  CHECK(vec3f1.lerp(vec3f2, 0.75f) == Raz::Vec3f(406.85248f, 45.9375f, 4.95925f)); // (vec3f1 + vec3f2 * 3) / 4
  CHECK(vec3f1.lerp(vec3f2, 1.f) == vec3f2);

  // nlerp() does normalize the resulting vector; it is strictly equal to vec.lerp(...).normalize()
  CHECK(vec4f1.nlerp(vec4f2, 0.f) == vec4f1.normalize());
  CHECK(vec4f1.nlerp(vec4f2, 0.25f) == Raz::Vec4f(0.10136666f, 0.00233993f, 0.0322656f, 0.994323f)); // (vec4f1 * 3 + vec4f2).normalize()
  CHECK(vec4f1.nlerp(vec4f2, 0.5f) == (vec4f1 + vec4f2).normalize());
  CHECK(vec4f1.nlerp(vec4f2, 0.75f) == Raz::Vec4f(0.11226334f, 0.00222709f, 0.23125429f, 0.966392f)); // (vec4f1 + vec4f2 * 3).normalize()
  CHECK(vec4f1.nlerp(vec4f2, 1.f) == vec4f2.normalize());

  CHECK_THAT(vec3d1.lerp(vec3d2, 0.0), IsNearlyEqualToVector(vec3d1));
  CHECK_THAT(vec3d1.lerp(vec3d2, 0.25), IsNearlyEqualToVector(Raz::Vec3d(-10.3225, 3564.0007499974999, -543.74363361375)));
  CHECK_THAT(vec3d1.lerp(vec3d2, 0.5), IsNearlyEqualToVector((vec3d1 + vec3d2) / 2, 0.000000000000001));
  CHECK_THAT(vec3d1.lerp(vec3d2, 0.75), IsNearlyEqualToVector(Raz::Vec3d(5.232499999999998, 1188.0002499925003, 52.76909915874989)));
  CHECK_THAT(vec3d1.lerp(vec3d2, 1.0), IsNearlyEqualToVector(vec3d2, 0.000000000001));
}

TEST_CASE("Vector structured bindings", "[math]") {
  static_assert(std::tuple_size_v<decltype(vec3b1)> == 3);
  static_assert(std::tuple_size_v<decltype(vec4f1)> == 4);
  static_assert(std::tuple_size_v<decltype(Raz::Vector<bool, 1>())> == 1);

  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(vec3i1)>, const int>);
  static_assert(std::is_same_v<std::tuple_element_t<2, decltype(vec3d1)>, const double>);
  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(Raz::Vector<bool, 1>())>, bool>);
  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(Raz::Vector<const bool, 1>())>, const bool>);
  static_assert(std::is_same_v<std::tuple_element_t<0, std::add_const_t<decltype(Raz::Vector<bool, 1>())>>, const bool>);

  // When using structured bindings, Raz::get<I>(e) is exclusively found using ADL (https://en.cppreference.com/w/cpp/language/adl)
  static_assert(Raz::get<0>(vec3f1) == vec3f1[0]);
  static_assert(Raz::get<1>(vec3f1) == vec3f1[1]);
  static_assert(Raz::get<2>(vec3f1) == vec3f1[2]);

  {
    const auto [x, y, z] = vec3b2;
    static_assert(std::is_same_v<decltype(x), const uint8_t>);
    static_assert(std::is_same_v<decltype(y), const uint8_t>);
    static_assert(std::is_same_v<decltype(z), const uint8_t>);

    CHECK(x == vec3b2.x());
    CHECK(y == vec3b2.y());
    CHECK(z == vec3b2.z());

    // Elements are new variables
    CHECK_FALSE(&x == &vec3b2.x());
    CHECK_FALSE(&y == &vec3b2.y());
    CHECK_FALSE(&z == &vec3b2.z());
  }

  {
    const auto& [x, y, z] = vec3d2;
    // decltype-ing a structured binding doesn't show a reference...
    static_assert(std::is_same_v<decltype(x), const double>);
    static_assert(std::is_same_v<decltype(y), const double>);
    static_assert(std::is_same_v<decltype(z), const double>);
    // ... but getting the elements independently does...
    static_assert(std::is_same_v<decltype(Raz::get<0>(vec3d2)), const double&>);
    static_assert(std::is_same_v<decltype(Raz::get<1>(vec3d2)), const double&>);
    static_assert(std::is_same_v<decltype(Raz::get<2>(vec3d2)), const double&>);

    CHECK(x == vec3d2.x());
    CHECK(y == vec3d2.y());
    CHECK(z == vec3d2.z());

    // ... and the elements are linked as expected
    CHECK(&x == &vec3d2.x());
    CHECK(&y == &vec3d2.y());
    CHECK(&z == &vec3d2.z());
  }

  {
    Raz::Vector<bool, 1> boolVec(38);

    {
      const auto [x] = boolVec;
      static_assert(std::is_same_v<decltype(x), const bool>);
      CHECK(x == boolVec.x());
      CHECK_FALSE(&x == &boolVec.x());
    }

    {
      auto [x] = boolVec;
      static_assert(std::is_same_v<decltype(x), bool>);
      CHECK(x == boolVec.x());
      CHECK_FALSE(&x == &boolVec.x());
    }

    {
      const auto& [x] = boolVec;
      static_assert(std::is_same_v<decltype(x), const bool>);
      static_assert(std::is_same_v<decltype(Raz::get<0>(std::as_const(boolVec))), const bool&>);
      CHECK(x == boolVec.x());
      CHECK(&x == &boolVec.x());
    }

    {
      auto& [x] = boolVec;
      static_assert(std::is_same_v<decltype(x), bool>);
      static_assert(std::is_same_v<decltype(Raz::get<0>(boolVec)), bool&>);
      CHECK(x == boolVec.x());
      CHECK(&x == &boolVec.x());
    }

    {
      auto&& [x] = std::move(boolVec);
      static_assert(std::is_same_v<decltype(x), bool>);
      static_assert(std::is_same_v<decltype(Raz::get<0>(std::move(boolVec))), bool&&>); // xvalue
      static_assert(std::is_same_v<decltype(Raz::get<0>(Raz::Vector<bool, 1>())), bool&&>); // prvalue
      CHECK(x == boolVec.x());
      CHECK(&x == &boolVec.x());
    }
  }
}

TEST_CASE("Vector hash", "[math]") {
  CHECK(vec3b1.hash() == vec3b1.hash());
  CHECK_FALSE(vec3b1.hash() == vec3b2.hash());

  CHECK(vec3i1.hash() == vec3i1.hash());
  CHECK_FALSE(vec3i1.hash() == vec3i2.hash());

  CHECK(vec3f1.hash() == vec3f1.hash());
  CHECK_FALSE(vec3f1.hash() == vec3f2.hash());

  CHECK(vec4f1.hash() == vec4f1.hash());
  CHECK_FALSE(vec4f1.hash() == vec4f2.hash());

  CHECK(vec3d1.hash() == vec3d1.hash());
  CHECK_FALSE(vec3d1.hash() == vec3d2.hash());

  constexpr Raz::Vec3f vec3f1Swizzled(vec3f1[2], vec3f1[0], vec3f1[1]);
  CHECK_FALSE(vec3f1.hash() == vec3f1Swizzled.hash());

  constexpr Raz::Vec3f vec3f1Epsilon = vec3f1 + std::numeric_limits<float>::epsilon();
  CHECK_FALSE(vec3f1.hash() == vec3f1Epsilon.hash());

  // Checking that it behaves as expected when used in a hashmap
  std::unordered_map<Raz::Vec3f, int> map;
  map.try_emplace(vec3f1, 1);
  map.try_emplace(vec3f1Swizzled, 2);
  map.try_emplace(vec3f1Epsilon, 3);
  map.try_emplace(vec3f2, 4);

  CHECK(map.size() == 4);

  CHECK(map.find(vec3f1)->second == 1);
  CHECK(map.find(vec3f1Swizzled)->second == 2);
  CHECK(map.find(vec3f1Epsilon)->second == 3);
  CHECK(map.find(vec3f2)->second == 4);

  map.erase(vec3f1Epsilon);
  CHECK(map.find(vec3f1Epsilon) == map.cend());
}

TEST_CASE("Vector near-equality", "[math]") {
  // Near-equality applies only to vectors of floating-point values

  CHECK_FALSE(vec3f1 == vec3f2);

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

TEST_CASE("Vector strict equality", "[math]") {
  CHECK(vec3b1.strictlyEquals(vec3b1));
  CHECK_FALSE(vec3b1.strictlyEquals(vec3b1 + Raz::Vec3b(1, 0, 0)));

  CHECK(vec3i1.strictlyEquals(vec3i1));
  CHECK_FALSE(vec3i1.strictlyEquals(vec3i1 + Raz::Vec3i(1, 0, 0)));

  CHECK(vec3f1.strictlyEquals(vec3f1));
  CHECK(vec4f1.strictlyEquals(vec4f1));

  CHECK(std::equal_to<Raz::Vec3f>()(vec3f1, vec3f1));
  CHECK(std::equal_to<Raz::Vec4f>()(vec4f1, vec4f1));

  constexpr Raz::Vec3f vec3f1Swizzled(vec3f1[2], vec3f1[0], vec3f1[1]);
  CHECK_FALSE(vec3f1.strictlyEquals(vec3f1Swizzled));
  CHECK_FALSE(std::equal_to<Raz::Vec3f>()(vec3f1, vec3f1Swizzled));

  constexpr Raz::Vec3f vec3f1Epsilon = vec3f1 + std::numeric_limits<float>::epsilon();
  CHECK(vec3f1 == vec3f1Epsilon); // Near-equality check
  CHECK_FALSE(vec3f1.strictlyEquals(vec3f1Epsilon)); // Strict-equality checks
  CHECK_FALSE(std::equal_to<Raz::Vec3f>()(vec3f1, vec3f1Epsilon));

  constexpr Raz::Vec3d vec3d2Epsilon = vec3d2 + std::numeric_limits<double>::epsilon();
  CHECK(vec3d2 == vec3d2Epsilon); // Near-equality check
  CHECK_FALSE(vec3d2.strictlyEquals(vec3d2Epsilon)); // Strict-equality checks
  CHECK_FALSE(std::equal_to<Raz::Vec3d>()(vec3d2, vec3d2Epsilon));

  constexpr std::array<Raz::Vec3f, 3> vectors = { vec3f1Swizzled, vec3f1Epsilon, vec3f1 };

  // When using a simple find(), which uses operator==, vec3f1Epsilon is found instead of vec3f1
  const auto foundIter = std::find(vectors.cbegin(), vectors.cend(), vec3f1);
  CHECK(std::distance(vectors.cbegin(), foundIter) == 1);

  // To search for a specific element, find_if() must be used with a strict equality check
  const auto foundIfIter = std::find_if(vectors.cbegin(), vectors.cend(), [&] (const Raz::Vec3f& compVec) { return vec3f1.strictlyEquals(compVec); });
  CHECK(std::distance(vectors.cbegin(), foundIfIter) == 2);

  constexpr std::array<Raz::Vec3f, 3> swappedVectors = { vec3f1Swizzled, vec3f1, vec3f1Epsilon };

  // Trying to compare for equality fails with a simple std::equal on the vectors
  CHECK(std::equal(vectors.cbegin(), vectors.cend(), swappedVectors.cbegin()));

  // When using the std::equal_to specialization, vectors are properly found to be unequal to each other
  CHECK_FALSE(std::equal(vectors.cbegin(), vectors.cend(), swappedVectors.cbegin(), std::equal_to<Raz::Vec3f>()));
}

TEST_CASE("Vector less-than", "[math]") {
  CHECK(std::less<Raz::Vec3b>()(vec3b1, vec3b2));
  CHECK_FALSE(std::less<Raz::Vec3b>()(vec3b2, vec3b1));

  CHECK(std::less<Raz::Vec3i>()(vec3i1, vec3i2));
  CHECK_FALSE(std::less<Raz::Vec3i>()(vec3i2, vec3i1));

  CHECK(std::less<Raz::Vec3f>()(vec3f1, vec3f2));
  CHECK_FALSE(std::less<Raz::Vec3f>()(vec3f2, vec3f1));
  CHECK_FALSE(std::less<Raz::Vec3f>()(vec3f1, vec3f1)); // Equal vectors are not strictly less than the other
  CHECK(std::less<Raz::Vec3f>()(vec3f1, vec3f1 + std::numeric_limits<float>::epsilon()));
  CHECK_FALSE(std::less<Raz::Vec3f>()(vec3f1 + std::numeric_limits<float>::epsilon(), vec3f1)); // Performs a strict check, no tolerance allowed

  CHECK_FALSE(std::less<Raz::Vec4f>()(vec4f1, vec4f2));
  CHECK(std::less<Raz::Vec4f>()(vec4f2, vec4f1));
  CHECK_FALSE(std::less<Raz::Vec4f>()(vec4f1, vec4f1));
  CHECK(std::less<Raz::Vec4f>()(vec4f1, vec4f1 + std::numeric_limits<float>::epsilon()));
  CHECK_FALSE(std::less<Raz::Vec4f>()(vec4f1 + std::numeric_limits<float>::epsilon(), vec4f1));

  CHECK(std::less<Raz::Vec3d>()(vec3d1, vec3d2));
  CHECK_FALSE(std::less<Raz::Vec3d>()(vec3d2, vec3d1));
  CHECK_FALSE(std::less<Raz::Vec3d>()(vec3d2, vec3d2));
  CHECK(std::less<Raz::Vec3d>()(vec3d2, vec3d2 + std::numeric_limits<double>::epsilon()));
  CHECK_FALSE(std::less<Raz::Vec3d>()(vec3d2 + std::numeric_limits<double>::epsilon(), vec3d2));
}

TEST_CASE("Vector conversion", "[math]") {
  CHECK(Raz::Vec3f(vec3b1) == Raz::Vec3f(31.f, 8.f, 12.f));
  CHECK(Raz::Vec3b(vec3i1) == Raz::Vec3b(208, 0, 54)); // Values are underflowed & overflowed
  CHECK(Raz::Vec3i(vec3f1) == Raz::Vec3i(3, 42, 0)); // Results are truncated, not rounded
  CHECK(Raz::Vec4u(vec4f1) == Raz::Vec4u(84, 2, 0, 847));
  CHECK(Raz::Vec4d(vec4f2) == Raz::Vec4d(13.010000228881835, 0.1500000059604644, 84.8000030517578, 72.0));
  CHECK(Raz::Vec4f(vec4f2).strictlyEquals(vec4f2));
  CHECK(Raz::Vec3i(vec3d1) == Raz::Vec3i(-18, 4752, -842));
}

TEST_CASE("Vector printing", "[math]") {
  std::stringstream stream;

  stream << vec3b1;
  CHECK(stream.str() == "[ 31, 8, 12 ]");

  stream.str(std::string()); // Resetting the stream
  stream << vec3i2;
  CHECK(stream.str() == "[ 740, -1058, 18 ]");

  stream.str(std::string());
  stream << vec3f1;
  CHECK(stream.str() == "[ 3.18, 42, 0.874 ]");

  stream.str(std::string());
  stream << vec3f2;
  CHECK(stream.str() == "[ 541.41, 47.25, 6.321 ]");

  stream.str(std::string());
  stream << vec4f1;
  CHECK(stream.str() == "[ 84.47, 2, 0.001, 847.12 ]");

  stream.str(std::string());
  stream << vec3d2;
  CHECK(stream.str() == "[ 13.01, -1e-08, 351.025 ]");
}
