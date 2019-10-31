#include "Catch.hpp"

#include "RaZ/Utils/TypeUtils.hpp"

TEST_CASE("TypeUtils type str") {
  int testInt {};
  const int& testIntRef = testInt;

  CHECK(Raz::TypeUtils::getTypeStr<int>() == "int");
  CHECK(Raz::TypeUtils::getTypeStr<decltype(testInt)>() == "int");

#if defined(RAZ_COMPILER_CLANG)
  CHECK(Raz::TypeUtils::getTypeStr<const int*>() == "const int *");
  CHECK(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int &");
  CHECK(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "char const (&)[13]");
  CHECK(Raz::TypeUtils::getTypeStr<std::string_view>() == "std::basic_string_view<char, std::char_traits<char> >");
#elif defined(RAZ_COMPILER_GCC)
  CHECK(Raz::TypeUtils::getTypeStr<const int*>() == "const int*");
  CHECK(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int&");
  CHECK(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "const char (&)[13]");
  CHECK(Raz::TypeUtils::getTypeStr<std::string_view>() == "std::basic_string_view<char>");
#elif defined(RAZ_COMPILER_MSVC)
  CHECK(Raz::TypeUtils::getTypeStr<const int*>() == "const int*");
  CHECK(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int&");
  CHECK(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "const char(&)[13]");
  CHECK(Raz::TypeUtils::getTypeStr<std::string_view>() == "class std::basic_string_view<char,struct std::char_traits<char> >");
#endif
}
