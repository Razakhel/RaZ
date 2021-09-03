#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <iostream>

namespace Raz {

namespace {

constexpr std::size_t recoverLastSeparatorPos(std::size_t firstSeparatorPos, std::size_t secondSeparatorPos, std::size_t invalidPos) {
  if (firstSeparatorPos == invalidPos)
    return secondSeparatorPos;

  // The first separator has been found

  if (secondSeparatorPos == invalidPos)
    return firstSeparatorPos;

  // Both separators have been found; the max (rightmost) position must be picked

  return std::max(firstSeparatorPos, secondSeparatorPos);
}

inline std::size_t recoverLastSeparatorPos(const std::string& pathStr) {
  const std::size_t lastSlashPos     = pathStr.find_last_of('/');
  const std::size_t lastBackslashPos = pathStr.find_last_of('\\');

  return recoverLastSeparatorPos(lastSlashPos, lastBackslashPos, std::string::npos);
}

inline std::size_t recoverLastSeparatorPos(const std::wstring& pathStr) {
  const std::size_t lastSlashPos     = pathStr.find_last_of(L'/');
  const std::size_t lastBackslashPos = pathStr.find_last_of(L'\\');

  return recoverLastSeparatorPos(lastSlashPos, lastBackslashPos, std::wstring::npos);
}

} // namespace

FilePath::FilePath(const char* pathStr)
#if defined(RAZ_PLATFORM_WINDOWS) && !defined(RAZ_PLATFORM_CYGWIN)
  : m_path{ StrUtils::toWide(pathStr) } {}
#else
  : m_path{ pathStr } {}
#endif

FilePath::FilePath(const wchar_t* pathStr)
#if defined(RAZ_PLATFORM_WINDOWS) && !defined(RAZ_PLATFORM_CYGWIN)
  : m_path{ pathStr } {}
#else
  : m_path{ StrUtils::toUtf8(pathStr) } {}
#endif

FilePath FilePath::recoverPathToFile(const std::string& pathStr) {
  return pathStr.substr(0, recoverLastSeparatorPos(pathStr) + 1);
}

FilePath FilePath::recoverPathToFile(const std::wstring& pathStr) {
  return pathStr.substr(0, recoverLastSeparatorPos(pathStr) + 1);
}

FilePath FilePath::recoverFileName(const std::string& pathStr, bool keepExtension) {
  std::string fileName = pathStr.substr(recoverLastSeparatorPos(pathStr) + 1);

  if (!keepExtension) {
    const std::size_t lastPointPos = fileName.find_last_of('.');

    if (lastPointPos != std::string::npos)
      fileName = fileName.erase(lastPointPos);
  }

  return fileName;
}

FilePath FilePath::recoverFileName(const std::wstring& pathStr, bool keepExtension) {
  std::wstring fileName = pathStr.substr(recoverLastSeparatorPos(pathStr) + 1);

  if (!keepExtension) {
    const std::size_t lastPointPos = fileName.find_last_of(L'.');

    if (lastPointPos != std::wstring::npos)
      fileName = fileName.erase(lastPointPos);
  }

  return fileName;
}

FilePath FilePath::recoverExtension(const std::string& pathStr) {
  const std::size_t lastPointPos = pathStr.find_last_of('.');

  if (lastPointPos == std::string::npos)
    return FilePath();

  return pathStr.substr(pathStr.find_last_of('.') + 1);
}

FilePath FilePath::recoverExtension(const std::wstring& pathStr) {
  const std::size_t lastPointPos = pathStr.find_last_of(L'.');

  if (lastPointPos == std::wstring::npos)
    return FilePath();

  return pathStr.substr(pathStr.find_last_of(L'.') + 1);
}

#if defined(RAZ_PLATFORM_WINDOWS) && !defined(RAZ_PLATFORM_CYGWIN)
std::string FilePath::toUtf8() const {
  return StrUtils::toUtf8(m_path);
}
#else
std::wstring FilePath::toWide() const {
  return StrUtils::toWide(m_path);
}
#endif

std::ostream& operator<<(std::ostream& stream, const FilePath& filePath) {
  stream << filePath.toUtf8();
  return stream;
}

std::wostream& operator<<(std::wostream& stream, const FilePath& filePath) {
  stream << filePath.toWide();
  return stream;
}

} // namespace Raz
