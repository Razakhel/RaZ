#include "Catch.hpp"

#include "RaZ/Utils/TypeUtils.hpp"

TEST_CASE("TypeUtils type str") {
  int testInt {};
  const int& testIntRef = testInt;

  REQUIRE(Raz::TypeUtils::getTypeStr<int>() == "int");
  REQUIRE(Raz::TypeUtils::getTypeStr<decltype(testInt)>() == "int");

#if defined(__clang__)
  REQUIRE(Raz::TypeUtils::getTypeStr<const int*>() == "const int *");
  REQUIRE(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int &");
  REQUIRE(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "char const (&)[13]");
  REQUIRE(Raz::TypeUtils::getTypeStr<std::string_view>() == "std::basic_string_view<char, std::char_traits<char> >");
#elif defined(__GNUC__)
  REQUIRE(Raz::TypeUtils::getTypeStr<const int*>() == "const int*");
  REQUIRE(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int&");
  REQUIRE(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "const char (&)[13]");
  REQUIRE(Raz::TypeUtils::getTypeStr<std::string_view>() == "std::basic_string_view<char>");
#elif defined(_MSC_VER)
  REQUIRE(Raz::TypeUtils::getTypeStr<const int*>() == "const int*");
  REQUIRE(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int&");
  REQUIRE(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "const char(&)[13]");
  REQUIRE(Raz::TypeUtils::getTypeStr<std::string_view>() == "class std::basic_string_view<char,struct std::char_traits<char> >");
#endif
}
