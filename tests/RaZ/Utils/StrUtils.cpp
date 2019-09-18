#include "Catch.hpp"

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

TEST_CASE("String splitting") {
  const std::string baseStr = "this'test  (is a)     good/test   /   ";

  const std::vector<std::string> spaceSplit = Raz::StrUtils::split(baseStr, ' ');

  REQUIRE(spaceSplit.size() == 5);
  REQUIRE(spaceSplit[0] == "this'test");
  REQUIRE(spaceSplit[1] == "(is");
  REQUIRE(spaceSplit[2] == "a)");
  REQUIRE(spaceSplit[3] == "good/test");
  REQUIRE(spaceSplit[4] == "/");

  const std::vector<std::string> slashSplit = Raz::StrUtils::split(baseStr, '/');

  REQUIRE(slashSplit.size() == 2);
  REQUIRE(slashSplit[0] == "this'test  (is a)     good");
  REQUIRE(slashSplit[1] == "test");
}
