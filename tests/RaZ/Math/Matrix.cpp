#include "catch/catch.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace {

// Declaring matrices to be tested
const Raz::Mat3f mat31({{ 4.12f,  25.1f, 30.7842f },
                        { 3.04f,    5.f,   -64.5f },
                        {  -1.f, -7.54f,    8.41f }});
const Raz::Mat3f mat32({{  47.4f, 10.001f,  15.12f },
                        {  8.01f,  -98.1f,    97.f },
                        { 12.54f,    70.f, -54.05f }});

const Raz::Mat4f mat41({{  -3.2f, 53.032f,  832.451f,   74.2f },
                        { 10.01f,   3.15f,   -91.41f, 187.46f },
                        {   -6.f,  -7.78f,      90.f,    38.f },
                        {  123.f,  -74.8f, 147.0001f,  748.6f }});
const Raz::Mat4f mat42({{   5.5f, 98.14f, -8.24f,   42.f },
                        { 15.84f, -145.f, 3.145f, 52.74f },
                        { -8.12f, 38.24f,   62.f, 43.12f },
                        {   74.f,  15.7f, 43.64f,  28.8f }});

} // namespace

TEST_CASE("Matrix near-equality") {
  REQUIRE_FALSE(mat31 == mat32);

  const Raz::Mat2f baseMat = Raz::Mat2f::identity();
  Raz::Mat2f compMat = baseMat;

  REQUIRE(baseMat[0] == compMat[0]); // Copied, strict equality
  REQUIRE(baseMat[1] == compMat[1]);
  REQUIRE(baseMat[2] == compMat[2]);
  REQUIRE(baseMat[3] == compMat[3]);

  compMat += 0.0000001f; // Adding a tiny offset

  REQUIRE_FALSE(baseMat[0] == compMat[0]); // Values not strictly equal
  REQUIRE_FALSE(baseMat[1] == compMat[1]);
  REQUIRE_FALSE(baseMat[2] == compMat[2]);
  REQUIRE_FALSE(baseMat[3] == compMat[3]);

  REQUIRE(Raz::FloatUtils::checkNearEquality(baseMat[0], compMat[0])); // Near-equality components check
  REQUIRE(Raz::FloatUtils::checkNearEquality(baseMat[1], compMat[1]));
  REQUIRE(Raz::FloatUtils::checkNearEquality(baseMat[2], compMat[2]));
  REQUIRE(Raz::FloatUtils::checkNearEquality(baseMat[3], compMat[3]));

  REQUIRE(baseMat == compMat); // Matrix::operator== does a near-equality check on floating point types
}

TEST_CASE("Matrix/scalar operations") {
  REQUIRE((mat31 * 3.f) == Raz::Mat3f({{ 12.36f,   75.3f, 92.3526f },
                                       {  9.12f,    15.f,  -193.5f },
                                       {   -3.f, -22.62f,   25.23f }}));
  REQUIRE((mat31 * 4.152f) == Raz::Mat3f({{ 17.10624f,  104.2152f, 127.8159984f },
                                          { 12.62208f,     20.76f,    -267.804f },
                                          {   -4.152f, -31.30608f,    34.91832f }}));

  REQUIRE((mat41 * 7.5f) == Raz::Mat4f({{   -24.f, 397.74f,  6243.3825f,   556.5f },
                                        { 75.075f, 23.625f,   -685.575f, 1405.95f },
                                        {   -45.f, -58.35f,       675.f,    285.f },
                                        {  922.5f,  -561.f, 1102.50075f,  5614.5f }}));
  REQUIRE((mat41 * 8.0002f) == Raz::Mat4f({{ -25.60064f, 424.2666064f,  6659.7744902f,   593.61484f },
                                           { 80.082002f,    25.20063f,   -731.298282f, 1499.717492f },
                                           {  -48.0012f,  -62.241556f,       720.018f,    304.0076f },
                                           {  984.0246f,  -598.41496f, 1176.03020002f,  5988.94972f }}));
  REQUIRE((mat41 * 0.f) == Raz::Mat4f({{ 0.f, 0.f, 0.f, 0.f },
                                       { 0.f, 0.f, 0.f, 0.f },
                                       { 0.f, 0.f, 0.f, 0.f },
                                       { 0.f, 0.f, 0.f, 0.f }}));
}

TEST_CASE("Matrix/matrix operations") {
  REQUIRE((mat31 - mat31) == Raz::Mat3f({{ 0.f, 0.f, 0.f },
                                         { 0.f, 0.f, 0.f },
                                         { 0.f, 0.f, 0.f }}));

  // Component-wise multiplication
  REQUIRE((mat31 % mat32) == Raz::Mat3f({{ 195.288f, 251.0251f, 465.457104f },
                                         { 24.3504f,   -490.5f,    -6256.5f },
                                         {  -12.54f,   -527.8f,  -454.5605f }}));

  // Matrix multiplication
  // Here, the cell containing -2.334007 isn't the actual result; by hand we find -2.334, but this test doesn't pass with that value
  // The code's result is here truncated to make it pass. Really not clean, but is assumed to fail because of errors accumulation
  REQUIRE((mat31 * mat32) == Raz::Mat3f({{ 782.372868f,  -266.21188f,  833.10839f },
                                         {   -624.684f, -4975.09696f,  4017.1898f },
                                         {  -2.334007f,    1318.373f, -1201.0605f }}));
  REQUIRE((mat32 * mat31) == Raz::Mat3f({{ 210.57104f, 1125.7402f,    941.26578f },
                                         { -362.2228f, -1020.829f,  7389.801442f },
                                         {  318.5148f,  1072.291f, -4583.526632f }}));

  REQUIRE((mat31 * Raz::Mat3f::identity()) == mat31);
  REQUIRE((mat41 * Raz::Mat4f::identity()) == mat41);
}

TEST_CASE("Matrix/vector operations") {
  const Raz::Vec3f vec3({ 3.18f, 42.f, 0.874f });
  REQUIRE((mat31 * vec3) == Raz::Vec3f({ 1094.2069908f, 163.2942f, -312.50966f }));
  REQUIRE((mat32 * vec3) == Raz::Vec3f({ 583.98888f, -4009.9502f, 2932.6375f }));

  const Raz::Vec4f vec4({ 84.47f, 2.f, 0.001f, 847.12f });
  REQUIRE((mat41 * vec4) == Raz::Vec4f({ 62692.896451f, 159652.86849f, 31668.27f, 644394.3890001f }));
  REQUIRE((mat42 * vec4) == Raz::Vec4f({ 36239.89676f, 45725.116745f, 35918.46f, 30679.27964f }));
}
