#pragma once

#ifndef RAZ_FILEUTILS_HPP
#define RAZ_FILEUTILS_HPP

#include <string>
#include <vector>

namespace Raz {

class FilePath;

namespace FileUtils {

/// Checks if a file is readable (exists and can be opened).
/// \param filePath Path to the file to be checked.
/// \return True if the file is readable, false otherwise.
bool isReadable(const FilePath& filePath);

/// Reads a whole file into a byte array.
/// \param filePath Path to the file to read.
/// \return Content of the file.
std::vector<unsigned char> readFileToArray(const FilePath& filePath);

/// Reads a whole file into a string.
/// \note The returned string is not trimmed: there can be spaces or empty lines at the beginning or the end if they exist in the file.
/// \param filePath Path to the file to read.
/// \return Content of the file.
std::string readFileToString(const FilePath& filePath);

} // namespace FileUtils

} // namespace Raz

#endif // RAZ_FILEUTILS_HPP
