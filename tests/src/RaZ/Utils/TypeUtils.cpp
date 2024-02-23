#include "RaZ/Utils/TypeUtils.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

class AttributeTest {
public:
  AttributeTest() = default;
  AttributeTest(const AttributeTest&) = default;
  AttributeTest(AttributeTest&&) = delete;

  AttributeTest& operator=(const AttributeTest&) = default;
  bool operator==(const AttributeTest&) { return true; }

  operator bool() { return true; }

  ~AttributeTest() = default;

protected:
  bool operator!=(const AttributeTest&) { return true; }

private:
  AttributeTest& operator=(AttributeTest&&) = default;
};

} // namespace

enum class EnumTest {
  VALUE,
  TEST,
  AGAIN
};

TEST_CASE("TypeUtils type str", "[utils]") {
  int testInt {};
  const int& testIntRef = testInt;

  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<int>() == "int");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<decltype(testInt)>() == "int");

#if defined(RAZ_COMPILER_CLANG)
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<const int*>() == "const int *");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int &");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "char const (&)[13]");
#if __clang_major__ == 11
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<std::string_view>() == "std::basic_string_view<char>");
#else
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<std::string_view>() == "std::basic_string_view<char, std::char_traits<char> >");
#endif
#elif defined(RAZ_COMPILER_GCC)
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<const int*>() == "const int*");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int&");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "const char (&)[13]");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<std::string_view>() == "std::basic_string_view<char>");
#elif defined(RAZ_COMPILER_MSVC)
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<const int*>() == "const int*");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<decltype(testIntRef)>() == "const int&");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<decltype("Hello world!")>() == "const char(&)[13]");
  CHECK_NOFAIL(Raz::TypeUtils::getTypeStr<std::string_view>() == "class std::basic_string_view<char,struct std::char_traits<char> >");
#endif
}

TEST_CASE("TypeUtils enum str", "[utils]") {
#if defined(RAZ_COMPILER_GCC) && __GNUC__ < 9
  // Prior to version 9, GCC prints enum values as (Type)value
  CHECK_NOFAIL(Raz::TypeUtils::getEnumStr<EnumTest::VALUE>() == "(EnumTest)0");
  CHECK_NOFAIL(Raz::TypeUtils::getEnumStr<EnumTest::TEST>() == "(EnumTest)1");
  CHECK_NOFAIL(Raz::TypeUtils::getEnumStr<EnumTest::AGAIN>() == "(EnumTest)2");
#else
  CHECK_NOFAIL(Raz::TypeUtils::getEnumStr<EnumTest::VALUE>() == "EnumTest::VALUE");
  CHECK_NOFAIL(Raz::TypeUtils::getEnumStr<EnumTest::TEST>() == "EnumTest::TEST");
  CHECK_NOFAIL(Raz::TypeUtils::getEnumStr<EnumTest::AGAIN>() == "EnumTest::AGAIN");
#endif
}

#if !defined(RAZ_COMPILER_MSVC)

TEST_CASE("TypeUtils has attribute", "[utils]") {
  CHECK(Raz::TypeUtils::hasDefaultConstructor<AttributeTest>());
  CHECK(Raz::TypeUtils::hasCopyConstructor<AttributeTest>());
  CHECK_FALSE(Raz::TypeUtils::hasMoveConstructor<AttributeTest>()); // Deleted

  CHECK(Raz::TypeUtils::hasCopyAssignmentOperator<AttributeTest>());
  CHECK_FALSE(Raz::TypeUtils::hasMoveAssignmentOperator<AttributeTest>()); // Existing but private

  CHECK(Raz::TypeUtils::hasEqualityOperator<AttributeTest>());
  CHECK_FALSE(Raz::TypeUtils::hasInequalityOperator<AttributeTest>()); // Existing but protected

  CHECK(Raz::TypeUtils::hasDefaultDestructor<AttributeTest>());

  CHECK(Raz::TypeUtils::hasReturnType<bool, Raz::TypeUtils::Attribute::EqualityOperator, AttributeTest>());
  CHECK(Raz::TypeUtils::hasReturnType<AttributeTest&, Raz::TypeUtils::Attribute::CopyAssignmentOperator, AttributeTest>());

  // AttributeTest has a non-explicit operator bool(); the created object is then implicitly convertible to bool
  CHECK(Raz::TypeUtils::hasReturnTypeConvertible<bool, Raz::TypeUtils::Attribute::DefaultConstructor, AttributeTest>());
}

#endif
