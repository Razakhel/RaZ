#include "Catch.hpp"

#include "RaZ/Math/Quaternion.hpp"

using namespace Raz::Literals;

namespace {

const Raz::Quaternionf quat1(10.0_deg, Raz::Axis::X);
const Raz::Quaternionf quat2(90.0_deg, 1.f, -2.f, 5.f);
const Raz::Quaternionf quat3(180.0_deg, 1.f, -2.f, 5.f);

} // namespace

TEST_CASE("Quaternion norm computation") {
  CHECK_THAT(quat1.computeSquaredNorm(), IsNearlyEqualTo(1.f));
  CHECK_THAT(quat1.computeNorm(), IsNearlyEqualTo(1.f));

  CHECK_THAT(quat2.computeSquaredNorm(), IsNearlyEqualTo(15.5f));
  CHECK_THAT(quat2.computeNorm(), IsNearlyEqualTo(3.93700385f));

  CHECK_THAT(quat3.computeSquaredNorm(), IsNearlyEqualTo(30.f));
  CHECK_THAT(quat3.computeNorm(), IsNearlyEqualTo(5.47722578f));
}

TEST_CASE("Quaternion matrix computation") {
  CHECK(quat1.computeMatrix() == Raz::Mat4f({{ 1.f,           0.f,          0.f, 0.f },
                                             { 0.f,   0.98480773f, 0.173648179f, 0.f },
                                             { 0.f, -0.173648179f,  0.98480773f, 0.f },
                                             { 0.f,           0.f,          0.f, 1.f }}));

  CHECK(quat2.computeMatrix() == Raz::Mat4f({{ -0.870967627f,  0.193548396f,   0.45161289f, 0.f },
                                             {  -0.45161289f, -0.677419245f, -0.580645144f, 0.f },
                                             {  0.193548396f, -0.709677398f,  0.677419424f, 0.f },
                                             {           0.f,           0.f,           0.f, 1.f }}));

  CHECK(quat3.computeMatrix() == Raz::Mat4f({{ -0.933333397f, -0.133333355f,  0.333333343f, 0.f },
                                             { -0.133333325f, -0.733333409f, -0.666666687f, 0.f },
                                             {  0.333333343f, -0.666666687f,  0.666666627f, 0.f },
                                             {           0.f,           0.f,           0.f, 1.f }}));
}

TEST_CASE("Quaternion multiplication") {
  const auto unitQuat = Raz::Quaternionf::identity();
  const Raz::Quaternionf squareUnitQuat = unitQuat * unitQuat;
  CHECK(squareUnitQuat.computeMatrix() == unitQuat.computeMatrix());

  const Raz::Quaternionf quatRotX(45.0_deg, Raz::Axis::X);
  const Raz::Quaternionf multRotX = quatRotX * quatRotX.conjugate();
  CHECK(multRotX.computeMatrix() == unitQuat.computeMatrix());

  const Raz::Quaternionf quatRotY(45.0_deg, Raz::Axis::Y);
  const Raz::Quaternionf multRotY = quatRotY * quatRotY.conjugate();
  CHECK(multRotY.computeMatrix() == unitQuat.computeMatrix());

  const Raz::Quaternionf quatRotZ(45.0_deg, Raz::Axis::Z);
  const Raz::Quaternionf multRotZ = quatRotZ * quatRotZ.conjugate();
  CHECK(multRotZ.computeMatrix() == unitQuat.computeMatrix());

  // From Wolfram Alpha:
  // https://www.wolframalpha.com/input/?i=quaternion%280.99619472%2C+0.0871557444%2C+0%2C+0%29+*+quaternion%280.707106769%2C+0.707106769%2C+-1.41421354%2C+3.53553391%29
  const Raz::Quaternionf quat12 = quat1 * quat2;
  CHECK_THAT(quat12.computeNorm(), IsNearlyEqualTo(3.93700385f));

  const Raz::Mat4f expected({{ -0.870967627f,  0.112186f,   0.478361f, 0.f },
                             {  -0.45161289f,   -0.5663f, -0.6894576f, 0.f },
                             {  0.193548396f, -0.816528f,   0.543894f, 0.f },
                             {           0.f,        0.f,         0.f, 1.f }});
  const Raz::Mat4f result = quat12.computeMatrix();

  for (std::size_t i = 0; i < 16; ++i)
    CHECK(result[i] == Approx(expected[i]));
}
