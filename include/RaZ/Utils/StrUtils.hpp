#pragma once

#ifndef RAZ_STRUTILS_HPP
#define RAZ_STRUTILS_HPP

#include <algorithm>
#include <string>

namespace Raz {

namespace StrUtils {

inline std::string& toLowercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), ::tolower);
  return text;
}

inline std::string& toUppercase(std::string& text) {
  std::transform(text.begin(), text.end(), text.begin(), ::toupper);
  return text;
}

inline std::string& toLowercaseCopy(std::string text) {
  return toLowercase(text);
}

inline std::string& toUppercaseCopy(std::string text) {
  return toUppercase(text);
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

} // namespace StrUtils

} // namespace Raz

#endif // RAZ_STRUTILS_HPP
