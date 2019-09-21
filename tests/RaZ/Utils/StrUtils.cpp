#include "Catch.hpp"

#include "RaZ/Utils/StrUtils.hpp"

TEST_CASE("String case changes") {
  const std::string lowerStr = "this is a test";
  const std::string upperStr = "THIS IS A TEST";
  std::string str = lowerStr;

  CHECK(Raz::StrUtils::toUppercase(str) == upperStr); // Modified in-place, no copy occurring
  CHECK(str == upperStr); // String remains uppercase

  CHECK(Raz::StrUtils::toLowercaseCopy(str) == lowerStr); // String is copied, str is untouched
  CHECK(str == upperStr); // Unchanged, still uppercase
}

TEST_CASE("String trimming") {
  const std::string baseStr         = "   this is  a    test     ";
  const std::string trimmedRightStr = "   this is  a    test";
  const std::string trimmedLeftStr  = "this is  a    test     ";
  const std::string trimmedBothStr  = "this is  a    test";

  std::string str = baseStr;

  CHECK(Raz::StrUtils::trimLeft(str) == trimmedLeftStr); // Modified in-place, no copy occurring
  CHECK(str == trimmedLeftStr); // String is still in uppercase
  CHECK(Raz::StrUtils::trimRight(str) == trimmedBothStr); // Already trimmed left, trimming right makes it fully trimmed

  CHECK(Raz::StrUtils::trimLeftCopy(baseStr) == trimmedLeftStr);
  CHECK(Raz::StrUtils::trimRightCopy(baseStr) == trimmedRightStr);
  CHECK(Raz::StrUtils::trimCopy(baseStr) == trimmedBothStr); // Trim both ends
}

TEST_CASE("String splitting") {
  const std::string baseStr = "this'test  (is a)     good/test   /   ";

  const std::vector<std::string> spaceSplit = Raz::StrUtils::split(baseStr, ' ');

  CHECK(spaceSplit.size() == 5);
  CHECK(spaceSplit[0] == "this'test");
  CHECK(spaceSplit[1] == "(is");
  CHECK(spaceSplit[2] == "a)");
  CHECK(spaceSplit[3] == "good/test");
  CHECK(spaceSplit[4] == "/");

  const std::vector<std::string> slashSplit = Raz::StrUtils::split(baseStr, '/');

  CHECK(slashSplit.size() == 2);
  CHECK(slashSplit[0] == "this'test  (is a)     good");
  CHECK(slashSplit[1] == "test");
}
