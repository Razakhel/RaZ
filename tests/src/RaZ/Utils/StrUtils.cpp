#include "Catch.hpp"

#include "RaZ/Utils/StrUtils.hpp"

TEST_CASE("String starts/ends with", "[utils]") {
  // Standard strings
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

  // Wide strings
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

TEST_CASE("String case change", "[utils]") {
  // Standard strings
  {
    const std::string lowerSimpleStr  = "this is a test";
    const std::string upperSimpleStr  = "THIS IS A TEST";
    std::string str = lowerSimpleStr;

    CHECK(Raz::StrUtils::toUppercase(str) == upperSimpleStr); // Modified in-place, no copy occurring
    CHECK(str == upperSimpleStr); // String remains uppercase

    CHECK(Raz::StrUtils::toLowercaseCopy(str) == lowerSimpleStr); // String is copied, str is untouched
    CHECK(str == upperSimpleStr); // Unchanged, still uppercase

    const std::string lowerSpecialStr = "ͳhîs ís ä †èsτ";
    const std::string upperSpecialStr = "ͳHîS íS ä †èSτ";
    str = lowerSpecialStr;

    CHECK(Raz::StrUtils::toUppercase(str) == upperSpecialStr);
    CHECK(str == upperSpecialStr);

    CHECK(Raz::StrUtils::toLowercaseCopy(str) == lowerSpecialStr);
    CHECK(str == upperSpecialStr);
  }

  // Wide strings
  {
    const std::wstring lowerSimpleStr  = L"this is a test";
    const std::wstring upperSimpleStr  = L"THIS IS A TEST";
    std::wstring str = lowerSimpleStr;

    CHECK(Raz::StrUtils::toUppercase(str) == upperSimpleStr); // Modified in-place, no copy occurring
    CHECK(str == upperSimpleStr); // String remains uppercase

    CHECK(Raz::StrUtils::toLowercaseCopy(str) == lowerSimpleStr); // String is copied, str is untouched
    CHECK(str == upperSimpleStr); // Unchanged, still uppercase

    const std::wstring lowerSpecialStr = L"ͳhîs ís ä †èsτ";
    const std::wstring upperSpecialStr = L"ͳHîS íS ä †èSτ";
    str = lowerSpecialStr;

    CHECK(Raz::StrUtils::toUppercase(str) == upperSpecialStr);
    CHECK(str == upperSpecialStr);

    CHECK(Raz::StrUtils::toLowercaseCopy(str) == lowerSpecialStr);
    CHECK(str == upperSpecialStr);
  }
}

TEST_CASE("String trimming", "[utils]") {
  // Standard strings
  {
    const std::string baseStr         = "   ͳhîs ís  ä    †èsτ     ";
    const std::string trimmedRightStr = "   ͳhîs ís  ä    †èsτ";
    const std::string trimmedLeftStr  = "ͳhîs ís  ä    †èsτ     ";
    const std::string trimmedBothStr  = "ͳhîs ís  ä    †èsτ";

    std::string str = baseStr;

    CHECK(Raz::StrUtils::trimLeft(str) == trimmedLeftStr); // Modified in-place, no copy occurring
    CHECK(str == trimmedLeftStr);
    CHECK(Raz::StrUtils::trimRight(str) == trimmedBothStr); // Already trimmed left, trimming right makes it fully trimmed

    CHECK(Raz::StrUtils::trimLeftCopy(baseStr) == trimmedLeftStr);
    CHECK(Raz::StrUtils::trimRightCopy(baseStr) == trimmedRightStr);
    CHECK(Raz::StrUtils::trimCopy(baseStr) == trimmedBothStr); // Trim both ends
  }

  // Wide strings
  {
    const std::wstring baseStr         = L"   ͳhîs ís  ä    †èsτ     ";
    const std::wstring trimmedRightStr = L"   ͳhîs ís  ä    †èsτ";
    const std::wstring trimmedLeftStr  = L"ͳhîs ís  ä    †èsτ     ";
    const std::wstring trimmedBothStr  = L"ͳhîs ís  ä    †èsτ";

    std::wstring str = baseStr;

    CHECK(Raz::StrUtils::trimLeft(str) == trimmedLeftStr); // Modified in-place, no copy occurring
    CHECK(str == trimmedLeftStr);
    CHECK(Raz::StrUtils::trimRight(str) == trimmedBothStr); // Already trimmed left, trimming right makes it fully trimmed

    CHECK(Raz::StrUtils::trimLeftCopy(baseStr) == trimmedLeftStr);
    CHECK(Raz::StrUtils::trimRightCopy(baseStr) == trimmedRightStr);
    CHECK(Raz::StrUtils::trimCopy(baseStr) == trimmedBothStr); // Trim both ends
  }
}

TEST_CASE("String splitting", "[utils]") {
  // Standard strings
  {
    const std::string baseStr = "ͳhîs'†èsτ  (ís ä)     gøθd/†èsτ   /   ";

    const std::vector<std::string> spaceSplit = Raz::StrUtils::split(baseStr, ' ');

    CHECK(spaceSplit.size() == 5);
    CHECK(spaceSplit[0] == "ͳhîs'†èsτ");
    CHECK(spaceSplit[1] == "(ís");
    CHECK(spaceSplit[2] == "ä)");
    CHECK(spaceSplit[3] == "gøθd/†èsτ");
    CHECK(spaceSplit[4] == "/");

    const std::vector<std::string> slashSplit = Raz::StrUtils::split(baseStr, '/');

    CHECK(slashSplit.size() == 2);
    CHECK(slashSplit[0] == "ͳhîs'†èsτ  (ís ä)     gøθd");
    CHECK(slashSplit[1] == "†èsτ");
  }

  // Wide strings
  {
    const std::wstring baseStr = L"ͳhîs'†èsτ  (ís ä)     gøθd/†èsτ   /   ";

    const std::vector<std::wstring> spaceSplit = Raz::StrUtils::split(baseStr, L' ');

    CHECK(spaceSplit.size() == 5);
    CHECK(spaceSplit[0] == L"ͳhîs'†èsτ");
    CHECK(spaceSplit[1] == L"(ís");
    CHECK(spaceSplit[2] == L"ä)");
    CHECK(spaceSplit[3] == L"gøθd/†èsτ");
    CHECK(spaceSplit[4] == L"/");

    const std::vector<std::wstring> slashSplit = Raz::StrUtils::split(baseStr, L'/');

    CHECK(slashSplit.size() == 2);
    CHECK(slashSplit[0] == L"ͳhîs'†èsτ  (ís ä)     gøθd");
    CHECK(slashSplit[1] == L"†èsτ");
  }
}
