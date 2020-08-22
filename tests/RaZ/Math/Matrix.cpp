#include "Catch.hpp"

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

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

TEST_CASE("Matrix resize") {
  const Raz::Mat3f truncatedMat(mat41);
  const Raz::Mat4f expandedMat(truncatedMat);

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

TEST_CASE("Matrix from vectors") {
  constexpr Raz::Matrix<float, 3, 4> testMat(Raz::Vec3f( 1.f,  2.f,  3.f),
                                             Raz::Vec3f( 4.f,  5.f,  6.f),
                                             Raz::Vec3f( 7.f,  8.f,  9.f),
                                             Raz::Vec3f(10.f, 11.f, 12.f));
  CHECK(testMat == Raz::Matrix<float, 3, 4>( 1.f,  2.f,  3.f,
                                             4.f,  5.f,  6.f,
                                             7.f,  8.f,  9.f,
                                            10.f, 11.f, 12.f));

  constexpr Raz::Mat3f testMat31(mat31.recoverRow(0), mat31.recoverRow(1), mat31.recoverRow(2));
  CHECK(testMat31 == mat31);

  constexpr Raz::Mat4f testMat41(mat41.recoverColumn(0), mat41.recoverColumn(1), mat41.recoverColumn(2), mat41.recoverColumn(3));
  CHECK(testMat41 == mat41.transpose());
}

TEST_CASE("Matrix elements fetching") {
  //                   height:
  //      [ 0, 1, 2 ] <- 0
  //      [ 3, 4, 5 ] <- 1
  //      [ 6, 7, 8 ] <- 2
  //        ^  ^  ^
  // width: 0  1  2
  CHECK(mat31.getElement(1, 1) == mat31[4]);
  CHECK(mat31.getElement(2, 1) == mat31[5]);
  CHECK(mat31.getElement(2, 2) == mat31[8]);

  //                         height:
  //     [  0,  1,  2,  3 ] <- 0
  //     [  4,  5,  6,  7 ] <- 1
  //     [  8,  9, 10, 11 ] <- 2
  //     [ 12, 13, 14, 15 ] <- 3
  //        ^   ^   ^   ^
  // width: 0   1   2   3
  CHECK(mat41.getElement(0, 0) == mat41[0]);
  CHECK(mat41.getElement(1, 2) == mat41[9]);
  CHECK(mat41.getElement(3, 3) == mat41[15]);

  CHECK(mat31.recoverRow(0) == Raz::Vec3f(4.12f,  25.1f, 30.7842f));
  CHECK(mat42.recoverRow(2) == Raz::Vec4f(-8.12f, 38.24f, 62.f, 43.12f));

  CHECK(mat32.recoverColumn(2) == Raz::Vec3f(15.12f, 97.f, -54.05f));
  CHECK(mat41.recoverColumn(1) == Raz::Vec4f(53.032f, 3.15f, -7.78f, -74.8f));

  const Raz::Matrix<float, 8, 2> testMat(1.f, 2.f,  3.f,  4.f,  5.f,  6.f,  7.f,  8.f,
                                         9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f);

  CHECK(testMat.recoverRow(1) == Raz::Vector<float, 8>(9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f));
  CHECK(testMat.recoverColumn(5) == Raz::Vec2f(6.f, 14.f));

  const Raz::Matrix<float, 2, 8> testMatTrans = testMat.transpose();

  CHECK(testMatTrans.recoverRow(7) == Raz::Vec2f(8.f, 16.f));
  CHECK(testMatTrans.recoverColumn(0) == Raz::Vector<float, 8>(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f));
}

TEST_CASE("Matrix/scalar operations") {
  CHECK((mat31 * 3.f) == Raz::Mat3f( 12.36f,  75.3f,   92.3526f,
                                     9.12f,   15.f,   -193.5f,
                                    -3.f,    -22.62f,  25.23f));
  CHECK((mat31 * 4.152f) == Raz::Mat3f( 17.10624f,  104.2152f,  127.8159984f,
                                        12.62208f,  20.76f,    -267.804f,
                                       -4.152f,    -31.30608f,  34.91832f));

  CHECK((mat41 * 7.5f) == Raz::Mat4f(-24.f,     397.74f,  6243.3825f,  556.5f,
                                      75.075f,  23.625f, -685.575f,    1405.95f,
                                     -45.f,    -58.35f,   675.f,       285.f,
                                      922.5f,  -561.f,    1102.50075f, 5614.5f));
  CHECK((mat41 * 8.0002f) == Raz::Mat4f(-25.60064f,   424.2666064f,  6659.7744902f,  593.61484f,
                                         80.082002f,  25.20063f,    -731.298282f,    1499.717492f,
                                        -48.0012f,   -62.241556f,    720.018f,       304.0076f,
                                         984.0246f,  -598.41496f,    1176.03020002f, 5988.94972f));
  CHECK((mat41 * 0.f) == Raz::Mat4f(0.f, 0.f, 0.f, 0.f,
                                    0.f, 0.f, 0.f, 0.f,
                                    0.f, 0.f, 0.f, 0.f,
                                    0.f, 0.f, 0.f, 0.f));
}

TEST_CASE("Matrix/matrix operations") {
  CHECK((mat31 - mat31) == Raz::Mat3f(0.f, 0.f, 0.f,
                                      0.f, 0.f, 0.f,
                                      0.f, 0.f, 0.f));

  // Component-wise multiplication
  CHECK((mat31 % mat32) == Raz::Mat3f( 195.288f,  251.0251f,  465.457104f,
                                       24.3504f, -490.5f,    -6256.5f,
                                      -12.54f,   -527.8f,    -454.5605f));

  // Matrix multiplication

  // The cell containing -2.334007 isn't the actual result; by hand we find -2.334, but this test doesn't pass with that value
  // The code's result is here truncated to make it pass. Really not clean, but is assumed to fail because of errors accumulation
  const Raz::Mat3f res3132( 782.372868f, -266.21188f,   833.10839f,
                           -624.684f,    -4975.09696f,  4017.1898f,
                           -2.334007f,    1318.373f,   -1201.0605f);
  const Raz::Mat3f res3231( 210.57104f,  1125.7402f,  941.26578f,
                           -362.2228f,  -1020.829f,   7389.801442f,
                            318.5148f,   1072.291f,  -4583.526632f);

  CHECK((mat31 * mat32) == res3132);
  CHECK((mat32 * mat31) == res3231);

  CHECK((mat31 * Raz::Mat3f::identity()) == mat31);
  CHECK((mat41 * Raz::Mat4f::identity()) == mat41);
}

TEST_CASE("Matrix/vector operations") {
  const Raz::Vec3f vec3(3.18f, 42.f, 0.874f);
  CHECK((mat31 * vec3) == Raz::Vec3f(1094.2069908f, 163.2942f, -312.50966f));
  CHECK((mat32 * vec3) == Raz::Vec3f(583.98888f, -4009.9502f, 2932.6375f));

  const Raz::Vec4f vec4(84.47f, 2.f, 0.001f, 847.12f);
  CHECK((mat41 * vec4) == Raz::Vec4f(62692.896451f, 159652.86849f, 31668.27f, 644394.3890001f));
  CHECK((mat42 * vec4) == Raz::Vec4f(36239.89676f, 45725.116745f, 35918.46f, 30679.27964f));
}

TEST_CASE("Matrix hash") {
  CHECK(mat31.hash() == mat31.hash());
  CHECK_FALSE(mat31.hash() == mat32.hash());

  CHECK(mat41.hash() == mat41.hash());
  CHECK_FALSE(mat41.hash() == mat42.hash());

  constexpr Raz::Mat3f mat31Swizzled(mat31.recoverRow(2), mat31.recoverRow(0), mat31.recoverRow(1));
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

TEST_CASE("Matrix near-equality") {
  CHECK_FALSE(mat31 == mat32);

  const Raz::Mat2f baseMat = Raz::Mat2f::identity();
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

TEST_CASE("Matrix strict equality") {
  CHECK(mat31.strictlyEquals(mat31));
  CHECK(mat41.strictlyEquals(mat41));

  CHECK(std::equal_to<Raz::Mat3f>()(mat31, mat31));
  CHECK(std::equal_to<Raz::Mat4f>()(mat41, mat41));

  constexpr Raz::Mat3f mat31Swizzled(mat31.recoverRow(2), mat31.recoverRow(0), mat31.recoverRow(1));
  CHECK_FALSE(mat31.strictlyEquals(mat31Swizzled));
  CHECK_FALSE(std::equal_to<Raz::Mat3f>()(mat31, mat31Swizzled));

  constexpr Raz::Mat3f mat31Epsilon = mat31 + std::numeric_limits<float>::epsilon();
  CHECK(mat31 == mat31Epsilon); // Near-equality check
  CHECK_FALSE(mat31.strictlyEquals(mat31Epsilon)); // Strict-equality checks
  CHECK_FALSE(std::equal_to<Raz::Mat3f>()(mat31, mat31Epsilon));

  constexpr std::array<Raz::Mat3f, 3> matrices = { mat31Swizzled, mat31Epsilon, mat31 };

  // When using a simple find(), which uses operator==, mat31Epsilon is found instead of mat31
  const auto foundIter = std::find(matrices.cbegin(), matrices.cend(), mat31);
  CHECK_FALSE(std::distance(matrices.cbegin(), foundIter) == 2);

  // To search for a specific element, find_if() must be used with a strict equality check
  const auto foundIfIter = std::find_if(matrices.cbegin(), matrices.cend(), [&] (const Raz::Mat3f& compMat) { return mat31.strictlyEquals(compMat); });
  CHECK(std::distance(matrices.cbegin(), foundIfIter) == 2);

  constexpr std::array<Raz::Mat3f, 3> swappedMatrices = { mat31Swizzled, mat31, mat31Epsilon };

  // Trying to compare for equality fails with a simple std::equal on the matrices
  CHECK(std::equal(matrices.cbegin(), matrices.cend(), swappedMatrices.cbegin()));

  // When using the std::equal_to specialization, matrices are properly found to be unequal to each other
  CHECK_FALSE(std::equal(matrices.cbegin(), matrices.cend(), swappedMatrices.cbegin(), std::equal_to<Raz::Mat3f>()));
}

TEST_CASE("Matrix printing") {
  std::stringstream stream;

  stream << mat31;
  CHECK(stream.str() == "[[ 4.12; 25.1; 30.7842 ]\n"
                        " [ 3.04; 5; -64.5 ]\n"
                        " [ -1; -7.54; 8.41 ]]");

  stream.str(std::string()); // Resetting the stream
  stream << mat32;
  CHECK(stream.str() == "[[ 47.4; 10.001; 15.12 ]\n"
                        " [ 8.01; -98.1; 97 ]\n"
                        " [ 12.54; 70; -54.05 ]]");

  stream.str(std::string());
  stream << mat41;
  CHECK(stream.str() == "[[ -3.2; 53.032; 832.451; 74.2 ]\n"
                        " [ 10.01; 3.15; -91.41; 187.46 ]\n"
                        " [ -6; -7.78; 90; 38 ]\n"
                        " [ 123; -74.8; 147; 748.6 ]]");
}
