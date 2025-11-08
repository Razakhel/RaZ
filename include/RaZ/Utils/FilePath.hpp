#pragma once

#ifndef RAZ_FILEPATH_HPP
#define RAZ_FILEPATH_HPP

#include <format>
#include <iosfwd>
#include <string>
#include <string_view>

namespace Raz {

class FilePath {
#if defined(_WIN32) && !defined(__CYGWIN__) // Cygwin handles conversions on its own; sticking to UTF-8 strings
  using StringType = std::wstring;
  using CharType   = wchar_t;
#else
  using StringType = std::string;
  using CharType   = char;
#endif

public:
  constexpr FilePath() = default;
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
  constexpr FilePath(const FilePath&) = default;
  constexpr FilePath(FilePath&&) noexcept = default;

  constexpr const StringType& getPath() const noexcept { return m_path; }
  constexpr const CharType* getPathStr() const noexcept { return m_path.c_str(); }
  constexpr bool isEmpty() const noexcept { return m_path.empty(); }

  static FilePath recoverPathToFile(const std::string& pathStr);
  static FilePath recoverPathToFile(const std::wstring& pathStr);
  static FilePath recoverPathToFile(const std::string_view& pathStr) { return recoverPathToFile(std::string(pathStr)); }
  static FilePath recoverPathToFile(const std::wstring_view& pathStr) { return recoverPathToFile(std::wstring(pathStr)); }
  static FilePath recoverFileName(const std::string& pathStr, bool keepExtension = true);
  static FilePath recoverFileName(const std::wstring& pathStr, bool keepExtension = true);
  static FilePath recoverFileName(const std::string_view& pathStr, bool keepExtension = true) { return recoverFileName(std::string(pathStr), keepExtension); }
  static FilePath recoverFileName(const std::wstring_view& pathStr, bool keepExtension = true) { return recoverFileName(std::wstring(pathStr), keepExtension); }
  static FilePath recoverExtension(const std::string& pathStr);
  static FilePath recoverExtension(const std::wstring& pathStr);
  static FilePath recoverExtension(const std::string_view& pathStr) { return recoverExtension(std::string(pathStr)); }
  static FilePath recoverExtension(const std::wstring_view& pathStr) { return recoverExtension(std::wstring(pathStr)); }

  FilePath recoverPathToFile() const { return recoverPathToFile(m_path); }
  FilePath recoverFileName(bool keepExtension = true) const { return recoverFileName(m_path, keepExtension); }
  FilePath recoverExtension() const { return recoverExtension(m_path); }
#if defined(_WIN32) && !defined(__CYGWIN__)
  std::string toUtf8() const;
  const StringType& toWide() const { return m_path; }
#else
  const StringType& toUtf8() const { return m_path; }
  std::wstring toWide() const;
#endif

  constexpr FilePath& operator=(const FilePath&) = default;
  constexpr FilePath& operator=(FilePath&&) noexcept = default;
  std::string operator+(char c) const { return toUtf8() + c; }
  std::wstring operator+(wchar_t c) const { return toWide() + c; }
  std::string operator+(const char* pathStr) const { return toUtf8() + pathStr; }
  std::wstring operator+(const wchar_t* pathStr) const { return toWide() + pathStr; }
  std::string operator+(const std::string& pathStr) const { return toUtf8() + pathStr; }
  std::wstring operator+(const std::wstring& pathStr) const { return toWide() + pathStr; }
  FilePath operator+(const FilePath& filePath) const { return m_path + filePath.getPath(); }
  bool operator==(const char* pathStr) const noexcept { return toUtf8() == pathStr; }
  bool operator==(const wchar_t* pathStr) const noexcept { return toWide() == pathStr; }
  bool operator==(const std::string& pathStr) const noexcept { return toUtf8() == pathStr; }
  bool operator==(const std::wstring& pathStr) const noexcept { return toWide() == pathStr; }
  bool operator==(const std::string_view& pathStr) const noexcept { return toUtf8() == pathStr; }
  bool operator==(const std::wstring_view& pathStr) const noexcept { return toWide() == pathStr; }
  bool operator==(const FilePath& filePath) const noexcept { return m_path == filePath.getPath(); }
  constexpr explicit operator const StringType&() const noexcept { return m_path; }
  constexpr operator const CharType*() const noexcept { return m_path.c_str(); }

private:
  StringType m_path;
};

static inline std::string operator+(char c, const FilePath& filePath) {
  return c + filePath.toUtf8();
}

static inline std::wstring operator+(wchar_t c, const FilePath& filePath) {
  return c + filePath.toWide();
}

static inline std::string operator+(const char* pathStr, const FilePath& filePath) {
  return pathStr + filePath.toUtf8();
}

static inline std::wstring operator+(const wchar_t* pathStr, const FilePath& filePath) {
  return pathStr + filePath.toWide();
}

static inline std::string operator+(const std::string& pathStr, const FilePath& filePath) {
  return pathStr + filePath.toUtf8();
}

static inline std::wstring operator+(const std::wstring& pathStr, const FilePath& filePath) {
  return pathStr + filePath.toWide();
}

extern std::ostream& operator<<(std::ostream& stream, const FilePath& filePath);
extern std::wostream& operator<<(std::wostream& stream, const FilePath& filePath);

} // namespace Raz

/// Specialization of std::hash for FilePath.
template <>
struct std::hash<Raz::FilePath> {
  /// Computes the hash of the given file path.
  /// \param filePath File path to compute the hash of.
  /// \return File path's hash value.
  std::size_t operator()(const Raz::FilePath& filePath) const noexcept { return std::hash<std::string>()(filePath.toUtf8()); }
};

/// Specialization of std::formatter for FilePath.
template <>
struct std::formatter<Raz::FilePath, char> : std::formatter<std::string, char> {
  /// Formats the given file path.
  /// \tparam ContextT Type of the formatting context.
  /// \param filePath File path to be formatted.
  /// \param context Formatting context.
  template <typename ContextT>
  constexpr auto format(const Raz::FilePath& filePath, ContextT& context) const {
    return std::formatter<std::string, char>::format(filePath.toUtf8(), context);
  }
};

/// Specialization of std::formatter for FilePath.
template <>
struct std::formatter<Raz::FilePath, wchar_t> : std::formatter<std::wstring, wchar_t> {
  /// Formats the given file path.
  /// \tparam ContextT Type of the formatting context.
  /// \param filePath File path to be formatted.
  /// \param context Formatting context.
  template <typename ContextT>
  constexpr auto format(const Raz::FilePath& filePath, ContextT& context) const {
    return std::formatter<std::wstring, wchar_t>::format(filePath.toWide(), context);
  }
};

#endif // RAZ_FILEPATH_HPP
