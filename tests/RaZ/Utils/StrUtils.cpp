#include "catch/catch.hpp"
#include "RaZ/Utils/StrUtils.hpp"

TEST_CASE("String case changes") {
  const std::string lowerStr = "this is a test";
  const std::string upperStr = "THIS IS A TEST";
  std::string str = lowerStr;

  REQUIRE(Raz::StrUtils::toUppercase(str) == upperStr); // Modified in-place, no copy occurring
  REQUIRE(str == upperStr); // String remains uppercase

  REQUIRE(Raz::StrUtils::toLowercaseCopy(str) == lowerStr); // String is copied, str is untouched
  REQUIRE(str == upperStr); // Unchanged, still uppercase
}

TEST_CASE("String trimming") {
  const std::string baseStr         = "   this is  a    test     ";
  const std::string trimmedRightStr = "   this is  a    test";
  const std::string trimmedLeftStr  = "this is  a    test     ";
  const std::string trimmedBothStr  = "this is  a    test";

  std::string str = baseStr;

  REQUIRE(Raz::StrUtils::trimLeft(str) == trimmedLeftStr); // Modified in-place, no copy occurring
  REQUIRE(str == trimmedLeftStr); // String is still in uppercase
  REQUIRE(Raz::StrUtils::trimRight(str) == trimmedBothStr); // Already trimmed left, trimming right makes it fully trimmed

  REQUIRE(Raz::StrUtils::trimLeftCopy(baseStr) == trimmedLeftStr);
  REQUIRE(Raz::StrUtils::trimRightCopy(baseStr) == trimmedRightStr);
  REQUIRE(Raz::StrUtils::trimCopy(baseStr) == trimmedBothStr); // Trim both ends
}
