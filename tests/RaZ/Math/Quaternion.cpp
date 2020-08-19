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
  const Raz::Mat4f quat1Mat = quat1.computeMatrix();
  const Raz::Mat4f quat2Mat = quat2.computeMatrix();
  const Raz::Mat4f quat3Mat = quat3.computeMatrix();

  // Checking that quaternion multiplication is symmetrical to the matrices'
  // The matrix multiplication may introduce a slight difference compared to the quaternion's
  CHECK_THAT((quat1 * quat2).computeMatrix(), IsNearlyEqualToMatrix(quat1Mat * quat2Mat, std::numeric_limits<float>::epsilon() * 2));
  CHECK_THAT((quat3 * quat1).computeMatrix(), IsNearlyEqualToMatrix(quat3Mat * quat1Mat));

  CHECK_THAT(quat1.computeMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(1.f,  0.f,           0.f,          0.f,
                                                                     0.f,  0.98480773f,  -0.173648179f, 0.f,
                                                                     0.f,  0.173648179f,  0.98480773f,  0.f,
                                                                     0.f,  0.f,           0.f,          1.f)));

  CHECK_THAT(quat2.computeMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(-0.870967627f, -0.45161289f,   0.193548396f, 0.f,
                                                                      0.193548396f, -0.677419245f, -0.709677398f, 0.f,
                                                                      0.45161289f,  -0.580645144f,  0.677419424f, 0.f,
                                                                      0.f,           0.f,           0.f,          1.f)));

  CHECK_THAT(quat3.computeMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(-0.933333397f, -0.133333355f,  0.333333343f, 0.f,
                                                                     -0.133333325f, -0.733333409f, -0.666666687f, 0.f,
                                                                      0.333333343f, -0.666666687f,  0.666666627f, 0.f,
                                                                      0.f,           0.f,           0.f,          1.f)));
}

TEST_CASE("Quaternion multiplication") {
  const Raz::Quaternionf unitQuat = Raz::Quaternionf::identity();
  const Raz::Mat4f unitQuatMat    = unitQuat.computeMatrix();
  CHECK(unitQuatMat == Raz::Mat4f::identity());

  const Raz::Quaternionf squareUnitQuat = unitQuat * unitQuat;
  CHECK(squareUnitQuat.computeMatrix() == unitQuatMat);

  const Raz::Quaternionf quatRotX(45.0_deg, Raz::Axis::X);
  const Raz::Quaternionf multRotX = quatRotX * quatRotX.conjugate();
  CHECK(multRotX.computeMatrix() == unitQuatMat);

  const Raz::Quaternionf quatRotY(45.0_deg, Raz::Axis::Y);
  const Raz::Quaternionf multRotY = quatRotY * quatRotY.conjugate();
  CHECK(multRotY.computeMatrix() == unitQuatMat);

  const Raz::Quaternionf quatRotZ(45.0_deg, Raz::Axis::Z);
  const Raz::Quaternionf multRotZ = quatRotZ * quatRotZ.conjugate();
  CHECK(multRotZ.computeMatrix() == unitQuatMat);

  const Raz::Quaternionf quat12 = quat1 * quat2;

  // Results taken from Wolfram Alpha: https://tinyurl.com/rxavw82
  // The matrix computation applies a transposition for consistency; the resulting matrix is thus transposed compared to Wolfram's

  CHECK_THAT(quat12.computeNorm(), IsNearlyEqualTo(3.9370039f));
  CHECK_THAT(quat12.computeMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(-0.870968f,  -0.451613f,   0.1935484f, 0.f,
                                                                       0.1121862f, -0.5663f,    -0.8165285f, 0.f,
                                                                       0.4783612f, -0.6894565f,  0.5438936f, 0.f,
                                                                       0.f,         0.f,         0.f,        1.f)));
}
