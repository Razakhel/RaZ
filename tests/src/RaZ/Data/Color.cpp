#include "Catch.hpp"

#include "RaZ/Data/Color.hpp"

TEST_CASE("Color components", "[data]") {
  Raz::Color color;
  CHECK(color.red() == 0.f);
  CHECK(color.green() == 0.f);
  CHECK(color.blue() == 0.f);

  color.red()   = 1.f;
  color.green() = 2.f;
  color.blue()  = 3.f;
  CHECK(color.red() == 1.f);
  CHECK(color.green() == 2.f);
  CHECK(color.blue() == 3.f);
}

TEST_CASE("Color float/byte conversion", "[data]") {
  CHECK(Raz::Vec3b(Raz::Color(0.f, 0.f, 0.f)).strictlyEquals(Raz::Vec3b(0, 0, 0)));
  CHECK(Raz::Vec3f(Raz::Color(0, 0, 0)).strictlyEquals(Raz::Vec3f(0.f, 0.f, 0.f)));

  CHECK(Raz::Vec3b(Raz::Color(0.5f, 0.5f, 0.5f)) == Raz::Vec3b(127, 127, 127)); // Result is truncated; if rounded, it would be [128, 128, 128]
  CHECK(Raz::Vec3f(Raz::Color(127, 127, 127)) == Raz::Vec3f(0.4980392f, 0.4980392f, 0.4980392f));

  CHECK(Raz::Vec3b(Raz::Color(1.f, 1.f, 1.f)).strictlyEquals(Raz::Vec3b(255, 255, 255)));
  CHECK(Raz::Vec3f(Raz::Color(255, 255, 255)).strictlyEquals(Raz::Vec3f(1.f, 1.f, 1.f)));
}

TEST_CASE("Color hexadecimal", "[data]") {
  CHECK(Raz::Color(0x000000) == Raz::Color(0.f, 0.f, 0.f));
  CHECK(Raz::Color(0x808080) == Raz::Color(0.5019608f, 0.5019608f, 0.5019608f));
  CHECK(Raz::Color(0x00FFFF) == Raz::Color(0.f, 1.f, 1.f));
  CHECK(Raz::Color(0xFF00FF) == Raz::Color(1.f, 0.f, 1.f));
  CHECK(Raz::Color(0xFFFF00) == Raz::Color(1.f, 1.f, 0.f));
  CHECK(Raz::Color(0xFFFFFF) == Raz::Color(1.f, 1.f, 1.f));
}
