#pragma once

#ifndef RAZ_LUAWRAPPER_HPP
#define RAZ_LUAWRAPPER_HPP

#include <string>

namespace sol { class state; }

namespace Raz {

class FilePath;

class LuaWrapper {
public:
  static void registerTypes();
  /// Executes a script from a string.
  /// \param code Code to be executed.
  /// \return True if the script has been executed without errors, false otherwise.
  static bool execute(const std::string& code);
  /// Executes a script from a file.
  /// \param filePath Path to the script to be executed. Must be valid ASCII; special characters are not handled.
  /// \return True if the script has been executed without errors, false otherwise.
  static bool executeFromFile(const FilePath& filePath);

private:
  static void registerMathTypes();

  static sol::state& getState();
};

} // namespace Raz

#endif // RAZ_LUAWRAPPER_HPP
