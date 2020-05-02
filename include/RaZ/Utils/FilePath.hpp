#pragma once

#ifndef RAZ_FILEPATH_HPP
#define RAZ_FILEPATH_HPP

// If using MinGW with a version below 9.X, std::filesystem's support is unpredictable
#if defined(__MINGW32__) && __GNUC__ < 9
#pragma message("Warning: std::filesystem may not be fully supported by your MinGW; it should be updated to MinGW 9.X or later.")
#else
#define RAZ_FILESYSTEM_AVAILABLE
#endif

#ifdef RAZ_FILESYSTEM_AVAILABLE

#include <filesystem>
#include <string>
#include <string_view>

namespace Raz {

class FilePath {
public:
  /// Creates a path in a platform-dependent encoding from a given UTF-8 string.
  /// \param pathStr UTF-8 encoded path string.
  FilePath(const char* pathStr) : m_path{ std::filesystem::u8path(pathStr) } {}
  /// Creates a path in a platform-dependent encoding from a given UTF-8 string.
  /// \param pathStr UTF-8 encoded path string.
  FilePath(const std::string& pathStr) : FilePath(pathStr.c_str()) {}
  /// Creates a path in a platform-dependent encoding from a given UTF-8 string.
  /// \param pathStr UTF-8 encoded path string.
  FilePath(const std::string_view& pathStr) : FilePath(pathStr.data()) {}

  const std::filesystem::path& getPath() const noexcept { return m_path; }

  bool operator==(const FilePath& filePath) const noexcept { return m_path == filePath.getPath(); }
  bool operator!=(const FilePath& filePath) const noexcept { return !(*this == filePath); }
  operator const std::filesystem::path&() const noexcept { return m_path; }

private:
  std::filesystem::path m_path {};
};

} // namespace Raz

#endif // RAZ_FILESYSTEM_AVAILABLE

#endif // RAZ_FILEPATH_HPP
