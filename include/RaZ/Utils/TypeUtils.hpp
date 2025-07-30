#pragma once

#ifndef RAZ_TYPEUTILS_HPP
#define RAZ_TYPEUTILS_HPP

#include <string_view>

namespace Raz::TypeUtils {

struct nonesuch {
  nonesuch() = delete;
  ~nonesuch() = delete;
  nonesuch(const nonesuch&) = delete;
  nonesuch& operator=(const nonesuch&) = delete;
};

template <typename...>
using void_t = void;

namespace internal {
  template <typename V, typename D>
  struct detect_impl {
    using value_t = V;
    using type = D;
  };

  template <typename D, template <typename...> class Check, typename... Args>
  auto detect_check(char) -> detect_impl<std::false_type, D>;

  template <typename D, template <typename...> class Check, typename... Args>
  auto detect_check(int) -> decltype(sizeof(Check<Args...>), detect_impl<std::true_type, Check<Args...>>{});

  template <typename D, typename Void, template <typename...> class Check, typename... Args>
  struct detect : decltype(detect_check<D, Check, Args...>(0)) {};
} // namespace internal

template <template <typename...> class Check, typename... Args>
using is_detected = typename internal::detect<nonesuch, void, Check, Args...>::value_t;

template <template <typename...> class Check, typename... Args>
inline constexpr bool is_detected_v = is_detected<Check, Args...>::value;

// Add this new alias for the detected type (replaces old DetectedT)
template <template <typename...> class Check, typename... Args>
using detected_t = typename internal::detect<nonesuch, void, Check, Args...>::type;


/// Recovers a string of the given type's name at compile-time.
/// \tparam T Type to recover the name of.
/// \return String representing the type's name.
template <typename T>
constexpr std::string_view getTypeStr() noexcept {
#if defined(__clang__)
  // Has the form "std::string_view Raz::TypeUtils::getTypeStr() [T = ...]"

  constexpr std::string_view funcSignature = __PRETTY_FUNCTION__;
  constexpr std::size_t startStride        = std::size("std::string_view Raz::TypeUtils::getTypeStr() [T = ") - 1;

  return std::string_view(funcSignature.data() + startStride, funcSignature.size() - startStride - 1);

  // The following implementation would be ideal, but can't be resolved at compile-time (yet?)
  // See: https://stackoverflow.com/questions/56484834/constexpr-stdstring-viewfind-last-of-doesnt-work-on-clang-8-with-libstdc

//  constexpr std::size_t firstPos = funcSignature.find_first_of('=');
//  static_assert(firstPos != std::string_view::npos, "Error: Character '=' not found in the function's signature.");
//
//  constexpr std::size_t lastPos = funcSignature.find_last_of(']');
//  static_assert(lastPos != std::string_view::npos, "Error: Character ']' not found in the function's signature.");
//
//  static_assert(firstPos < lastPos, "Error: Trailing character found before leading one in the function's signature.");
//
//  return std::string_view(funcSignature.data() + firstPos + 2, lastPos - firstPos - 2);
#elif defined(__GNUC__)
  // Has the form "constexpr std::string_view Raz::TypeUtils::getTypeStr() [with T = ...; std::string_view = std::basic_string_view<char>]"

  constexpr std::string_view funcSignature = __PRETTY_FUNCTION__;

  constexpr std::size_t firstPos = funcSignature.find_first_of('=');
  static_assert(firstPos != std::string_view::npos, "Error: Character '=' not found in the function's signature.");

  constexpr std::size_t lastPos = funcSignature.find_first_of(';', firstPos);
  static_assert(lastPos != std::string_view::npos, "Error: Character ';' not found in the function's signature.");

  static_assert(firstPos < lastPos, "Error: Trailing character found before leading one in the function's signature.");

  return std::string_view(funcSignature.data() + firstPos + 2, lastPos - firstPos - 2);
#elif defined(_MSC_VER)
  // Has the form "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Raz::TypeUtils::getTypeStr<...>(void) noexcept"

  constexpr std::string_view funcSignature = __FUNCSIG__;

  constexpr std::size_t startStride = std::size("class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Raz::TypeUtils::getTypeStr<") - 1;
  constexpr std::size_t endStride   = std::size(">(void) noexcept") - 1;

  return std::string_view(funcSignature.data() + startStride, funcSignature.size() - startStride - endStride);

  // The following implementation would be ideal, but can't be resolved at compile-time because of a bug fixed only recently
  // See: https://developercommunity.visualstudio.com/content/problem/331234/stdbasic-string-viewfind-is-not-always-constexpr.html

//  constexpr std::string_view funcName = __FUNCTION__;
//
//  constexpr std::size_t firstPos = funcSignature.find(funcName);
//  static_assert(firstPos != std::string_view::npos, "Error: Function name not found in the function's signature.");
//
//  constexpr std::size_t lastPos = funcSignature.find_last_of('>');
//  static_assert(lastPos != std::string_view::npos, "Error: Character '>' not found in the function's signature.");
//
//  static_assert(firstPos < lastPos, "Error: Trailing character found before leading one in the function's signature.");
//
//  return std::string_view(funcSignature.data() + firstPos + funcName.size(), lastPos - firstPos - funcName.size());
#else
#pragma message("Warning: The current compiler is not supported.")
  return {};
#endif
}

/// Recovers a string of the given enumeration value's name at compile-time.
/// \tparam Enum Enumeration value to recover the name of.
/// \return String representing the enum value's name.
template <auto Enum>
constexpr std::string_view getEnumStr() noexcept {
  static_assert(std::is_enum_v<decltype(Enum)>, "Error: The type of the given value must be an enum.");

#if defined(__clang__)
  // Has the form "std::string_view Raz::TypeUtils::getEnumStr() [Enum = ...]"

  constexpr std::string_view funcSignature = __PRETTY_FUNCTION__;
  constexpr std::size_t startStride        = std::size("std::string_view Raz::TypeUtils::getEnumStr() [Enum = ") - 1;

  return std::string_view(funcSignature.data() + startStride, funcSignature.size() - startStride - 1);
#elif defined(__GNUC__)
  // Has the form "constexpr std::string_view Raz::TypeUtils::getEnumStr() [with auto Enum = ...; std::string_view = std::basic_string_view<char>]"

  constexpr std::string_view funcSignature = __PRETTY_FUNCTION__;

  constexpr std::size_t firstPos = funcSignature.find_first_of('=');
  static_assert(firstPos != std::string_view::npos, "Error: Character '=' not found in the function's signature.");

  constexpr std::size_t lastPos = funcSignature.find_first_of(';', firstPos);
  static_assert(lastPos != std::string_view::npos, "Error: Character ';' not found in the function's signature.");

  static_assert(firstPos < lastPos, "Error: Trailing character found before leading one in the function's signature.");

  return std::string_view(funcSignature.data() + firstPos + 2, lastPos - firstPos - 2);
#elif defined(_MSC_VER)
  // Has the form "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Raz::TypeUtils::getEnumStr<...>(void) noexcept"

  constexpr std::string_view funcSignature = __FUNCSIG__;

  constexpr std::size_t startStride = std::size("class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Raz::TypeUtils::getEnumStr<") - 1;
  constexpr std::size_t endStride   = std::size(">(void) noexcept") - 1;

  return std::string_view(funcSignature.data() + startStride, funcSignature.size() - startStride - endStride);
#else
#pragma message("Warning: The current compiler is not supported.")
  return {};
#endif
}

// Cross-platform implementation of std::experimental::is_detected
// This implementation works correctly with MSVC, GCC, and Clang
// See: https://en.cppreference.com/w/cpp/experimental/is_detected

// Replace these (if present) with:
template <template <typename...> typename Attr, typename... Args>
using is_detected_t = is_detected<Attr, Args...>;  // Type: std::true_type or std::false_type

template <typename Expected, template <typename...> typename Attr, typename... Args>
using is_detected_exact_t = std::is_same<Expected, detected_t<Attr, Args...>>;

template <typename To, template <typename...> typename Attr, typename... Args>
using is_detected_convertible_t = std::is_convertible<detected_t<Attr, Args...>, To>;

namespace Attribute {

template <typename T>
using DefaultConstructor = decltype(T());

template <typename T>
using CopyConstructor = decltype(T(std::declval<const T&>()));

template <typename T>
using MoveConstructor = decltype(T(std::declval<T&&>()));

template <typename T>
using CopyAssignmentOperator = decltype(std::declval<T&>() = std::declval<const T&>());

template <typename T>
using MoveAssignmentOperator = decltype(std::declval<T&>() = std::declval<T&&>());

template <typename T, typename U = T>
using EqualityOperator = decltype(std::declval<T>() == std::declval<U>());

template <typename T, typename U = T>
using InequalityOperator = decltype(std::declval<T>() != std::declval<U>());

template <typename T, typename U = T>
using PlusOperator = decltype(std::declval<T>() + std::declval<U>());

template <typename T, typename U = T>
using MinusOperator = decltype(std::declval<T>() - std::declval<U>());

template <typename T, typename U = T>
using MultiplicationOperator = decltype(std::declval<T>() * std::declval<U>());

template <typename T, typename U = T>
using DivisionOperator = decltype(std::declval<T>() / std::declval<U>());

template <typename T>
using DefaultDestructor = decltype(std::declval<T&>().~T());

} // namespace Attribute

/// Checks if the given types make available an attribute.
/// \note Non-accessible attributes (private/protected) cannot be detected.
/// \tparam Attr Attribute to be checked.
/// \tparam Args Types in which to check the attribute's existence.
/// \return True if the attribute exists & is accessible, false otherwise.
template <template <typename...> typename Attr, typename... Args>
constexpr bool hasAttribute() {
  return is_detected_v<Attr, Args...>;
}

/// Checks if the given types' attribute returns the specific expected type.
/// \tparam Expected Return type to be checked.
/// \tparam Attr Attribute to be checked.
/// \tparam Args Types for which to check the attribute's return type.
/// \return True if the attribute's return type is the same as expected, false otherwise.
template <typename Expected, template <typename...> typename Attr, typename... Args>
constexpr bool hasReturnType() {
  return is_detected_exact_t<Expected, Attr, Args...>::value;
}

/// Checks if the given types' attribute return type is convertible to the given one.
/// \tparam To Convertible return type to be checked.
/// \tparam Attr Attribute to be checked.
/// \tparam Args Types for which to check the attribute's return type.
/// \return True if the attribute's return type is convertible, false otherwise.
template <typename To, template <typename...> typename Attr, typename... Args>
constexpr bool hasReturnTypeConvertible() {
  return is_detected_convertible_t<To, Attr, Args...>::value;
}

/// Checks if the default constructor is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the default constructor is available, false otherwise.
template <typename T>
constexpr bool hasDefaultConstructor() noexcept { return hasAttribute<Attribute::DefaultConstructor, T>(); }

/// Checks if the copy constructor is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the copy constructor is available, false otherwise.
template <typename T>
constexpr bool hasCopyConstructor() noexcept { return hasAttribute<Attribute::CopyConstructor, T>(); }

/// Checks if the move constructor is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the move constructor is available, false otherwise.
template <typename T>
constexpr bool hasMoveConstructor() noexcept { return hasAttribute<Attribute::MoveConstructor, T>(); }

/// Checks if the copy assignment operator is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the copy assignment operator is available, false otherwise.
template <typename T>
constexpr bool hasCopyAssignmentOperator() noexcept { return hasAttribute<Attribute::CopyAssignmentOperator, T>(); }

/// Checks if the move assignment operator is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the move assignment operator is available, false otherwise.
template <typename T>
constexpr bool hasMoveAssignmentOperator() noexcept { return hasAttribute<Attribute::MoveAssignmentOperator, T>(); }

/// Checks if the equality operator is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the equality operator is available, false otherwise.
template <typename T>
constexpr bool hasEqualityOperator() noexcept { return hasAttribute<Attribute::EqualityOperator, T>(); }

/// Checks if the inequality operator is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the inequality operator is available, false otherwise.
template <typename T>
constexpr bool hasInequalityOperator() noexcept { return hasAttribute<Attribute::InequalityOperator, T>(); }

/// Checks if the default destructor is available for the given type.
/// \tparam T Type to check the attribute for.
/// \return True if the default destructor is available, false otherwise.
template <typename T>
constexpr bool hasDefaultDestructor() noexcept { return hasAttribute<Attribute::DefaultDestructor, T>(); }


} // namespace Raz::TypeUtils

#endif // RAZ_TYPEUTILS_HPP
