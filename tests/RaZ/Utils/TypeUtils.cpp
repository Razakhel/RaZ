#include "Catch.hpp"

#include "RaZ/Utils/TypeUtils.hpp"

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

#if !defined(RAZ_COMPILER_MSVC)

TEST_CASE("TypeUtils has attribute") {
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
