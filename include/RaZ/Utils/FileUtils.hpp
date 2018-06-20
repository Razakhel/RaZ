#pragma once

#ifndef RAZ_FILEUTILS_HPP
#define RAZ_FILEUTILS_HPP

#include <string>

namespace Raz {

namespace FileUtils {

inline std::string extractFileExtension(const std::string& fileName) {
  return fileName.substr(fileName.find_last_of('.') + 1);
}

inline std::string extractPathToFile(const std::string& fileName) {
  return fileName.substr(0, fileName.find_last_of('/') + 1);
}

inline std::string extractFileNameFromPath(const std::string& filePath, bool keepExtension = true) {
  auto fileName = filePath.substr(filePath.find_last_of('/') + 1);

  if (!keepExtension)
    fileName = fileName.erase(fileName.find_last_of('.'));

  return fileName;
}

} // namespace FileUtils

} // namespace Raz

#endif // RAZ_FILEUTILS_HPP
