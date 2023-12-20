#include "RaZ/Application.hpp"
#include "RaZ/Script/LuaScript.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Logger.hpp"

#define SOL_SAFE_GETTER 0 // Allowing implicit conversion to bool
#include "sol/sol.hpp"

namespace Raz {

LuaScript::LuaScript(const std::string& code) {
  Logger::debug("[LuaScript] Creating script...");

  Raz::LuaWrapper::registerTypes();
  loadCode(code);

  Logger::debug("[LuaScript] Created script");
}

void LuaScript::loadCode(const std::string& code) {
  Logger::debug("[LuaScript] Loading code...");

  const sol::object owningEntity = m_environment.get("this");

  m_environment.clear();

  if (!m_environment.execute(code))
    throw std::invalid_argument("Error: The given Lua script is invalid");

  if (m_environment.get("update").get_type() != sol::type::function)
    throw std::invalid_argument("Error: A Lua script must have an update() function");

  if (owningEntity.is<Entity>()) {
    m_environment.registerEntity(owningEntity.as<Entity>(), "this");
    setup();
  }

  Logger::debug("[LuaScript] Loaded code");
}

void LuaScript::loadCodeFromFile(const FilePath& filePath) {
  Logger::debug("[LuaScript] Loading code from file ('" + filePath + "')...");
  loadCode(FileUtils::readFileToString(filePath));
  Logger::debug("[LuaScript] Loaded code from file");
}

bool LuaScript::update(const FrameTimeInfo& timeInfo) const {
  const sol::unsafe_function updateFunc = m_environment.get("update");
  const sol::unsafe_function_result updateRes = updateFunc(timeInfo);
  return (updateRes.get_type() == sol::type::none || updateRes);
}

void LuaScript::setup() const {
  if (!m_environment.exists("setup"))
    return;

  const sol::reference setupRef = m_environment.get("setup");

  if (setupRef.get_type() != sol::type::function)
    return;

  Logger::debug("[LuaScript] Running script setup...");

  const sol::unsafe_function setupFunc = setupRef;

  if (!setupFunc().valid())
    throw std::runtime_error("Error: The Lua script failed to be setup");

  Logger::debug("[LuaScript] Ran script setup");
}

} // namespace Raz
