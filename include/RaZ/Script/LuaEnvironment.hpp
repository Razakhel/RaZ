#pragma once

#ifndef RAZ_LUAENVIRONMENT_HPP
#define RAZ_LUAENVIRONMENT_HPP

#include <memory>
#include <string>

namespace sol {

template <bool> class basic_reference;
using reference = basic_reference<false>;

template <typename> struct basic_environment;
using environment = basic_environment<reference>;

template <typename> class basic_object;
using object = basic_object<reference>;

} // namespace sol

namespace Raz {

class FilePath;

class LuaEnvironment {
public:
  LuaEnvironment();
  LuaEnvironment(const LuaEnvironment&) = delete;
  LuaEnvironment(LuaEnvironment&&) noexcept = default;

  /// Executes a script from a string.
  /// \param code Code to be executed.
  /// \return True if the script has been executed without errors, false otherwise.
  bool execute(const std::string& code) const;
  /// Executes a script from a file.
  /// \param filePath Path to the script to be executed. Must be valid ASCII; special characters are not handled.
  /// \return True if the script has been executed without errors, false otherwise.
  bool executeFromFile(const FilePath& filePath) const;
  /// Checks if a given global symbol (variable or function) is registered in the environment.
  /// \note Symbols declared as 'local' will NOT be found by this call, as they exist only during the script's execution.
  /// \param name Name of the symbol to be checked.
  /// \return True if the symbol has been found, false otherwise.
  bool exists(const char* name) const;
  /// Clears the environment, removing all existing symbols.
  void clear();

  LuaEnvironment& operator=(const LuaEnvironment&) = delete;
  LuaEnvironment& operator=(LuaEnvironment&&) noexcept = default;

  ~LuaEnvironment();

private:
  sol::object get(const char* name) const;

  std::unique_ptr<sol::environment> m_environment {};
};

} // namespace Raz

#endif // RAZ_LUAENVIRONMENT_HPP
