#include "Catch.hpp"

#include "RaZ/Utils/StrUtils.hpp"

TEST_CASE("String starts/ends with") {
  {
    const std::string str = "this is a test";

    CHECK(Raz::StrUtils::startsWith(str, "this"));
    CHECK(Raz::StrUtils::startsWith(str, 't'));
    CHECK_FALSE(Raz::StrUtils::startsWith(str, "his"));
    CHECK_FALSE(Raz::StrUtils::startsWith(str, "test"));

    CHECK_FALSE(Raz::StrUtils::endsWith(str, "this"));
    CHECK(Raz::StrUtils::endsWith(str, "test"));
    CHECK(Raz::StrUtils::endsWith(str, 't'));
    CHECK(Raz::StrUtils::endsWith(str, "est"));
  }

  {
    const std::wstring str = L"this is a test";

    CHECK(Raz::StrUtils::startsWith(str, L"this"));
    CHECK(Raz::StrUtils::startsWith(str, L't'));
    CHECK_FALSE(Raz::StrUtils::startsWith(str, L"his"));
    CHECK_FALSE(Raz::StrUtils::startsWith(str, L"test"));

    CHECK_FALSE(Raz::StrUtils::endsWith(str, L"this"));
    CHECK(Raz::StrUtils::endsWith(str, L"test"));
    CHECK(Raz::StrUtils::endsWith(str, L't'));
    CHECK(Raz::StrUtils::endsWith(str, L"est"));
  }
}

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
  CHECK(str == trimmedLeftStr);
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
