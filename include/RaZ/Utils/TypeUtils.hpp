#pragma once

#ifndef RAZ_TYPEUTILS_HPP
#define RAZ_TYPEUTILS_HPP

#include <string_view>

namespace Raz::TypeUtils {

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

  constexpr std::size_t lastPos  = funcSignature.find_first_of(';', firstPos);
  static_assert(lastPos != std::string_view::npos, "Error: Character ';' not found in the function's signature.");

  static_assert(firstPos < lastPos, "Error: Trailing character found before leading one in the function's signature.");

  return std::string_view(funcSignature.data() + firstPos + 2, lastPos - firstPos - 2);
#elif defined(_MSC_VER)
  // Has the form "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Raz::TypeUtils::getTypeStr<...>(void) noexcept"

  constexpr std::string_view funcSignature = __FUNCSIG__;

  constexpr std::size_t startStride = std::size("class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Raz::TypeUtils::getTypeStr<") - 1;
  constexpr std::size_t endStride   = std::size(">(void) noexcept") - 1;

  constexpr std::string_view typeStr(funcSignature.data() + startStride, funcSignature.size() - startStride - endStride);

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
  static_assert(false, "Error: The current compiler is not supported.")
#endif
}

} // namespace Raz::TypeUtils

#endif // RAZ_TYPEUTILS_HPP
