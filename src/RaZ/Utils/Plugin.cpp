#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Plugin.hpp"

#if defined(RAZ_PLATFORM_WINDOWS)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace Raz {

void Plugin::load(const FilePath& pluginPath) noexcept {
  close();

#if defined(RAZ_PLATFORM_WINDOWS)
  m_pluginHandle = LoadLibraryW(pluginPath.toWide().c_str());
#else
  m_pluginHandle = dlopen(pluginPath.toUtf8().c_str(), RTLD_LAZY);
#endif
}

void Plugin::close() noexcept {
  if (m_pluginHandle == nullptr)
    return;

#if defined(RAZ_PLATFORM_WINDOWS)
  FreeLibrary(static_cast<HMODULE>(m_pluginHandle));
#else
  dlclose(m_pluginHandle);
#endif

  m_pluginHandle = nullptr;
}

void* Plugin::loadFunctionPointer(const std::string& functionName) noexcept {
#if defined(RAZ_PLATFORM_WINDOWS)
  static_assert(sizeof(void*) == sizeof(FARPROC));

  void* res {};

  FARPROC funcPtr = GetProcAddress(static_cast<HMODULE>(m_pluginHandle), functionName.c_str());
  std::memcpy(&res, &funcPtr, sizeof(void*));

  return res;
#else
  return dlsym(m_pluginHandle, functionName.c_str());
#endif
}

} // namespace Raz
