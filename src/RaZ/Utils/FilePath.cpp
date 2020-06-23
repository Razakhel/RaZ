#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

namespace Raz {

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

FilePath FilePath::recoverPathToFile(const StringType& filePath) {
  return filePath.substr(0, filePath.find_last_of('/') + 1);
}

FilePath FilePath::recoverFileName(const StringType& filePath, bool keepExtension) {
  auto fileName = filePath.substr(filePath.find_last_of('/') + 1);

  if (!keepExtension)
    fileName = fileName.erase(fileName.find_last_of('.'));

  return fileName;
}

FilePath FilePath::recoverExtension(const StringType& filePath) {
  return filePath.substr(filePath.find_last_of('.') + 1);
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

} // namespace Raz
