#include "Catch.hpp"

#include "RaZ/Math/PerlinNoise.hpp"

TEST_CASE("Perlin noise 1D") {
  // Whole coordinates always give 0
  CHECK(Raz::PerlinNoise::get1D(0.f) == 0.f);
  CHECK(Raz::PerlinNoise::get1D(1.f) == 0.f);

  // The same coordinate must always give the same value
  CHECK(Raz::PerlinNoise::get1D(1.0123f) == 0.01231783256f);
  CHECK(Raz::PerlinNoise::get1D(1.0123f) == 0.01231783256f);

  // Changing the octaves

  CHECK(Raz::PerlinNoise::get1D(0.001f) == -0.0010000101f);
  CHECK(Raz::PerlinNoise::get1D(0.01f) == -0.010009653f);
  CHECK(Raz::PerlinNoise::get1D(0.1f) == -0.106848f);

  CHECK(Raz::PerlinNoise::get1D(0.001f, 2) == -0.0020000497f);
  CHECK(Raz::PerlinNoise::get1D(0.01f, 2) == -0.02004691f);
  CHECK(Raz::PerlinNoise::get1D(0.1f, 2) == -0.224224f);

  CHECK(Raz::PerlinNoise::get1D(0.001f, 4) == -0.0040008295f);
  CHECK(Raz::PerlinNoise::get1D(0.01f, 4) == -0.040660575f);
  CHECK(Raz::PerlinNoise::get1D(0.1f, 4) == -0.36943999f);

  CHECK(Raz::PerlinNoise::get1D(0.001f, 8) == -0.0081444457f);
  CHECK(Raz::PerlinNoise::get1D(0.01f, 8) == -0.06821575f);
  CHECK(Raz::PerlinNoise::get1D(0.1f, 8) == -0.34726396f);

  // Big coordinates also give relevant results
  CHECK(Raz::PerlinNoise::get1D(1026.1134f, 8) == 0.020990238f);
  CHECK(Raz::PerlinNoise::get1D(3721.846f, 8) == -0.027749581f);

  // The value can be normalized between [0; 1]
  CHECK(Raz::PerlinNoise::get1D(1026.1134f, 8, true) == 0.51049513f);
  CHECK(Raz::PerlinNoise::get1D(3721.846f, 8, true) == 0.4861252f);
}

TEST_CASE("Perlin noise 2D") {
  // Whole coordinates always give 0
  CHECK(Raz::PerlinNoise::get2D(0.f, 0.f) == 0.f);
  CHECK(Raz::PerlinNoise::get2D(1.f, 1.f) == 0.f);

  // The same coordinates must always give the same value
  CHECK(Raz::PerlinNoise::get2D(1.0123f, 2.0123f) == 0.0122686075f);
  CHECK(Raz::PerlinNoise::get2D(1.0123f, 2.0123f) == 0.0122686075f);

  // Changing the octaves

  CHECK(Raz::PerlinNoise::get2D(0.001f, 0.001f) == -0.00099998584f);
  CHECK(Raz::PerlinNoise::get2D(0.01f, 0.01f) == -0.0099860113f);
  CHECK(Raz::PerlinNoise::get2D(0.1f, 0.1f) == -0.087559439f);

  CHECK(Raz::PerlinNoise::get2D(0.001f, 0.001f, 2) == -0.0019999295f);
  CHECK(Raz::PerlinNoise::get2D(0.01f, 0.01f, 2) == -0.019930679f);
  CHECK(Raz::PerlinNoise::get2D(0.1f, 0.1f, 2) == -0.14678603f);

  CHECK(Raz::PerlinNoise::get2D(0.001f, 0.001f, 4) == -0.0039988072f);
  CHECK(Raz::PerlinNoise::get2D(0.01f, 0.01f, 4) == -0.038893871f);
  CHECK(Raz::PerlinNoise::get2D(0.1f, 0.1f, 4) == -0.18312122f);

  CHECK(Raz::PerlinNoise::get2D(0.001f, 0.001f, 8) == -0.0077331755f);
  CHECK(Raz::PerlinNoise::get2D(0.01f, 0.01f, 8) == -0.04978792f);
  CHECK(Raz::PerlinNoise::get2D(0.1f, 0.1f, 8) == -0.19044922f);

  // Big coordinates also give relevant results
  CHECK(Raz::PerlinNoise::get2D(1026.1134f, 1026.1134f, 8) == -0.14166377f);
  CHECK(Raz::PerlinNoise::get2D(3721.846f, 3721.846f, 8) == 0.16130497f);

  // The value can be normalized between [0; 1]
  CHECK(Raz::PerlinNoise::get2D(1026.1134f, 1026.1134f, 8, true) == 0.42916811f);
  CHECK(Raz::PerlinNoise::get2D(3721.846f, 3721.846f, 8, true) == 0.58065248f);
}
