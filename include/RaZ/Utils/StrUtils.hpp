#pragma once

#ifndef RAZ_STRUTILS_HPP
#define RAZ_STRUTILS_HPP

#include <algorithm>
#include <cctype>
#include <string>

namespace Raz::StrUtils {

/// Transforms in-place a character to lowercase.
/// \param character Character to be transformed.
/// \return Lowercase character.
inline char toLowercase(char& character) {
  character = static_cast<char>(std::tolower(character));
  return character;
}

/// Transforms in-place a string to lowercase.
/// \param text String to be transformed.
/// \return Reference to the lowercase string.
inline std::string& toLowercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), [] (char character) { return toLowercase(character); });
  return text;
}

/// Transforms in-place a character to uppercase.
/// \param character Character to be transformed.
/// \return Uppercase character.
inline char toUppercase(char& character) {
  character = static_cast<char>(std::toupper(character));
  return character;
}

/// Transforms in-place a string to uppercase.
/// \param text String to be transformed.
/// \return Reference to the uppercase string.
inline std::string& toUppercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), [] (char character) { return toUppercase(character); });
  return text;
}

/// Transforms a character to lowercase without modifying the input.
/// \param character Character to be transformed.
/// \return Lowercase character.
inline char toLowercaseCopy(char character) {
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

/// Transforms a character to uppercase without modifying the input.
/// \param character Character to be transformed.
/// \return Uppercase character.
inline char toUppercaseCopy(char character) {
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

/// Removes spaces in-place at the start of a string.
/// \param text String to be trimmed.
/// \return Reference to the trimmed string.
inline std::string& trimLeft(std::string& text) {
  text.erase(text.begin(), std::find_if(text.begin(), text.end(), [] (int c) {
    return !std::isspace(c);
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

/// Removes spaces in-place on both ends of a string.
/// \param text String to be trimmed.
/// \return Reference to the trimmed string.
inline std::string& trim(std::string& text) {
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

/// Removes spaces at the end of a string without modifying the input.
/// \param text String to be trimmed.
/// \return Trimmed string copy.
inline std::string trimRightCopy(std::string text) {
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

/// Splits a string with a given delimiter.
/// \param text String to be splitted.
/// \param delimiter Character used to split into parts.
/// \return Parts of the splitted string.
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

} // namespace Raz::StrUtils

#endif // RAZ_STRUTILS_HPP
