#pragma once

#ifndef RAZ_FILEUTILS_HPP
#define RAZ_FILEUTILS_HPP

#include <string>

namespace Raz {

class FilePath;

namespace FileUtils {

/// Reads a whole file into a string.
/// \note The returned string is not trimmed: there can be spaces or empty lines at the beginning or the end if they exist in the file.
/// \param filePath Path to the file to read.
/// \return Content of the file.
std::string readFile(const FilePath& filePath);

} // namespace FileUtils

} // namespace Raz

#endif // RAZ_FILEUTILS_HPP
