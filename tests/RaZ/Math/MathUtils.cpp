#include "Catch.hpp"

#include "RaZ/Math/MathUtils.hpp"

TEST_CASE("MathUtils linear interpolation") {
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
}

TEST_CASE("MathUtils smoothstep value") {
  CHECK(Raz::MathUtils::smoothstep(0.f) == 0.f);
  CHECK(Raz::MathUtils::smoothstep(0.25f) == 0.15625f);
  CHECK(Raz::MathUtils::smoothstep(0.5f) == 0.5f);
  CHECK(Raz::MathUtils::smoothstep(0.75f) == 0.84375f);
  CHECK(Raz::MathUtils::smoothstep(1.f) == 1.f);
}

TEST_CASE("MathUtils smoothstep range") {
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

TEST_CASE("MathUtils smootherstep value") {
  CHECK(Raz::MathUtils::smootherstep(0.f) == 0.f);
  CHECK(Raz::MathUtils::smootherstep(0.25f) == 0.103515625f);
  CHECK(Raz::MathUtils::smootherstep(0.5f) == 0.5f);
  CHECK(Raz::MathUtils::smootherstep(0.75f) == 0.89648438f);
  CHECK(Raz::MathUtils::smootherstep(1.f) == 1.f);
}

TEST_CASE("MathUtils smootherstep range") {
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
