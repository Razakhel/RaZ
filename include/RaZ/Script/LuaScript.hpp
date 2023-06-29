#pragma once

#ifndef RAZ_LUASCRIPT_HPP
#define RAZ_LUASCRIPT_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Script/LuaEnvironment.hpp"

#include <string>

namespace Raz {

class FilePath;
struct FrameTimeInfo;

class LuaScript final : public Component {
  friend class ScriptSystem;

public:
  explicit LuaScript(const std::string& code);

  const LuaEnvironment& getEnvironment() const { return m_environment; }

  /// Loads a script from a string.
  /// \note The script must contain a function named update().
  /// \note This clears the script's environment, effectively unregistering all existing symbols.
  /// \param code Code to be loaded.
  void loadCode(const std::string& code);
  /// Loads a script from a file.
  /// \note The script must contain a function named update().
  /// \note This clears the script's environment, effectively unregistering all existing symbols.
  /// \param filePath Path to the script to be loaded.
  void loadCodeFromFile(const FilePath& filePath);
  /// Registers an entity to a variable, making it accessible from the script.
  /// \param entity Entity to be registered.
  /// \param name Name of the variable to bind the entity to.
  void registerEntity(const Entity& entity, const std::string& name) { m_environment.registerEntity(entity, name); }
  /// Executes the script's update function.
  /// \warning If the update function does not return anything or returns nil, this call will always return false.
  /// \warning If the update function does return anything other than a boolean, this call will always return true.
  /// \param timeInfo Time-related frame information.
  /// \retval true If the update function returned true or any non-boolean value (excluding nil).
  /// \retval false If the update function returned false, nil or nothing.
  bool update(const FrameTimeInfo& timeInfo) const;

private:
  /// Executes the script's setup function. Does nothing if none exists.
  /// \throws std::runtime_error If the setup function has not been executed correctly.
  void setup() const;

  LuaEnvironment m_environment {};
};

} // namespace Raz

#endif // RAZ_LUASCRIPT_HPP
