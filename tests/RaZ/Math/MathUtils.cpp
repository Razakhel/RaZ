#include "Catch.hpp"

#include "RaZ/Math/MathUtils.hpp"

TEST_CASE("MathUtils linear interpolation") {
  CHECK(Raz::MathUtils::interpolate(0.f, 1.f, 0.f) == 0.f);
  CHECK(Raz::MathUtils::interpolate(0.f, 1.f, 0.25f) == 0.25f);
  CHECK(Raz::MathUtils::interpolate(0.f, 1.f, 0.5f) == 0.5f);
  CHECK(Raz::MathUtils::interpolate(0.f, 1.f, 0.75f) == 0.75f);
  CHECK(Raz::MathUtils::interpolate(0.f, 1.f, 1.f) == 1.f);

  CHECK(Raz::MathUtils::interpolate(-5.f, 5.f, 0.f) == -5.f);
  CHECK(Raz::MathUtils::interpolate(-5.f, 5.f, 0.25f) == -2.5f);
  CHECK(Raz::MathUtils::interpolate(-5.f, 5.f, 0.5f) == 0.f);
  CHECK(Raz::MathUtils::interpolate(-5.f, 5.f, 0.75f) == 2.5f);
  CHECK(Raz::MathUtils::interpolate(-5.f, 5.f, 1.f) == 5.f);

  CHECK(Raz::MathUtils::interpolate(-1.43f, 8.12f, 0.f) == -1.43f);
  CHECK(Raz::MathUtils::interpolate(-1.43f, 8.12f, 0.25f) == 0.9575f);
  CHECK(Raz::MathUtils::interpolate(-1.43f, 8.12f, 0.5f) == 3.345f);
  CHECK(Raz::MathUtils::interpolate(-1.43f, 8.12f, 0.75f) == 5.7325001f);
  CHECK(Raz::MathUtils::interpolate(-1.43f, 8.12f, 1.f) == 8.12f);
}
