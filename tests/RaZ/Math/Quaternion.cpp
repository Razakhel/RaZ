#include "Catch.hpp"

#include "RaZ/Math/Quaternion.hpp"

using namespace Raz::Literals;

namespace {

const Raz::Quaternionf quat1(10_deg, Raz::Axis::X);
const Raz::Quaternionf quat2(90_deg, 1.f, -2.f, 5.f);
const Raz::Quaternionf quat3(180_deg, 1.f, -2.f, 5.f);

} // namespace

TEST_CASE("Quaternion dot product") {
  const Raz::Quaternionf normedQuat1 = quat1.normalize();
  const Raz::Quaternionf normedQuat2 = quat2.normalize();
  const Raz::Quaternionf normedQuat3 = quat3.normalize();

  CHECK(quat1.dot(quat1) == 1.f);
  CHECK(normedQuat1.dot(normedQuat1) == 1.f);

  CHECK(quat2.dot(quat2) == 15.5f);
  CHECK_THAT(normedQuat2.dot(normedQuat2), IsNearlyEqualTo(1.f));

  CHECK(quat3.dot(quat3) == 30.f);
  CHECK_THAT(normedQuat3.dot(normedQuat3), IsNearlyEqualTo(1.f));

  CHECK_THAT(quat1.dot(quat2), IsNearlyEqualTo(0.7660444f));
  CHECK_THAT(normedQuat1.dot(normedQuat2), IsNearlyEqualTo(0.1945755f));
  CHECK(quat1.dot(quat2) == quat2.dot(quat1));

  CHECK_THAT(quat3.dot(quat1), IsNearlyEqualTo(0.0871557f));
  CHECK_THAT(normedQuat3.dot(normedQuat1), IsNearlyEqualTo(0.0159124f));
  CHECK(quat3.dot(quat1) == quat1.dot(quat3));
}

TEST_CASE("Quaternion norm computation") {
  CHECK_THAT(quat1.computeSquaredNorm(), IsNearlyEqualTo(1.f));
  CHECK_THAT(quat1.computeNorm(), IsNearlyEqualTo(1.f));

  CHECK_THAT(quat2.computeSquaredNorm(), IsNearlyEqualTo(15.5f));
  CHECK_THAT(quat2.computeNorm(), IsNearlyEqualTo(3.93700385f));

  CHECK_THAT(quat3.computeSquaredNorm(), IsNearlyEqualTo(30.f));
  CHECK_THAT(quat3.computeNorm(), IsNearlyEqualTo(5.47722578f));
}

TEST_CASE("Quaternion interpolation") {
  // Results taken from Rust's nalgebra crate: https://play.rust-lang.org/?version=stable&mode=release&edition=2018&gist=fb93e96b13f3708b3b54aacef65365c2

  CHECK(quat1.lerp(quat2, 0.f) == quat1);
  CHECK_THAT(quat1.lerp(quat2, 0.25f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.92392272f, 0.2421435f, -0.35355338f, 0.88388348f)));
  CHECK_THAT(quat1.lerp(quat2, 0.5f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.85165071f, 0.39713126f, -0.70710677f, 1.767767f)));
  CHECK_THAT(quat1.lerp(quat2, 0.75f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.77937877f, 0.552119f, -1.06066f, 2.6516504f)));
  CHECK(quat1.lerp(quat2, 1.f) == quat2);

  const Raz::Quaternionf quat1Norm = quat1.normalize();
  const Raz::Quaternionf quat2Norm = quat2.normalize();

  // Direct nlerp() (quaternions are not normalized beforehand)
  CHECK(quat1.nlerp(quat2, 0.f) == quat1Norm);
  CHECK_THAT(quat1.nlerp(quat2, 0.25f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.68513638f, 0.179562f, -0.262178f, 0.65544516f)));
  CHECK_THAT(quat1.nlerp(quat2, 0.5f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.40111461f, 0.18704282f, -0.3330366f, 0.83259147f)));
  CHECK_THAT(quat1.nlerp(quat2, 0.75f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.25880963f, 0.183343f, -0.3522152f, 0.880538f)));
  CHECK(quat1.nlerp(quat2, 1.f) == quat2Norm);

  // Pre-normalized nlerp() (almost strictly equivalent to nlerp(...).normalize() with non-normalized quaternions like above)
  CHECK(quat1Norm.nlerp(quat2Norm, 0.f) == quat1Norm);
  CHECK_THAT(quat1Norm.nlerp(quat2Norm, 0.25f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.94805622f, 0.13198756f, -0.10749099f, 0.26872748f)));
  CHECK_THAT(quat1Norm.nlerp(quat2Norm, 0.5f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.76069689f, 0.17258403f, -0.23239529f, 0.58098823f)));
  CHECK_THAT(quat1Norm.nlerp(quat2Norm, 0.75f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.45934007f, 0.18731718f, -0.32247299f, 0.8061825f)));
  CHECK(quat1Norm.nlerp(quat2Norm, 1.f) == quat2Norm);

  CHECK(quat1Norm.slerp(quat2Norm, 0.f) == quat1Norm);
  CHECK_THAT(quat1Norm.slerp(quat2Norm, 0.25f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.93302816f, 0.13793936f, -0.12341759f, 0.30854398f)));
  CHECK_THAT(quat1Norm.slerp(quat2Norm, 0.5f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.76069695f, 0.17258403f, -0.23239529f, 0.58098823f)));
  CHECK_THAT(quat1Norm.slerp(quat2Norm, 0.75f), IsNearlyEqualToQuaternion(Raz::Quaternionf(0.49936396f, 0.18703631f, -0.31418267f, 0.78545672f)));
  CHECK(quat1Norm.slerp(quat2Norm, 1.f) == quat2Norm);
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

TEST_CASE("Quaternion/vector multiplication") {
  // See: https://www.geogebra.org/m/aderzasm

  const Raz::Quaternionf quatRotX(45_deg, Raz::Axis::X);
  CHECK(quatRotX * Raz::Axis::X == Raz::Axis::X);
  CHECK(Raz::Axis::X * quatRotX == Raz::Axis::X);

  const Raz::Quaternionf quatRotY(45_deg, Raz::Axis::Y);
  CHECK(quatRotY * Raz::Axis::X == Raz::Vec3f(0.70710677f, 0.f, -0.70710677f));
  CHECK(Raz::Axis::X * quatRotY == Raz::Vec3f(0.70710677f, 0.f, 0.70710677f));

  const Raz::Quaternionf quatRotZ(45_deg, Raz::Axis::Z);
  CHECK(quatRotZ * Raz::Axis::X == Raz::Vec3f(0.70710677f, 0.70710677f, 0.f));
  CHECK(Raz::Axis::X * quatRotZ == Raz::Vec3f(0.70710677f, -0.70710677f, 0.f));

  CHECK(Raz::Axis::Z * Raz::Quaternionf(90_deg, Raz::Axis::Y) == -Raz::Axis::X);
  CHECK(Raz::Axis::Z * Raz::Mat3f(Raz::Quaternionf(90_deg, Raz::Axis::Y).computeMatrix()) == -Raz::Axis::X);

  constexpr Raz::Vec3f pos(0.123f, 14.51145f, 7.58413f);

  CHECK(quatRotX * pos == Raz::Vec3f(pos.x(), 4.89835405f, 15.6239338f));
  CHECK(quatRotY * pos == Raz::Vec3f(5.44976425f, pos.y(), 5.27581501f));
  CHECK(quatRotZ * pos == Raz::Vec3f(-10.1741714f, 10.3481178f, pos.z()));

  CHECK(pos * quatRotX == Raz::Vec3f(pos.x(), 15.6239338f, -4.89835596f));
  CHECK(pos * quatRotY == Raz::Vec3f(-5.27581596f, pos.y(), 5.4497633f));
  CHECK(pos * quatRotZ == Raz::Vec3f(10.3481188f, 10.1741686f, pos.z()));

  // Checking that the matrix multiplication behaves the same way

  CHECK(quatRotX * pos == Raz::Mat3f(quatRotX.computeMatrix()) * pos);
  CHECK(quatRotY * pos == Raz::Mat3f(quatRotY.computeMatrix()) * pos);
  CHECK(quatRotZ * pos == Raz::Mat3f(quatRotZ.computeMatrix()) * pos);

  CHECK(pos * quatRotX == pos * Raz::Mat3f(quatRotX.computeMatrix()));
  CHECK(pos * quatRotY == pos * Raz::Mat3f(quatRotY.computeMatrix()));
  CHECK(pos * quatRotZ == pos * Raz::Mat3f(quatRotZ.computeMatrix()));
}

TEST_CASE("Quaternion/quaternion multiplication") {
  constexpr Raz::Quaternionf unitQuat = Raz::Quaternionf::identity();
  constexpr Raz::Mat4f unitQuatMat    = unitQuat.computeMatrix();
  CHECK(unitQuatMat == Raz::Mat4f::identity());

  constexpr Raz::Quaternionf squareUnitQuat = unitQuat * unitQuat;
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
  CHECK(quat12.w() == 0.642787635f);
  CHECK(quat12.x() == 0.766044438f);
  CHECK(quat12.y() == -1.716974139f);
  CHECK(quat12.z() == 3.398823261f);
  CHECK_THAT(quat12.computeNorm(), IsNearlyEqualTo(3.9370039f));
  CHECK_THAT(quat12.computeMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(-0.870968f,  -0.451613f,   0.1935484f, 0.f,
                                                                       0.1121862f, -0.5663f,    -0.8165285f, 0.f,
                                                                       0.4783612f, -0.6894565f,  0.5438936f, 0.f,
                                                                       0.f,         0.f,         0.f,        1.f)));

  const Raz::Quaternionf quat21 = quat2 * quat1;

  // Results taken from Wolfram Alpha: https://tinyurl.com/426hupnb
  CHECK(quat21.w() == 0.642787635f);
  CHECK(quat21.x() == 0.766044438f);
  CHECK(quat21.y() == -1.10069f);
  CHECK(quat21.z() == 3.645337105f);
  CHECK_THAT(quat21.computeNorm(), IsNearlyEqualTo(3.9370039f));
  CHECK_THAT(quat21.computeMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(-0.870968f,    -0.411142588f,  0.269029707f, 0.f,
                                                                       0.193548396f, -0.790362f,    -0.581263244f, 0.f,
                                                                       0.451613f,    -0.454191267f,  0.76795578f,  0.f,
                                                                       0.f,           0.f,           0.f,          1.f)));
}

TEST_CASE("Quaternion near-equality") {
  CHECK_FALSE(quat1 == quat2);

  constexpr Raz::Quaternionf baseQuat(1.f, 1.f, 1.f, 1.f);
  Raz::Quaternionf compQuat = baseQuat;

  CHECK(baseQuat.w() == compQuat.w()); // Copied, strict equality
  CHECK(baseQuat.x() == compQuat.x());
  CHECK(baseQuat.y() == compQuat.y());
  CHECK(baseQuat.z() == compQuat.z());

  compQuat = Raz::Quaternionf(baseQuat.w() + 0.0000001f,
                              baseQuat.x() + 0.0000001f,
                              baseQuat.y() + 0.0000001f,
                              baseQuat.z() + 0.0000001f); // Adding a tiny offset

  CHECK_FALSE(baseQuat.w() == compQuat.w()); // Values not strictly equal
  CHECK_FALSE(baseQuat.x() == compQuat.x());
  CHECK_FALSE(baseQuat.y() == compQuat.y());
  CHECK_FALSE(baseQuat.z() == compQuat.z());

  CHECK_THAT(baseQuat.w(), IsNearlyEqualTo(compQuat.w())); // Near-equality components check
  CHECK_THAT(baseQuat.x(), IsNearlyEqualTo(compQuat.x()));
  CHECK_THAT(baseQuat.y(), IsNearlyEqualTo(compQuat.y()));
  CHECK_THAT(baseQuat.z(), IsNearlyEqualTo(compQuat.z()));

  CHECK_THAT(baseQuat, IsNearlyEqualToQuaternion(compQuat)); // Simpler check in a single call

  CHECK(baseQuat == compQuat); // Quaternion::operator== does a near-equality check
}

TEST_CASE("Quaternion printing") {
  std::stringstream stream;

  stream << quat1;
  CHECK(stream.str() == "[ 0.996195; 0.0871557; 0; 0 ]");

  stream.str(std::string()); // Resetting the stream
  stream << quat2;
  CHECK(stream.str() == "[ 0.707107; 0.707107; -1.41421; 3.53553 ]");

  stream.str(std::string());
  stream << quat3;
  CHECK(stream.str() == "[ -4.37114e-08; 1; -2; 5 ]");
}
