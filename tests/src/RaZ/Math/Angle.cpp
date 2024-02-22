#include "Catch.hpp"

#include "RaZ/Math/Angle.hpp"

TEST_CASE("Degrees to Radians", "[math]") {
  constexpr Raz::Degreesf deg45(45.f);
  constexpr Raz::Degreesf deg90(90.f);
  constexpr Raz::Degreesf deg180(180.f);
  constexpr Raz::Degreesf deg360(360.f);

  CHECK_THAT(Raz::Radiansf(deg45).value,  IsNearlyEqualTo(Raz::Pi<float> / 4));
  CHECK_THAT(Raz::Radiansf(deg90).value,  IsNearlyEqualTo(Raz::Pi<float> / 2));
  CHECK_THAT(Raz::Radiansf(deg180).value, IsNearlyEqualTo(Raz::Pi<float>));
  CHECK_THAT(Raz::Radiansf(deg360).value, IsNearlyEqualTo(Raz::Pi<float> * 2));
}

TEST_CASE("Radians to Degrees", "[math]") {
  constexpr Raz::Radiansf radFourthPi(Raz::Pi<float> / 4);
  constexpr Raz::Radiansf radHalfPi(Raz::Pi<float> / 2);
  constexpr Raz::Radiansf radPi(Raz::Pi<float>);
  constexpr Raz::Radiansf radDoublePi(Raz::Pi<float> * 2);

  CHECK_THAT(Raz::Degreesf(radFourthPi).value, IsNearlyEqualTo(45.f));
  CHECK_THAT(Raz::Degreesf(radHalfPi).value,   IsNearlyEqualTo(90.f));
  CHECK_THAT(Raz::Degreesf(radPi).value,       IsNearlyEqualTo(180.f));
  CHECK_THAT(Raz::Degreesf(radDoublePi).value, IsNearlyEqualTo(360.f));
}

TEST_CASE("Angle operators", "[math]") {
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
  CHECK_THAT(radToDegFloat.value, IsNearlyEqualTo(180.f));

  constexpr Raz::Radiansf degToRadFloat = degTest;
  CHECK_THAT(degToRadFloat.value, IsNearlyEqualTo(Raz::Pi<float>));

  // Mathematical operators
  // Degrees
  auto degAdd = degTest + 2.f;
  // Checking that mathematical operations keeps the original angle type (no implicit cast to the value type)
  CHECK(std::is_same_v<std::decay_t<decltype(degTest)>, std::decay_t<decltype(degAdd)>>);
  CHECK_THAT(degAdd.value, IsNearlyEqualTo(182.0L));

  auto degSub = degTest - 2.f;
  CHECK(std::is_same_v<std::decay_t<decltype(degTest)>, std::decay_t<decltype(degSub)>>);
  CHECK_THAT(degSub.value, IsNearlyEqualTo(178.0L));

  auto degMul = degTest * 2.f;
  CHECK(std::is_same_v<std::decay_t<decltype(degTest)>, std::decay_t<decltype(degMul)>>);
  CHECK_THAT(degMul.value, IsNearlyEqualTo(360.0L));

  auto degDiv = degTest / 2.f;
  CHECK(std::is_same_v<std::decay_t<decltype(degTest)>, std::decay_t<decltype(degDiv)>>);
  CHECK_THAT(degDiv.value, IsNearlyEqualTo(90.0L));

  degAdd -= 2.f;
  CHECK(degAdd == degTest);
  degSub += 2.f;
  CHECK(degSub == degTest);
  degMul /= 2.f;
  CHECK(degMul == degTest);
  degDiv *= 2.f;
  CHECK(degDiv == degTest);

  // Radians
  auto radAdd = radTest + Raz::Pi<float>;
  // Checking that mathematical operations keeps the original angle type (no implicit cast to the value type)
  CHECK(std::is_same_v<std::decay_t<decltype(radTest)>, std::decay_t<decltype(radAdd)>>);
  // A float epsilon must be specified for the test to pass
  // long double's epsilon is too precise, so we need to compare with a value of the type we made an operation with
  CHECK_THAT(radAdd.value, IsNearlyEqualTo(Raz::Pi<long double> * 2, std::numeric_limits<float>::epsilon()));

  auto radSub = radTest - Raz::Pi<float>;
  CHECK(std::is_same_v<std::decay_t<decltype(radTest)>, std::decay_t<decltype(radSub)>>);
  CHECK_THAT(radSub.value, IsNearlyEqualTo(0.0L, std::numeric_limits<float>::epsilon()));

  auto radMul = radTest * 2.f;
  CHECK(std::is_same_v<std::decay_t<decltype(radTest)>, std::decay_t<decltype(radMul)>>);
  CHECK_THAT(radMul.value, IsNearlyEqualTo(Raz::Pi<long double> * 2, std::numeric_limits<float>::epsilon()));

  auto radDiv = radTest / 2.f;
  CHECK(std::is_same_v<std::decay_t<decltype(radTest)>, std::decay_t<decltype(radDiv)>>);
  CHECK_THAT(radDiv.value, IsNearlyEqualTo(Raz::Pi<long double> / 2, std::numeric_limits<float>::epsilon()));

  radAdd -= Raz::Pi<float>;
  CHECK(radAdd == radTest);

  radSub += Raz::Pi<float>;
  CHECK(radSub == radTest);

  radMul /= 2.f;
  CHECK(radMul == radTest);

  radDiv *= 2.f;
  CHECK(radDiv == radTest);
}
