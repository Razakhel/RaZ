#include "RaZ/Math/MathUtils.hpp"
#include "RaZ/Math/Vector.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MathUtils lerp arithmetic types", "[math]") {
  CHECK(Raz::MathUtils::lerp(0.f, 1.f, 0.f) == 0.f);
  CHECK(Raz::MathUtils::lerp(0.f, 1.f, 0.25f) == 0.25f);
  CHECK(Raz::MathUtils::lerp(0.f, 1.f, 0.5f) == 0.5f);
  CHECK(Raz::MathUtils::lerp(0.f, 1.f, 0.75f) == 0.75f);
  CHECK(Raz::MathUtils::lerp(0.f, 1.f, 1.f) == 1.f);

  CHECK(Raz::MathUtils::lerp(-5.f, 5.f, 0.f) == -5.f);
  CHECK(Raz::MathUtils::lerp(-5.f, 5.f, 0.25f) == -2.5f);
  CHECK(Raz::MathUtils::lerp(-5.f, 5.f, 0.5f) == 0.f);
  CHECK(Raz::MathUtils::lerp(-5.f, 5.f, 0.75f) == 2.5f);
  CHECK(Raz::MathUtils::lerp(-5.f, 5.f, 1.f) == 5.f);

  CHECK(Raz::MathUtils::lerp(-1.43f, 8.12f, 0.f) == -1.43f);
  CHECK(Raz::MathUtils::lerp(-1.43f, 8.12f, 0.25f) == 0.9575f);
  CHECK(Raz::MathUtils::lerp(-1.43f, 8.12f, 0.5f) == 3.345f);
  CHECK(Raz::MathUtils::lerp(-1.43f, 8.12f, 0.75f) == 5.7325001f);
  CHECK(Raz::MathUtils::lerp(-1.43f, 8.12f, 1.f) == 8.12f);

  CHECK(Raz::MathUtils::lerp(457, 5324, 0.f) == 457);
  CHECK(Raz::MathUtils::lerp(457, 5324, 0.25f) == 1673);
  CHECK(Raz::MathUtils::lerp(457, 5324, 0.5f) == 2890);
  CHECK(Raz::MathUtils::lerp(457, 5324, 0.75f) == 4107);
  CHECK(Raz::MathUtils::lerp(457, 5324, 1.f) == 5324);
}

TEST_CASE("MathUtils lerp vector", "[math]") {
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(0.f), Raz::Vec3f(1.f), 0.f) == Raz::Vec3f(0.f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(0.f), Raz::Vec3f(1.f), 0.25f) == Raz::Vec3f(0.25f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(0.f), Raz::Vec3f(1.f), 0.5f) == Raz::Vec3f(0.5f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(0.f), Raz::Vec3f(1.f), 0.75f) == Raz::Vec3f(0.75f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(0.f), Raz::Vec3f(1.f), 1.f) == Raz::Vec3f(1.f));

  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(1.f, 2.f, 3.f), Raz::Vec3f(3.f, 2.f, 1.f), 0.f) == Raz::Vec3f(1.f, 2.f, 3.f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(1.f, 2.f, 3.f), Raz::Vec3f(3.f, 2.f, 1.f), 0.25f) == Raz::Vec3f(1.5f, 2.f, 2.5f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(1.f, 2.f, 3.f), Raz::Vec3f(3.f, 2.f, 1.f), 0.5f) == Raz::Vec3f(2.f, 2.f, 2.f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(1.f, 2.f, 3.f), Raz::Vec3f(3.f, 2.f, 1.f), 0.75f) == Raz::Vec3f(2.5f, 2.f, 1.5f));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3f(1.f, 2.f, 3.f), Raz::Vec3f(3.f, 2.f, 1.f), 1.f) == Raz::Vec3f(3.f, 2.f, 1.f));

  CHECK(Raz::MathUtils::lerp(Raz::Vec3i(140, 874, 3541), Raz::Vec3i(841, 53, 4712), 0.f) == Raz::Vec3i(140, 874, 3541));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3i(140, 874, 3541), Raz::Vec3i(841, 53, 4712), 0.25f) == Raz::Vec3i(315, 668, 3833));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3i(140, 874, 3541), Raz::Vec3i(841, 53, 4712), 0.5f) == Raz::Vec3i(490, 463, 4126));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3i(140, 874, 3541), Raz::Vec3i(841, 53, 4712), 0.75f) == Raz::Vec3i(665, 258, 4419));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3i(140, 874, 3541), Raz::Vec3i(841, 53, 4712), 1.f) == Raz::Vec3i(841, 53, 4712));

  CHECK(Raz::MathUtils::lerp(Raz::Vec3b(255, 140, 27), Raz::Vec3b(75, 241, 173), 0.f) == Raz::Vec3b(255, 140, 27));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3b(255, 140, 27), Raz::Vec3b(75, 241, 173), 0.25f) == Raz::Vec3b(210, 165, 63));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3b(255, 140, 27), Raz::Vec3b(75, 241, 173), 0.5f) == Raz::Vec3b(165, 190, 100));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3b(255, 140, 27), Raz::Vec3b(75, 241, 173), 0.75f) == Raz::Vec3b(120, 215, 136));
  CHECK(Raz::MathUtils::lerp(Raz::Vec3b(255, 140, 27), Raz::Vec3b(75, 241, 173), 1.f) == Raz::Vec3b(75, 241, 173));
}

TEST_CASE("MathUtils smoothstep value", "[math]") {
  CHECK(Raz::MathUtils::smoothstep(0.f) == 0.f);
  CHECK(Raz::MathUtils::smoothstep(0.25f) == 0.15625f);
  CHECK(Raz::MathUtils::smoothstep(0.5f) == 0.5f);
  CHECK(Raz::MathUtils::smoothstep(0.75f) == 0.84375f);
  CHECK(Raz::MathUtils::smoothstep(1.f) == 1.f);
}

TEST_CASE("MathUtils smoothstep range", "[math]") {
  CHECK(Raz::MathUtils::smoothstep(0.f, 1.f, -1.f) == 0.f); // If value < minThresh, returns 0
  CHECK(Raz::MathUtils::smoothstep(0.f, 1.f, 0.f) == 0.f);
  CHECK(Raz::MathUtils::smoothstep(0.f, 1.f, 0.25f) == 0.15625f);
  CHECK(Raz::MathUtils::smoothstep(0.f, 1.f, 0.5f) == 0.5f);
  CHECK(Raz::MathUtils::smoothstep(0.f, 1.f, 0.75f) == 0.84375f);
  CHECK(Raz::MathUtils::smoothstep(0.f, 1.f, 1.f) == 1.f);
  CHECK(Raz::MathUtils::smoothstep(0.f, 1.f, 2.f) == 1.f); // If value > maxThresh, returns 1

  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, -10.f) == 0.f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, -5.f - std::numeric_limits<float>::epsilon()) == 0.f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, -5.f) == 0.f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, -4.5f) == 0.00725000072f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, -2.5f) == 0.15625f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, -0.5f) == 0.42524996f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, 0.f) == 0.5f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, 0.5f) == 0.57475001f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, 2.5f) == 0.84375f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, 4.5f) == 0.99274999f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, 5.f) == 1.f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, 5.f + std::numeric_limits<float>::epsilon()) == 1.f);
  CHECK(Raz::MathUtils::smoothstep(-5.f, 5.f, 10.f) == 1.f);
}

TEST_CASE("MathUtils smootherstep value", "[math]") {
  CHECK(Raz::MathUtils::smootherstep(0.f) == 0.f);
  CHECK(Raz::MathUtils::smootherstep(0.25f) == 0.103515625f);
  CHECK(Raz::MathUtils::smootherstep(0.5f) == 0.5f);
  CHECK(Raz::MathUtils::smootherstep(0.75f) == 0.89648438f);
  CHECK(Raz::MathUtils::smootherstep(1.f) == 1.f);
}

TEST_CASE("MathUtils smootherstep range", "[math]") {
  CHECK(Raz::MathUtils::smootherstep(0.f, 1.f, -1.f) == 0.f); // If value < minThresh, returns 0
  CHECK(Raz::MathUtils::smootherstep(0.f, 1.f, 0.f) == 0.f);
  CHECK(Raz::MathUtils::smootherstep(0.f, 1.f, 0.25f) == 0.103515625f);
  CHECK(Raz::MathUtils::smootherstep(0.f, 1.f, 0.5f) == 0.5f);
  CHECK(Raz::MathUtils::smootherstep(0.f, 1.f, 0.75f) == 0.89648438f);
  CHECK(Raz::MathUtils::smootherstep(0.f, 1.f, 1.f) == 1.f);
  CHECK(Raz::MathUtils::smootherstep(0.f, 1.f, 2.f) == 1.f); // If value > maxThresh, returns 1

  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, -10.f) == 0.f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, -5.f - std::numeric_limits<float>::epsilon()) == 0.f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, -5.f) == 0.f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, -4.5f) == 0.0011581251f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, -2.5f) == 0.103515625f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, -0.5f) == 0.40687308f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, 0.f) == 0.5f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, 0.5f) == 0.59312695f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, 2.5f) == 0.89648438f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, 4.5f) == 0.99884182f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, 5.f) == 1.f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, 5.f + std::numeric_limits<float>::epsilon()) == 1.f);
  CHECK(Raz::MathUtils::smootherstep(-5.f, 5.f, 10.f) == 1.f);
}

TEST_CASE("MathUtils orthonormal basis", "[math]") {
  Raz::Vec3f axis2;
  Raz::Vec3f axis3;

  Raz::MathUtils::computeOrthonormalBasis(Raz::Axis::X, axis2, axis3);
  CHECK(axis2 == -Raz::Axis::Z);
  CHECK(axis3 == Raz::Axis::Y);

  Raz::MathUtils::computeOrthonormalBasis(Raz::Axis::Y, axis2, axis3);
  CHECK(axis2 == Raz::Axis::X);
  CHECK(axis3 == -Raz::Axis::Z);

  Raz::MathUtils::computeOrthonormalBasis(Raz::Axis::Z, axis2, axis3);
  CHECK(axis2 == Raz::Axis::X);
  CHECK(axis3 == Raz::Axis::Y);

  Raz::MathUtils::computeOrthonormalBasis(Raz::Vec3f(1.f).normalize(), axis2, axis3);
  CHECK(axis2 == Raz::Vec3f(0.788675f, -0.211324856f, -0.577350259f));
  CHECK(axis3 == Raz::Vec3f(-0.211324856f, 0.788675f, -0.577350259f));
  CHECK(axis2.dot(axis3) == 0.f);

  Raz::MathUtils::computeOrthonormalBasis(Raz::Vec3f(-1.f).normalize(), axis2, axis3);
  CHECK(axis2 == Raz::Vec3f(0.788675f, -0.211324856f, -0.577350259f));
  CHECK(axis3 == Raz::Vec3f(0.211324856f, -0.788675f, 0.577350259f));
  CHECK(axis2.dot(axis3) == 0.f);
}

TEST_CASE("MathUtils Fibonacci sphere", "[math]") {
  std::vector<Raz::Vec3f> fiboPoints = Raz::MathUtils::computeFibonacciSpherePoints(0);
  CHECK(fiboPoints.empty());

  fiboPoints = Raz::MathUtils::computeFibonacciSpherePoints(1);
  REQUIRE(fiboPoints.size() == 1);
  CHECK_THAT(fiboPoints[0], IsNearlyEqualToVector(Raz::Vec3f(1.f, 0.f, 0.f)));

  fiboPoints = Raz::MathUtils::computeFibonacciSpherePoints(2);
  REQUIRE(fiboPoints.size() == 2);
  CHECK_THAT(fiboPoints[0], IsNearlyEqualToVector(Raz::Vec3f(0.8660253f, 0.5f, 0.f)));
  CHECK_THAT(fiboPoints[1], IsNearlyEqualToVector(Raz::Vec3f(-0.6385802f, -0.5, -0.5849917f)));

  fiboPoints = Raz::MathUtils::computeFibonacciSpherePoints(3);
  REQUIRE(fiboPoints.size() == 3);
  CHECK_THAT(fiboPoints[0], IsNearlyEqualToVector(Raz::Vec3f(0.745356f, 0.6666666f, 0.f)));
  CHECK_THAT(fiboPoints[1], IsNearlyEqualToVector(Raz::Vec3f(-0.7373688f, 0.f, -0.6754903f)));
  CHECK_THAT(fiboPoints[2], IsNearlyEqualToVector(Raz::Vec3f(0.0651632f, -0.6666666f, 0.742502f)));

  fiboPoints = Raz::MathUtils::computeFibonacciSpherePoints(4);
  REQUIRE(fiboPoints.size() == 4);
  CHECK_THAT(fiboPoints[0], IsNearlyEqualToVector(Raz::Vec3f(0.6614378f, 0.75f, 0.f)));
  CHECK_THAT(fiboPoints[1], IsNearlyEqualToVector(Raz::Vec3f(-0.7139543f, 0.25f, -0.6540406f)));
  CHECK_THAT(fiboPoints[2], IsNearlyEqualToVector(Raz::Vec3f(0.0846495f, -0.25f, 0.9645385f)));
  CHECK_THAT(fiboPoints[3], IsNearlyEqualToVector(Raz::Vec3f(0.4024442f, -0.75f, -0.5249176f)));

  fiboPoints = Raz::MathUtils::computeFibonacciSpherePoints(5);
  REQUIRE(fiboPoints.size() == 5);
  CHECK_THAT(fiboPoints[0], IsNearlyEqualToVector(Raz::Vec3f(0.6f, 0.8f, 0.f)));
  CHECK_THAT(fiboPoints[1], IsNearlyEqualToVector(Raz::Vec3f(-0.6758097f, 0.3999999f, -0.619097f)));
  CHECK_THAT(fiboPoints[2], IsNearlyEqualToVector(Raz::Vec3f(0.0874256f, 0.f, 0.996171f)));
  CHECK_THAT(fiboPoints[3], IsNearlyEqualToVector(Raz::Vec3f(0.5576431f, -0.3999999f, -0.7273473f)));
  CHECK_THAT(fiboPoints[4], IsNearlyEqualToVector(Raz::Vec3f(-0.5908281f, -0.7999999f, 0.104509f)));
}
