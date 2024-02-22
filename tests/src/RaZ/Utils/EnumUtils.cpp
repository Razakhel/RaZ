#include "Catch.hpp"

#include "RaZ/Utils/EnumUtils.hpp"

enum class EnumTest : unsigned int {
  ONE   = 1,
  TWO   = 2,
  FOUR  = 4,
  EIGHT = 8,
  ALL   = ONE | TWO | FOUR | EIGHT // 15
};

MAKE_ENUM_FLAG(EnumTest)

TEST_CASE("EnumUtils make enum flag", "[utils]") {
  constexpr EnumTest zero   = EnumTest::ONE & EnumTest::TWO;
  constexpr EnumTest three  = EnumTest::ONE | EnumTest::TWO;
  constexpr EnumTest one    = three & EnumTest::ONE;
  constexpr EnumTest twelve = EnumTest::FOUR ^ EnumTest::EIGHT;

  CHECK(static_cast<unsigned int>(zero) == 0);
  CHECK(static_cast<unsigned int>(three) == 3);
  CHECK(static_cast<unsigned int>(one) == 1);
  CHECK(static_cast<unsigned int>(twelve) == 12);

  EnumTest five = EnumTest::FOUR;
  five |= EnumTest::ONE;
  CHECK(static_cast<unsigned int>(five) == 5);

  EnumTest four = five;
  four &= EnumTest::FOUR;
  CHECK(static_cast<unsigned int>(four) == 4);

  EnumTest six = EnumTest::TWO;
  six ^= EnumTest::FOUR;
  CHECK(static_cast<unsigned int>(six) == 6);

  CHECK(static_cast<unsigned int>(EnumTest::ALL & EnumTest::ONE) == 1);
  CHECK(static_cast<unsigned int>(EnumTest::ALL & EnumTest::TWO) == 2);
  CHECK(static_cast<unsigned int>(EnumTest::ALL & EnumTest::FOUR) == 4);
  CHECK(static_cast<unsigned int>(EnumTest::ALL & EnumTest::EIGHT) == 8);

  CHECK(static_cast<unsigned int>(EnumTest::ALL | EnumTest::ONE) == 15);
  CHECK(static_cast<unsigned int>(EnumTest::ALL | EnumTest::TWO) == 15);
  CHECK(static_cast<unsigned int>(EnumTest::ALL | EnumTest::FOUR) == 15);
  CHECK(static_cast<unsigned int>(EnumTest::ALL | EnumTest::EIGHT) == 15);

  CHECK(static_cast<unsigned int>(EnumTest::ALL ^ EnumTest::ONE) == 14);
  CHECK(static_cast<unsigned int>(EnumTest::ALL ^ EnumTest::TWO) == 13);
  CHECK(static_cast<unsigned int>(EnumTest::ALL ^ EnumTest::FOUR) == 11);
  CHECK(static_cast<unsigned int>(EnumTest::ALL ^ EnumTest::EIGHT) == 7);
}
