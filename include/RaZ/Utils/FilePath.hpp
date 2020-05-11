#pragma once

#ifndef RAZ_FILEPATH_HPP
#define RAZ_FILEPATH_HPP

#include <string>
#include <string_view>

namespace Raz {

class FilePath {
#if defined(RAZ_PLATFORM_WINDOWS) && !defined(RAZ_PLATFORM_CYGWIN) // Cygwin handles conversion on its own; sticking to UTF-8 strings
  using StringType = std::wstring;
#else
  using StringType = std::string;
#endif

public:
  /// Creates a path in a platform-dependent encoding from a given UTF-8 string.
  /// \note On Windows, the given string will be converted to UTF-16.
  /// \param pathStr UTF-8 encoded path string.
  FilePath(const char* pathStr);
  /// Creates a path in a platform-dependent encoding from a given UTF-8 string.
  /// \note On Windows, the given string will be converted to UTF-16.
  /// \param pathStr UTF-8 encoded path string.
  FilePath(const std::string& pathStr) : FilePath(pathStr.c_str()) {}
  /// Creates a path in a platform-dependent encoding from a given UTF-8 string.
  /// \note On Windows, the given string will be converted to UTF-16.
  /// \param pathStr UTF-8 encoded path string.
  FilePath(const std::string_view& pathStr) : FilePath(pathStr.data()) {}
  /// Creates a path in a platform-dependent encoding from a given wide string.
  /// \note On platforms other than Windows, the given string will be converted to UTF-8. If not using Windows, use the standard string constructors.
  /// \param pathStr Platform-specific encoded wide path string.
  FilePath(const wchar_t* pathStr);
  /// Creates a path in a platform-dependent encoding from a given wide string.
  /// \note On platforms other than Windows, the given string will be converted to UTF-8. If not using Windows, use the standard string constructors.
  /// \param pathStr Platform-specific encoded wide path string.
  FilePath(const std::wstring& pathStr) : FilePath(pathStr.c_str()) {}
  /// Creates a path in a platform-dependent encoding from a given wide string.
  /// \note On platforms other than Windows, the given string will be converted to UTF-8. If not using Windows, use the standard string constructors.
  /// \param pathStr Platform-specific encoded wide path string.
  FilePath(const std::wstring_view& pathStr) : FilePath(pathStr.data()) {}

  const StringType& getPath() const noexcept { return m_path; }
  const StringType::value_type* getPathStr() const noexcept { return m_path.c_str(); }

  bool operator==(const FilePath& filePath) const noexcept { return m_path == filePath.getPath(); }
  bool operator!=(const FilePath& filePath) const noexcept { return !(*this == filePath); }
  operator const StringType&() const noexcept { return m_path; }
  operator const StringType::value_type*() const noexcept { return m_path.c_str(); }

private:
  StringType m_path {};
};

} // namespace Raz

#endif // RAZ_FILEPATH_HPP
