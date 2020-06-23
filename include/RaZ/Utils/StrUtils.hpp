#pragma once

#ifndef RAZ_STRUTILS_HPP
#define RAZ_STRUTILS_HPP

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <cwctype>
#include <locale>
#include <string>
#include <vector>

namespace Raz::StrUtils {

/// Transforms in-place a character to lowercase.
/// \param character Character to be transformed.
/// \return Lowercase character.
inline char toLowercase(char& character) {
  character = static_cast<char>(std::tolower(character));
  return character;
}

/// Transforms in-place a wide character to lowercase.
/// \param character Wide character to be transformed.
/// \return Lowercase wide character.
inline wchar_t toLowercase(wchar_t& character) {
  character = static_cast<wchar_t>(std::towlower(character));
  return character;
}

/// Transforms in-place a string to lowercase.
/// \param text String to be transformed.
/// \return Reference to the lowercase string.
inline std::string& toLowercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), [] (char character) { return toLowercase(character); });
  return text;
}

/// Transforms in-place a wide string to lowercase.
/// \param text Wide string to be transformed.
/// \return Reference to the lowercase wide string.
inline std::wstring& toLowercase(std::wstring& text) {
  std::transform(text.begin(), text.end(), text.begin(), [] (wchar_t character) { return toLowercase(character); });
  return text;
}

/// Transforms in-place a character to uppercase.
/// \param character Character to be transformed.
/// \return Uppercase character.
inline char toUppercase(char& character) {
  character = static_cast<char>(std::toupper(character));
  return character;
}

/// Transforms in-place a wide character to uppercase.
/// \param character Wide character to be transformed.
/// \return Uppercase wide character.
inline wchar_t toUppercase(wchar_t& character) {
  character = static_cast<wchar_t>(std::towupper(character));
  return character;
}

/// Transforms in-place a string to uppercase.
/// \param text String to be transformed.
/// \return Reference to the uppercase string.
inline std::string& toUppercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), [] (char character) { return toUppercase(character); });
  return text;
}

/// Transforms in-place a wide string to uppercase.
/// \param text Wide string to be transformed.
/// \return Reference to the uppercase wide string.
inline std::wstring& toUppercase(std::wstring& text) {
  std::transform(text.begin(), text.end(), text.begin(), [] (wchar_t character) { return toUppercase(character); });
  return text;
}

/// Transforms a character to lowercase without modifying the input.
/// \param character Character to be transformed.
/// \return Lowercase character copy.
inline char toLowercaseCopy(char character) {
  toLowercase(character);
  return character;
}

/// Transforms a wide character to lowercase without modifying the input.
/// \param character Wide character to be transformed.
/// \return Lowercase wide character.
inline wchar_t toLowercaseCopy(wchar_t character) {
  toLowercase(character);
  return character;
}

/// Transforms a string to lowercase without modifying the input.
/// \param text String to be transformed.
/// \return Lowercase string copy.
inline std::string toLowercaseCopy(std::string text) {
  toLowercase(text);
  return text;
}

/// Transforms a wide string to lowercase without modifying the input.
/// \param text Wide string to be transformed.
/// \return Lowercase wide string copy.
inline std::wstring toLowercaseCopy(std::wstring text) {
  toLowercase(text);
  return text;
}

/// Transforms a character to uppercase without modifying the input.
/// \param character Character to be transformed.
/// \return Uppercase character copy.
inline char toUppercaseCopy(char character) {
  toUppercase(character);
  return character;
}

/// Transforms a wide character to uppercase without modifying the input.
/// \param character Wide character to be transformed.
/// \return Uppercase wide character copy.
inline wchar_t toUppercaseCopy(wchar_t character) {
  toUppercase(character);
  return character;
}

/// Transforms a string to uppercase without modifying the input.
/// \param text String to be transformed.
/// \return Uppercase string copy.
inline std::string toUppercaseCopy(std::string text) {
  toUppercase(text);
  return text;
}

/// Transforms a wide string to uppercase without modifying the input.
/// \param text Wide string to be transformed.
/// \return Uppercase wide string copy.
inline std::wstring toUppercaseCopy(std::wstring text) {
  toUppercase(text);
  return text;
}

/// Removes spaces in-place at the start of a string.
/// \param text String to be trimmed.
/// \return Reference to the trimmed string.
inline std::string& trimLeft(std::string& text) {
  text.erase(text.begin(), std::find_if(text.begin(), text.end(), [] (int c) {
    return !std::isspace(c);
  }));
  return text;
}

/// Removes spaces in-place at the start of a wide string.
/// \param text Wide string to be trimmed.
/// \return Reference to the trimmed wide string.
inline std::wstring& trimLeft(std::wstring& text) {
  text.erase(text.begin(), std::find_if(text.begin(), text.end(), [] (wchar_t c) {
    return !std::iswspace(c);
  }));
  return text;
}

/// Removes spaces in-place at the end of a string.
/// \param text String to be trimmed.
/// \return Reference to the trimmed string.
inline std::string& trimRight(std::string& text) {
  text.erase(std::find_if(text.rbegin(), text.rend(), [] (int c) {
    return !std::isspace(c);
  }).base(), text.end());
  return text;
}

/// Removes spaces in-place at the end of a wide string.
/// \param text Wide string to be trimmed.
/// \return Reference to the trimmed wide string.
inline std::wstring& trimRight(std::wstring& text) {
  text.erase(std::find_if(text.rbegin(), text.rend(), [] (wchar_t c) {
    return !std::iswspace(c);
  }).base(), text.end());
  return text;
}

/// Removes spaces in-place on both ends of a string.
/// \param text String to be trimmed.
/// \return Reference to the trimmed string.
inline std::string& trim(std::string& text) {
  trimLeft(text);
  trimRight(text);
  return text;
}

/// Removes spaces in-place on both ends of a wide string.
/// \param text Wide string to be trimmed.
/// \return Reference to the trimmed wide string.
inline std::wstring& trim(std::wstring& text) {
  trimLeft(text);
  trimRight(text);
  return text;
}

/// Removes spaces at the start of a string without modifying the input.
/// \param text String to be trimmed.
/// \return Trimmed string copy.
inline std::string trimLeftCopy(std::string text) {
  trimLeft(text);
  return text;
}

/// Removes spaces at the start of a wide string without modifying the input.
/// \param text Wide string to be trimmed.
/// \return Trimmed wide string copy.
inline std::wstring trimLeftCopy(std::wstring text) {
  trimLeft(text);
  return text;
}

/// Removes spaces at the end of a string without modifying the input.
/// \param text String to be trimmed.
/// \return Trimmed string copy.
inline std::string trimRightCopy(std::string text) {
  trimRight(text);
  return text;
}

/// Removes spaces at the end of a wide string without modifying the input.
/// \param text Wide string to be trimmed.
/// \return Trimmed wide string copy.
inline std::wstring trimRightCopy(std::wstring text) {
  trimRight(text);
  return text;
}

/// Removes spaces on both ends of a string without modifying the input.
/// \param text String to be trimmed.
/// \return Trimmed string copy.
inline std::string trimCopy(std::string text) {
  trim(text);
  return text;
}

/// Removes spaces on both ends of a wide string without modifying the input.
/// \param text Wide string to be trimmed.
/// \return Trimmed wide string copy.
inline std::wstring trimCopy(std::wstring text) {
  trim(text);
  return text;
}

/// Splits a string with a given delimiter.
/// \param text String to be split.
/// \param delimiter Character used to split into parts.
/// \return Parts of the split string.
inline std::vector<std::string> split(std::string text, char delimiter) {
  trimRight(text);

  std::vector<std::string> parts {};

  while (!text.empty()) {
    const std::size_t delimPos = text.find_first_of(delimiter);

    if (delimPos > text.size()) {
      parts.emplace_back(std::move(text));
      break;
    }

    parts.emplace_back(text.substr(0, delimPos));
    trimRight(parts.back());

    text.erase(0, delimPos + 1);
    trimLeft(text);
  }

  return parts;
}

/// Splits a wide string with a given delimiter.
/// \param text Wide string to be split.
/// \param delimiter Wide character used to split into parts.
/// \return Parts of the split wide string.
inline std::vector<std::wstring> split(std::wstring text, wchar_t delimiter) {
  trimRight(text);

  std::vector<std::wstring> parts {};

  while (!text.empty()) {
    const std::size_t delimPos = text.find_first_of(delimiter);

    if (delimPos > text.size()) {
      parts.emplace_back(std::move(text));
      break;
    }

    parts.emplace_back(text.substr(0, delimPos));
    trimRight(parts.back());

    text.erase(0, delimPos + 1);
    trimLeft(text);
  }

  return parts;
}

/// Converts a wide string to an UTF-8 encoded one.
/// \param text Wide string to convert.
/// \return Converted UTF-8 string.
inline std::string toUtf8(const std::wstring& text) {
  return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(text);
}

/// Returns the current UTF-8 encoded string.
/// \note This does nothing & exists only to avoid using preprocessor switches.
/// \param text UTF-8 string to be returned.
/// \return Input UTF-8 string.
constexpr const std::string& toUtf8(const std::string& text) {
  return text;
}

/// Converts an UTF-8 encoded string to a wide one.
/// \param text UTF-8 string to convert.
/// \return Converted wide string.
inline std::wstring toWide(const std::string& text) {
  return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(text);
}

/// Returns the current wide string.
/// \note This does nothing & exists only to avoid using preprocessor switches.
/// \param text Wide string to be returned.
/// \return Input wide string.
constexpr const std::wstring& toWide(const std::wstring& text) {
  return text;
}

} // namespace Raz::StrUtils

#endif // RAZ_STRUTILS_HPP
