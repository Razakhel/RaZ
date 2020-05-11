#include "RaZ/Utils/FilePath.hpp"

#include <codecvt>
#include <locale>

namespace Raz {

FilePath::FilePath(const char* pathStr)
#if defined(RAZ_PLATFORM_WINDOWS) && !defined(RAZ_PLATFORM_CYGWIN)
  : m_path{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(pathStr) } {}
#else
  : m_path{ pathStr } {}
#endif

FilePath::FilePath(const wchar_t* pathStr)
#if defined(RAZ_PLATFORM_WINDOWS) && !defined(RAZ_PLATFORM_CYGWIN)
  : m_path{ pathStr } {}
#else
  : m_path{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(pathStr) } {}
#endif

} // namespace Raz
