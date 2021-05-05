#include "Catch.hpp"

#include "RaZ/Math/PerlinNoise.hpp"

TEST_CASE("Perlin noise 1D") {
  CHECK(Raz::PerlinNoise::get(0.f) == 0.5f);
  CHECK(Raz::PerlinNoise::get(0.001f) == 0.49950001f);
  CHECK(Raz::PerlinNoise::get(0.01f) == 0.49499518f);
  CHECK(Raz::PerlinNoise::get(0.1f) == 0.446576f);

  CHECK(Raz::PerlinNoise::get(1.0123f) == 0.50615889f);
  CHECK(Raz::PerlinNoise::get(1.0123f) == 0.50615889f); // The same coordinate must always give the same value
}

TEST_CASE("Perlin noise 2D") {
  CHECK(Raz::PerlinNoise::get(0.f, 0.f) == 0.5f);
  CHECK(Raz::PerlinNoise::get(0.001f, 0.001f) == 0.49950001f);
  CHECK(Raz::PerlinNoise::get(0.01f, 0.01f) == 0.49500701f);
  CHECK(Raz::PerlinNoise::get(0.1f, 0.1f) == 0.45622027f);

  CHECK(Raz::PerlinNoise::get(1.0123f, 2.0123f) == 0.50613433f);
  CHECK(Raz::PerlinNoise::get(1.0123f, 2.0123f) == 0.50613433f); // The same coordinates must always give the same value
}
