#include "catch/catch.hpp"
#include "RaZ/Math/Angle.hpp"

TEST_CASE("Degrees to Radians") {
  constexpr Raz::Degreesf deg45(45.f);
  constexpr Raz::Degreesf deg90(90.f);
  constexpr Raz::Degreesf deg180(180.f);
  constexpr Raz::Degreesf deg360(360.f);

  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Radiansf(deg45).value, Raz::PI<float> / 4));
  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Radiansf(deg90).value, Raz::PI<float> / 2));
  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Radiansf(deg180).value, Raz::PI<float>));
  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Radiansf(deg360).value, Raz::PI<float> * 2));
}

TEST_CASE("Radians to Degrees") {
  constexpr Raz::Radiansf radFourthPi(Raz::PI<float> / 4);
  constexpr Raz::Radiansf radHalfPi(Raz::PI<float> / 2);
  constexpr Raz::Radiansf radPi(Raz::PI<float>);
  constexpr Raz::Radiansf radDoublePi(Raz::PI<float> * 2);

  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Degreesf(radFourthPi).value, 45.f));
  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Degreesf(radHalfPi).value, 90.f));
  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Degreesf(radPi).value, 180.f));
  CHECK(Raz::FloatUtils::areNearlyEqual(Raz::Degreesf(radDoublePi).value, 360.f));
}

TEST_CASE("Angle operators") {
  // Literal operators
  using namespace Raz::Literals;

  constexpr auto degTest = 180.0_deg;
  CHECK(std::is_same_v<Raz::Degreesld, std::decay_t<decltype(degTest)>>);

  constexpr auto radTest = 3.14159265358979323846_rad;
  CHECK(std::is_same_v<Raz::Radiansld, std::decay_t<decltype(radTest)>>);

  // Type conversion operators
  // To check conversion, verifying that values are (nearly) the same before & after conversion; in this example, there won't be much difference
  // Degrees' & Radians' (un)equality operators automatically verify near-equality
  constexpr Raz::Degreesf degFloat = degTest;
  CHECK(degFloat == degTest);
  constexpr Raz::Degreesd degDouble = degTest;
  CHECK(degDouble == degTest);

  constexpr Raz::Radiansf radFloat = radTest;
  CHECK(radFloat == radTest);
  constexpr Raz::Radiansd radDouble = radTest;
  CHECK(radDouble == radTest);

  // Checking that conversion between degrees & radians with different data types works as expected
  constexpr Raz::Degreesf radToDegFloat = radTest;
  CHECK(Raz::FloatUtils::areNearlyEqual(radToDegFloat.value, 180.f));

  constexpr Raz::Radiansf degToRadFloat = degTest;
  CHECK(Raz::FloatUtils::areNearlyEqual(degToRadFloat.value, Raz::PI<float>));
}
