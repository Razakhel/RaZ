#pragma once

#ifndef RAZ_PLUGIN_HPP
#define RAZ_PLUGIN_HPP

#include <cstring>
#include <string>

namespace Raz {

class FilePath;

class Plugin {
public:
  Plugin() = default;
  explicit Plugin(const FilePath& pluginPath) noexcept { load(pluginPath); }

  void load(const FilePath& pluginPath) noexcept;
  bool isLoaded() const noexcept { return (m_pluginHandle != nullptr); }
  template <typename FuncT>
  FuncT loadFunction(const std::string& functionName) noexcept {
    static_assert(sizeof(FuncT) == sizeof(void*));

    FuncT func {};

    void* funcPtr = loadFunctionPointer(functionName);
    std::memcpy(&func, &funcPtr, sizeof(FuncT));

    return func;
  }
  void close() noexcept;

  ~Plugin() { close(); }

private:
  void* loadFunctionPointer(const std::string& functionName) noexcept;

  void* m_pluginHandle {};
};

} // namespace Raz

#endif // RAZ_PLUGIN_HPP
