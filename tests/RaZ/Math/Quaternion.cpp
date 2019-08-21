#include "catch/catch.hpp"
#include "RaZ/Math/Quaternion.hpp"

using namespace Raz::Literals;

namespace {

const Raz::Quaternionf quat1(10.0_deg, Raz::Axis::X);
const Raz::Quaternionf quat2(90.0_deg, 1.f, -2.f, 5.f);
const Raz::Quaternionf quat3(180.0_deg, 1.f, -2.f, 5.f);

} // namespace

TEST_CASE("Quaternion norm computation") {
  CHECK(Raz::FloatUtils::areNearlyEqual(quat1.computeSquaredNorm(), 1.f));
  CHECK(Raz::FloatUtils::areNearlyEqual(quat1.computeNorm(), 1.f));

  CHECK(Raz::FloatUtils::areNearlyEqual(quat2.computeSquaredNorm(), 15.5f));
  CHECK(Raz::FloatUtils::areNearlyEqual(quat2.computeNorm(), 3.93700385f));

  CHECK(Raz::FloatUtils::areNearlyEqual(quat3.computeSquaredNorm(), 30.f));
  CHECK(Raz::FloatUtils::areNearlyEqual(quat3.computeNorm(), 5.47722578f));
}

TEST_CASE("Quaternion matrix computation") {
  CHECK(quat1.computeMatrix() == Raz::Mat4f({{ 1.f,           0.f,          0.f, 0.f },
                                             { 0.f,   0.98480773f, 0.173648179f, 0.f },
                                             { 0.f, -0.173648179f,  0.98480773f, 0.f },
                                             { 0.f,           0.f,          0.f, 1.f }}));

  CHECK(quat2.computeMatrix() == Raz::Mat4f({{ -0.870967627f,   0.193548396f,   0.45161289f, 0.f },
                                             {  -0.45161289f,  -0.677419245f, -0.580645144f, 0.f },
                                             {  0.193548396f,  -0.709677398f,  0.677419424f, 0.f },
                                             {           0.f,            0.f,           0.f, 1.f }}));

  CHECK(quat3.computeMatrix() == Raz::Mat4f({{  -0.933333397f, -0.133333355f,  0.333333343f, 0.f },
                                             {  -0.133333325f, -0.733333409f, -0.666666687f, 0.f },
                                             {   0.333333343f, -0.666666687f,  0.666666627f, 0.f },
                                             {            0.f,           0.f,           0.f, 1.f }}));
}
