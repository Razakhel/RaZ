#pragma once

#ifndef RAZ_PLUGIN_HPP
#define RAZ_PLUGIN_HPP

namespace Raz {

class FilePath;

class Plugin {
public:
  explicit Plugin(const FilePath& pluginPath) { load(pluginPath); }

  void load(const FilePath& pluginPath);
  bool isLoaded() const noexcept { return (m_pluginHandle != nullptr); }
  template <typename FuncT>
  FuncT loadFunction(const std::string& functionName) { return reinterpret_cast<FuncT>(loadFunctionPointer(functionName)); }
  void close();

  ~Plugin() { close(); }

private:
  void* loadFunctionPointer(const std::string& functionName);

  void* m_pluginHandle {};
};

} // namespace Raz

#endif // RAZ_PLUGIN_HPP
