#pragma once

#ifndef RAZ_STRUTILS_HPP
#define RAZ_STRUTILS_HPP

#include <algorithm>
#include <cctype>
#include <string>

namespace Raz {

namespace StrUtils {

inline char toLowercase(char& character) {
  character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
  return character;
}

inline std::string& toLowercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), ::tolower);
  return text;
}

inline char toUppercase(char& character) {
  character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
  return character;
}

inline std::string& toUppercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), ::toupper);
  return text;
}

inline char toLowercaseCopy(char character) {
  toLowercase(character);
  return character;
}

inline std::string toLowercaseCopy(std::string text) {
  toLowercase(text);
  return text;
}

inline char toUppercaseCopy(char character) {
  toUppercase(character);
  return character;
}

inline std::string toUppercaseCopy(std::string text) {
  toUppercase(text);
  return text;
}

inline std::string& trimLeft(std::string& text) {
  text.erase(text.begin(), std::find_if(text.begin(), text.end(), [] (int c) {
    return !std::isspace(c);
  }));
  return text;
}

inline std::string& trimRight(std::string& text) {
  text.erase(std::find_if(text.rbegin(), text.rend(), [] (int c) {
    return !std::isspace(c);
  }).base(), text.end());
  return text;
}

inline std::string& trim(std::string& text) {
  trimLeft(text);
  trimRight(text);
  return text;
}

inline std::string trimLeftCopy(std::string text) {
  trimLeft(text);
  return text;
}

inline std::string trimRightCopy(std::string text) {
  trimRight(text);
  return text;
}

inline std::string trimCopy(std::string text) {
  trim(text);
  return text;
}

inline std::vector<std::string> split(std::string inputStr, char delimiter) {
  trimRight(inputStr);

  std::vector<std::string> parts {};

  while (!inputStr.empty()) {
    const std::size_t delimPos = inputStr.find_first_of(delimiter);

    if (delimPos > inputStr.size()) {
      parts.push_back(inputStr);
      break;
    }

    parts.push_back(inputStr.substr(0, delimPos));
    trimRight(parts.back());

    inputStr.erase(0, delimPos + 1);
    trimLeft(inputStr);
  }

  return parts;
}

} // namespace StrUtils

} // namespace Raz

#endif // RAZ_STRUTILS_HPP
