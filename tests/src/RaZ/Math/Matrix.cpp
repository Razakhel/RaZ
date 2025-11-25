#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

#include <unordered_map>

namespace {

// Declaring matrices to be tested
constexpr Raz::Mat3f mat31( 4.12f,  25.1f,  30.7842f,
                            3.04f,  5.f,   -64.5f,
                           -1.f,   -7.54f,  8.41f);
constexpr Raz::Mat3f mat32(47.4f,   10.001f,  15.12f,
                           8.01f,  -98.1f,    97.f,
                           12.54f,  70.f,    -54.05f);

constexpr Raz::Mat4f mat41(-3.2f,    53.032f,  832.451f,  74.2f,
                            10.01f,  3.15f,   -91.41f,    187.46f,
                           -6.f,    -7.78f,    90.f,      38.f,
                            123.f,  -74.8f,    147.0001f, 748.6f);
constexpr Raz::Mat4f mat42( 5.5f,    98.14f, -8.24f,  42.f,
                            15.84f, -145.f,   3.145f, 52.74f,
                           -8.12f,   38.24f,  62.f,   43.12f,
                            74.f,    15.7f,   43.64f, 28.8f);

} // namespace

TEST_CASE("Matrix resize", "[math]") {
  constexpr Raz::Mat3f truncatedMat(mat41);
  constexpr Raz::Mat4f expandedMat(truncatedMat);

  CHECK(truncatedMat == Raz::Mat3f(-3.2f,    53.032f,  832.451f,
                                    10.01f,  3.15f,   -91.41f,
                                   -6.f,    -7.78f,    90.f));
  CHECK(expandedMat == Raz::Mat4f(-3.2f,    53.032f,  832.451f, 0.f,
                                   10.01f,  3.15f,   -91.41f,   0.f,
                                  -6.f,    -7.78f,    90.f,     0.f,
                                   0.f,     0.f,      0.f,      1.f));

  CHECK(Raz::Mat4f(mat31) == Raz::Mat4f( 4.12f,  25.1f,  30.7842f, 0.f,
                                         3.04f,  5.f,   -64.5f,    0.f,
                                        -1.f,   -7.54f,  8.41f,    0.f,
                                         0.f,    0.f,    0.f,      1.f));

  CHECK(Raz::Mat4f(Raz::Mat3f(mat42)) == Raz::Mat4f( 5.5f,    98.14f, -8.24f,  0.f,
                                                     15.84f, -145.f,   3.145f, 0.f,
                                                    -8.12f,   38.24f,  62.f,   0.f,
                                                     0.f,     0.f,     0.f,    1.f));
}

TEST_CASE("Matrix from vectors", "[math]") {
  constexpr auto testMatRows = Raz::Matrix<float, 3, 4>::fromRows(Raz::Vec3f( 1.f,  2.f,  3.f),
                                                                  Raz::Vec3f( 4.f,  5.f,  6.f),
                                                                  Raz::Vec3f( 7.f,  8.f,  9.f),
                                                                  Raz::Vec3f(10.f, 11.f, 12.f));
  constexpr auto testMatColumns = Raz::Matrix<float, 3, 4>::fromColumns(Raz::Vec4f(1.f, 4.f, 7.f, 10.f),
                                                                        Raz::Vec4f(2.f, 5.f, 8.f, 11.f),
                                                                        Raz::Vec4f(3.f, 6.f, 9.f, 12.f));
  constexpr Raz::Matrix<float, 3, 4> testRes( 1.f,  2.f,  3.f,
                                              4.f,  5.f,  6.f,
                                              7.f,  8.f,  9.f,
                                             10.f, 11.f, 12.f);
  CHECK(testMatRows == testRes);
  CHECK(testMatColumns == testRes);

  constexpr auto testMat31 = Raz::Mat3f::fromRows(mat31.recoverRow(0), mat31.recoverRow(1), mat31.recoverRow(2));
  CHECK(testMat31 == mat31);

  constexpr auto testMat41 = Raz::Mat4f::fromColumns(mat41.recoverColumn(0), mat41.recoverColumn(1), mat41.recoverColumn(2), mat41.recoverColumn(3));
  CHECK(testMat41 == mat41);
}

TEST_CASE("Matrix elements fetching", "[math]") {
  {
    constexpr Raz::Mat3i testMat(0, 3, 6,
                                 1, 4, 7,
                                 2, 5, 8);

    CHECK(testMat[0] == 0);
    CHECK(testMat[1] == 1);
    CHECK(testMat[2] == 2);
    CHECK(testMat[3] == 3);
    CHECK(testMat[4] == 4);
    CHECK(testMat[5] == 5);
    CHECK(testMat[6] == 6);
    CHECK(testMat[7] == 7);
    CHECK(testMat[8] == 8);
  }

  {
    constexpr Raz::Matrix<int, 3, 2> testMat(0, 1, 2,
                                             3, 4, 5);

    CHECK(testMat[0] == 0);
    CHECK(testMat[1] == 3);
    CHECK(testMat[2] == 1);
    CHECK(testMat[3] == 4);
    CHECK(testMat[4] == 2);
    CHECK(testMat[5] == 5);
  }

  {
    constexpr Raz::Matrix<int, 3, 2> testMat(0, 2, 4,
                                             1, 3, 5);

    CHECK(testMat[0] == 0);
    CHECK(testMat[1] == 1);
    CHECK(testMat[2] == 2);
    CHECK(testMat[3] == 3);
    CHECK(testMat[4] == 4);
    CHECK(testMat[5] == 5);
  }

  {
    constexpr Raz::Matrix<int, 2, 3> testMat(0, 3,
                                             1, 4,
                                             2, 5);

    CHECK(testMat[0] == 0);
    CHECK(testMat[1] == 1);
    CHECK(testMat[2] == 2);
    CHECK(testMat[3] == 3);
    CHECK(testMat[4] == 4);
    CHECK(testMat[5] == 5);
  }

  //                   height:
  //      [ 0, 3, 6 ] <- 0
  //      [ 1, 4, 7 ] <- 1
  //      [ 2, 5, 8 ] <- 2
  //        ^  ^  ^
  // width: 0  1  2
  CHECK(mat31.getElement(1, 1) == mat31[4]);
  CHECK(mat31.getElement(2, 1) == mat31[7]);
  CHECK(mat31.getElement(2, 2) == mat31[8]);

  //                         height:
  //      [ 0, 4,  8, 12 ] <- 0
  //      [ 1, 5,  9, 13 ] <- 1
  //      [ 2, 6, 10, 14 ] <- 2
  //      [ 3, 7, 11, 15 ] <- 3
  //        ^  ^   ^   ^
  // width: 0  1   2   3
  CHECK(mat41.getElement(0, 0) == mat41[0]);
  CHECK(mat41.getElement(1, 2) == mat41[6]);
  CHECK(mat41.getElement(3, 3) == mat41[15]);

  CHECK(mat31.recoverRow(0) == Raz::Vec3f(4.12f,  25.1f, 30.7842f));
  CHECK(mat42.recoverRow(2) == Raz::Vec4f(-8.12f, 38.24f, 62.f, 43.12f));

  CHECK(mat32.recoverColumn(2) == Raz::Vec3f(15.12f, 97.f, -54.05f));
  CHECK(mat41.recoverColumn(1) == Raz::Vec4f(53.032f, 3.15f, -7.78f, -74.8f));

  constexpr Raz::Matrix<float, 8, 2> testMat(1.f, 2.f,  3.f,  4.f,  5.f,  6.f,  7.f,  8.f,
                                             9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f);
  CHECK(testMat[0] == 1.f);
  CHECK(testMat[1] == 9.f);
  CHECK(testMat[3] == 10.f);
  CHECK(testMat[7] == 12.f);
  CHECK(testMat[8] == 5.f);

  CHECK(testMat.getElement(0, 0) == 1.f);
  CHECK(testMat.getElement(0, 1) == 9.f);
  CHECK(testMat.getElement(1, 1) == 10.f);
  CHECK(testMat.getElement(3, 1) == 12.f);
  CHECK(testMat.getElement(4, 0) == 5.f);

  CHECK(testMat.recoverRow(1) == Raz::Vector<float, 8>(9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f));
  CHECK(testMat.recoverColumn(5) == Raz::Vec2f(6.f, 14.f));

  constexpr Raz::Matrix<float, 2, 8> testMatTrans = testMat.transpose();

  CHECK(testMatTrans.recoverRow(7) == Raz::Vec2f(8.f, 16.f));
  CHECK(testMatTrans.recoverColumn(0) == Raz::Vector<float, 8>(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f));
}

TEST_CASE("Matrix transposition", "[math]") {
  CHECK(mat31.transpose() == Raz::Mat3f(4.12f,     3.04f, -1.f,
                                        25.1f,     5.f,   -7.54f,
                                        30.7842f, -64.5f,  8.41f));

  CHECK(mat41.transpose() == Raz::Mat4f(-3.2f,      10.01f, -6.f,    123.f,
                                         53.032f,   3.15f,  -7.78f, -74.8f,
                                         832.451f, -91.41f,  90.f,   147.000107f,
                                         74.2f,     187.46f, 38.f,   748.6f));


  constexpr Raz::Matrix<float, 2, 3> mat2x3(1.f, 2.f,
                                            3.f, 4.f,
                                            5.f, 6.f);

  constexpr Raz::Matrix<float, 3, 2> mat3x2(1.f, 3.f, 5.f,
                                            2.f, 4.f, 6.f);

  CHECK(mat2x3.transpose() == mat3x2);
  CHECK(mat3x2.transpose() == mat2x3);
}

TEST_CASE("Matrix inversion", "[math]") {
  // Results taken from Wolfram Alpha: https://tinyurl.com/dve4wvbs
  CHECK(Raz::FloatUtils::areNearlyEqual(mat31.computeDeterminant(), -1404.90235872f));
  CHECK(mat31.inverse() == Raz::Mat3f( 0.3162355f,     0.315469503f,    1.26191771f,
                                      -0.0277126748f, -0.0465751f,     -0.255764395f,
                                       0.0127564743f, -0.00424570311f,  0.0396497f));

  // Results taken from Wolfram Alpha: https://tinyurl.com/2p8d7m24
  CHECK(Raz::FloatUtils::areNearlyEqual(mat41.computeDeterminant(), 348493952.f));
  CHECK(mat41.inverse() == Raz::Mat4f(0.00278244f,  -0.0177149f,  -0.0550912f,   0.00695677f,
                                      0.00705852f,   0.0187818f,  -0.0407667f,  -0.00333349f,
                                      0.00075337f,  -0.00172146f,  0.00197614f,  0.000256093f,
                                      0.000100176f,  0.00512538f,  0.0045904f,  -0.000190586f));

  CHECK(Raz::Mat3f::identity().computeDeterminant() == 1.f);
  CHECK(Raz::Mat3f::identity().inverse() == Raz::Mat3f::identity());
}

TEST_CASE("Matrix/scalar operations", "[math]") {
  CHECK(mat31 * 3.f == Raz::Mat3f( 12.36f,  75.3f,   92.3526f,
                                   9.12f,   15.f,   -193.5f,
                                  -3.f,    -22.62f,  25.23f));
  CHECK(mat31 * 4.152f == Raz::Mat3f( 17.10624f,  104.2152f,  127.8159984f,
                                      12.62208f,  20.76f,    -267.804f,
                                     -4.152f,    -31.30608f,  34.91832f));

  CHECK(mat41 * 7.5f == Raz::Mat4f(-24.f,     397.74f,  6243.3825f,  556.5f,
                                    75.075f,  23.625f, -685.575f,    1405.95f,
                                   -45.f,    -58.35f,   675.f,       285.f,
                                    922.5f,  -561.f,    1102.50075f, 5614.5f));
  CHECK(mat41 * 8.0002f == Raz::Mat4f(-25.60064f,   424.2666064f,  6659.7744902f,  593.61484f,
                                       80.082002f,  25.20063f,    -731.298282f,    1499.717492f,
                                      -48.0012f,   -62.241556f,    720.018f,       304.0076f,
                                       984.0246f,  -598.41496f,    1176.03020002f, 5988.94972f));
  CHECK(mat41 * 0.f == Raz::Mat4f(0.f, 0.f, 0.f, 0.f,
                                  0.f, 0.f, 0.f, 0.f,
                                  0.f, 0.f, 0.f, 0.f,
                                  0.f, 0.f, 0.f, 0.f));
}

TEST_CASE("Matrix/matrix operations", "[math]") {
  CHECK(mat31 - mat31 == Raz::Mat3f(0.f, 0.f, 0.f,
                                    0.f, 0.f, 0.f,
                                    0.f, 0.f, 0.f));

  // Component-wise multiplication
  CHECK(mat31 % mat32 == Raz::Mat3f( 195.288f,  251.0251f,  465.457104f,
                                     24.3504f, -490.5f,    -6256.5f,
                                    -12.54f,   -527.8f,    -454.5605f));

  // Matrix multiplication

  constexpr Raz::Mat3f res3132( 782.372868f, -266.21188f,   833.10839f,
                               -624.684f,    -4975.09696f,  4017.1898f,
                               -2.334007f,    1318.373f,   -1201.0605f);
  constexpr Raz::Mat3f res3231( 210.57104f,  1125.7402f,  941.26578f,
                               -362.2228f,  -1020.829f,   7389.801442f,
                                318.5148f,   1072.291f,  -4583.526632f);

  // Clang 14+ (at least between 14.0.0 & 14.0.6 at the time of writing), only in Release, has apparently a slightly different behavior
  //   and does not give the same value in the 3rd row, 1st column
#if defined(RAZ_COMPILER_CLANG) && defined(RAZ_CONFIG_RELEASE) && __clang_major__ >= 14
  CHECK_THAT(mat31 * mat32, IsNearlyEqualToMatrix(res3132, 0.000001f));
#else
  CHECK(mat31 * mat32 == res3132);
#endif
  CHECK(mat32 * mat31 == res3231);

  CHECK(mat31 * Raz::Mat3f::identity() == mat31);
  CHECK(mat41 * Raz::Mat4f::identity() == mat41);

  constexpr Raz::Matrix<float, 2, 3> mat2x3(1.f, 2.f,
                                            3.f, 4.f,
                                            5.f, 6.f);

  constexpr Raz::Matrix<float, 3, 2> mat3x2(1.f, 2.f, 3.f,
                                            4.f, 5.f, 6.f);

  // Results taken from Wolfram Alpha: https://tinyurl.com/3cx3zp94
  CHECK(mat2x3 * mat3x2 == Raz::Mat3f(9.f,  12.f, 15.f,
                                      19.f, 26.f, 33.f,
                                      29.f, 40.f, 51.f));

  // Results taken from Wolfram Alpha: https://tinyurl.com/y2chjdu2
  CHECK(mat3x2 * mat2x3 == Raz::Mat2f(22.f, 28.f,
                                      49.f, 64.f));
}

TEST_CASE("Matrix/vector multiplication", "[math]") {
  constexpr Raz::Mat3f matRotX(1.f,         0.f,          0.f,
                               0.f, 0.70710677f, -0.70710677f,
                               0.f, 0.70710677f,  0.70710677f);
  CHECK(matRotX * Raz::Axis::X == Raz::Axis::X);
  CHECK(Raz::Axis::X * matRotX == Raz::Axis::X);

  constexpr Raz::Mat3f matRotY( 0.70710677f, 0.f, 0.70710677f,
                                        0.f, 1.f,         0.f,
                               -0.70710677f, 0.f, 0.70710677f);
  CHECK(matRotY * Raz::Axis::X == Raz::Vec3f(0.70710677f, 0.f, -0.70710677f));
  CHECK(Raz::Axis::X * matRotY == Raz::Vec3f(0.70710677f, 0.f, 0.70710677f));

  constexpr Raz::Mat3f matRotZ(0.70710677f, -0.70710677f, 0.f,
                               0.70710677f,  0.70710677f, 0.f,
                                       0.f,          0.f, 1.f);
  CHECK(matRotZ * Raz::Axis::X == Raz::Vec3f(0.70710677f, 0.70710677f, 0.f));
  CHECK(Raz::Axis::X * matRotZ == Raz::Vec3f(0.70710677f, -0.70710677f, 0.f));

  // See: https://www.geogebra.org/m/je2vyv75
  constexpr Raz::Vec3f vec3(3.18f, 4.2f, 0.874f);

  CHECK(matRotX * vec3 == Raz::Vec3f(vec3.x(), 2.35183716f, 3.58785963f));
  CHECK(vec3 * matRotX == Raz::Vec3f(vec3.x(), 3.58785963f, -2.35183716f));

  CHECK(matRotY * vec3 == Raz::Vec3f(2.86661077f, vec3.y(), -1.63058829f));
  CHECK(vec3 * matRotY == Raz::Vec3f(1.63058829f, vec3.y(), 2.86661077f));

  CHECK(matRotZ * vec3 == Raz::Vec3f(-0.721249f, 5.21844769f, vec3.z()));
  CHECK(vec3 * matRotZ == Raz::Vec3f(5.21844769f, 0.721249f, vec3.z()));

  CHECK(mat31 * vec3 == Raz::Vec3f(145.427f, -25.7058f, -27.4976597f));
  CHECK(vec3 * mat31 == Raz::Vec3f(24.9956f, 94.2280426f, -165.655884f));
  CHECK(mat32 * vec3 == Raz::Vec3f(205.95108f, -301.770203f, 286.637512f));
  CHECK(vec3 * mat32 == Raz::Vec3f(195.333969f, -319.036804f, 408.241882f));

  CHECK(Raz::Mat3f::identity() * vec3 == vec3);
  CHECK(vec3 * Raz::Mat3f::identity() == vec3);

  constexpr Raz::Vec2f vec2(10.f, 0.1f);

  constexpr Raz::Matrix<float, 3, 2> mat3x2(1.f, 2.f, 3.f,
                                            4.f, 5.f, 6.f);

  constexpr Raz::Matrix<float, 2, 3> mat2x3(1.f, 2.f,
                                            3.f, 4.f,
                                            5.f, 6.f);

  // Results taken from Wolfram Alpha
  CHECK(mat3x2 * vec3 == Raz::Vec2f(14.202f, 38.964f)); // https://tinyurl.com/2m6jx2he
  CHECK(vec2 * mat3x2 == Raz::Vec3f(10.4f, 20.5f, 30.6f)); // https://tinyurl.com/43a9uufn

  CHECK(vec3 * mat2x3 == Raz::Vec2f(20.15f, 28.404f)); // https://tinyurl.com/4m9teh6a
  CHECK(mat2x3 * vec2 == Raz::Vec3f(10.2f, 30.4f, 50.6f)); // https://tinyurl.com/2p85wpk6

  constexpr Raz::Vec4f vec4(84.47f, 2.f, 0.001f, 847.12f);

  CHECK(mat41 * vec4 == Raz::Vec4f(62692.896451f, 159652.86849f, 31668.27f, 644394.3890001f));
  CHECK(vec4 * mat41 == Raz::Vec4f(103945.469f, -58878.6719f, 194661.125f, 640796.625f));

  CHECK(mat42 * vec4 == Raz::Vec4f(36239.89676f, 45725.116745f, 35918.46f, 30679.27964f));
  CHECK(vec4 * mat42 == Raz::Vec4f(63183.1367f, 21299.707f, 36278.6367f, 28050.3184f));

  CHECK(Raz::Mat4f::identity() * vec4 == vec4);
  CHECK(vec4 * Raz::Mat4f::identity() == vec4);

  constexpr Raz::Matrix<float, 4, 2> mat4x2(1.f, 2.f, 3.f, 4.f,
                                            5.f, 6.f, 7.f, 8.f);

  constexpr Raz::Matrix<float, 2, 4> mat2x4(1.f, 2.f,
                                            3.f, 4.f,
                                            5.f, 6.f,
                                            7.f, 8.f);

  // Results taken from Wolfram Alpha
  CHECK(mat4x2 * vec4 == Raz::Vec2f(3476.95288f, 7211.31689f)); // https://tinyurl.com/2xp73arw
  CHECK(vec2 * mat4x2 == Raz::Vec4f(10.5f, 20.6f, 30.7f, 40.8f)); // https://tinyurl.com/5n73nzmk

  CHECK(vec4 * mat2x4 == Raz::Vec2f(6020.31494f, 6953.90576f)); // https://tinyurl.com/26muhrx3
  CHECK(mat2x4 * vec2 == Raz::Vec4f(10.2f, 30.4f, 50.6f, 70.8f)); // https://tinyurl.com/3k5p9uyk
}

TEST_CASE("Matrix hash", "[math]") {
  CHECK(mat31.hash() == mat31.hash());
  CHECK_FALSE(mat31.hash() == mat32.hash());

  CHECK(mat41.hash() == mat41.hash());
  CHECK_FALSE(mat41.hash() == mat42.hash());

  constexpr auto mat31Swizzled = Raz::Mat3f::fromRows(mat31.recoverRow(2), mat31.recoverRow(0), mat31.recoverRow(1));
  CHECK_FALSE(mat31.hash() == mat31Swizzled.hash());

  constexpr Raz::Mat3f mat31Epsilon = mat31 + std::numeric_limits<float>::epsilon();
  CHECK_FALSE(mat31.hash() == mat31Epsilon.hash());

  // Checking that it behaves as expected when used in a hashmap
  std::unordered_map<Raz::Mat3f, int> map;
  map.try_emplace(mat31, 1);
  map.try_emplace(mat31Swizzled, 2);
  map.try_emplace(mat31Epsilon, 3);
  map.try_emplace(mat32, 4);

  CHECK(map.size() == 4);

  CHECK(map.find(mat31)->second == 1);
  CHECK(map.find(mat31Swizzled)->second == 2);
  CHECK(map.find(mat31Epsilon)->second == 3);
  CHECK(map.find(mat32)->second == 4);

  map.erase(mat31Epsilon);
  CHECK(map.find(mat31Epsilon) == map.cend());
}

TEST_CASE("Matrix near-equality", "[math]") {
  CHECK_FALSE(mat31 == mat32);

  constexpr Raz::Mat2f baseMat = Raz::Mat2f::identity();
  Raz::Mat2f compMat = baseMat;

  CHECK(baseMat[0] == compMat[0]); // Copied, strict equality
  CHECK(baseMat[1] == compMat[1]);
  CHECK(baseMat[2] == compMat[2]);
  CHECK(baseMat[3] == compMat[3]);

  compMat += 0.0000001f; // Adding a tiny offset

  CHECK_FALSE(baseMat[0] == compMat[0]); // Values not strictly equal
  CHECK_FALSE(baseMat[1] == compMat[1]);
  CHECK_FALSE(baseMat[2] == compMat[2]);
  CHECK_FALSE(baseMat[3] == compMat[3]);

  CHECK_THAT(baseMat[0], IsNearlyEqualTo(compMat[0])); // Near-equality components check
  CHECK_THAT(baseMat[1], IsNearlyEqualTo(compMat[1]));
  CHECK_THAT(baseMat[2], IsNearlyEqualTo(compMat[2]));
  CHECK_THAT(baseMat[3], IsNearlyEqualTo(compMat[3]));

  CHECK_THAT(baseMat, IsNearlyEqualToMatrix(compMat)); // Simpler check in a single call

  CHECK(baseMat == compMat); // Matrix::operator== does a near-equality check on floating point types
}

TEST_CASE("Matrix strict equality", "[math]") {
  CHECK(mat31.strictlyEquals(mat31));
  CHECK(mat41.strictlyEquals(mat41));

  CHECK(std::equal_to<Raz::Mat3f>()(mat31, mat31));
  CHECK(std::equal_to<Raz::Mat4f>()(mat41, mat41));

  constexpr auto mat31Swizzled = Raz::Mat3f::fromRows(mat31.recoverRow(2), mat31.recoverRow(0), mat31.recoverRow(1));
  CHECK_FALSE(mat31.strictlyEquals(mat31Swizzled));
  CHECK_FALSE(std::equal_to<Raz::Mat3f>()(mat31, mat31Swizzled));

  constexpr Raz::Mat3f mat31Epsilon = mat31 + std::numeric_limits<float>::epsilon();
  CHECK(mat31 == mat31Epsilon); // Near-equality check
  CHECK_FALSE(mat31.strictlyEquals(mat31Epsilon)); // Strict-equality checks
  CHECK_FALSE(std::equal_to<Raz::Mat3f>()(mat31, mat31Epsilon));

  constexpr std::array<Raz::Mat3f, 3> matrices = { mat31Swizzled, mat31Epsilon, mat31 };

  // When using a simple find(), which uses operator==, mat31Epsilon is found instead of mat31
  const auto foundIter = std::ranges::find(matrices, mat31);
  CHECK_FALSE(std::distance(matrices.cbegin(), foundIter) == 2);

  // To search for a specific element, find_if() must be used with a strict equality check
  const auto foundIfIter = std::ranges::find_if(matrices, [&] (const Raz::Mat3f& compMat) noexcept { return mat31.strictlyEquals(compMat); });
  CHECK(std::distance(matrices.cbegin(), foundIfIter) == 2);

  constexpr std::array<Raz::Mat3f, 3> swappedMatrices = { mat31Swizzled, mat31, mat31Epsilon };

  // Trying to compare for equality fails with a simple std::equal on the matrices
  CHECK(std::equal(matrices.cbegin(), matrices.cend(), swappedMatrices.cbegin()));

  // When using the std::equal_to specialization, matrices are properly found to be unequal to each other
  CHECK_FALSE(std::equal(matrices.cbegin(), matrices.cend(), swappedMatrices.cbegin(), std::equal_to<Raz::Mat3f>()));
}

TEST_CASE("Matrix less-than", "[math]") {
  CHECK(std::less<Raz::Mat3f>()(mat31, mat32));
  CHECK_FALSE(std::less<Raz::Mat3f>()(mat32, mat31));
  CHECK_FALSE(std::less<Raz::Mat3f>()(mat31, mat31)); // Equal matrices are not strictly less than the other
  CHECK(std::less<Raz::Mat3f>()(mat31, mat31 + std::numeric_limits<float>::epsilon()));
  CHECK_FALSE(std::less<Raz::Mat3f>()(mat31 + std::numeric_limits<float>::epsilon(), mat31)); // Performs a strict check, no tolerance allowed

  CHECK(std::less<Raz::Mat4f>()(mat41, mat42));
  CHECK_FALSE(std::less<Raz::Mat4f>()(mat42, mat41));
  CHECK_FALSE(std::less<Raz::Mat4f>()(mat41, mat41));
  CHECK(std::less<Raz::Mat4f>()(mat41, mat41 + std::numeric_limits<float>::epsilon()));
  CHECK_FALSE(std::less<Raz::Mat4f>()(mat41 + std::numeric_limits<float>::epsilon(), mat41));
}

TEST_CASE("Matrix printing", "[math]") {
  std::stringstream stream;

  stream << mat31;
  CHECK(stream.str() == "[[ 4.12, 25.1, 30.7842 ]\n"
                        " [ 3.04, 5, -64.5 ]\n"
                        " [ -1, -7.54, 8.41 ]]");

  stream.str(std::string()); // Resetting the stream
  stream << mat32;
  CHECK(stream.str() == "[[ 47.4, 10.001, 15.12 ]\n"
                        " [ 8.01, -98.1, 97 ]\n"
                        " [ 12.54, 70, -54.05 ]]");

  stream.str(std::string());
  stream << mat41;
  CHECK(stream.str() == "[[ -3.2, 53.032, 832.451, 74.2 ]\n"
                        " [ 10.01, 3.15, -91.41, 187.46 ]\n"
                        " [ -6, -7.78, 90, 38 ]\n"
                        " [ 123, -74.8, 147, 748.6 ]]");

  stream.str(std::string());
  stream << Raz::Matrix<float, 3, 2>(1.f, 2.f, 3.f,
                                     4.f, 5.f, 6.f);
  CHECK(stream.str() == "[[ 1, 2, 3 ]\n"
                        " [ 4, 5, 6 ]]");
}
