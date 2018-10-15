#include "catch/catch.hpp"
#include "RaZ/Math/Matrix.hpp"

namespace {

// Declaring matrices to be tested
const Raz::Mat3f mat31({{ 4.12f,  25.1f, 30.7842f },
                        { 3.04f,    5.f,   -64.5f },
                        {  -1.f, -7.54f,    8.41f }});
const Raz::Mat3f mat32({{  47.4f, 10.001f,  15.12f },
                        {  8.01f,  -98.1f,    97.f },
                        { 12.54f,    70.f, -54.05f }});

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
