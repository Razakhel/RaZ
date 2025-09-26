#include "RaZ/Entity.hpp"
#include "RaZ/Script/LuaEnvironment.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 0
#include "sol/sol.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

LuaEnvironment::LuaEnvironment()
  : m_environment{ std::make_unique<sol::environment>(LuaWrapper::getState(), sol::create, LuaWrapper::getState().globals()) } {}

bool LuaEnvironment::execute(const std::string& code) const {
  ZoneScopedN("LuaEnvironment::execute");

  if (code.empty())
    return false;

  Logger::debug("[LuaEnvironment] Executing code...");

  try {
    LuaWrapper::getState().script(code, *m_environment);
  } catch (const sol::error& err) {
    Logger::error("[LuaEnvironment] Error executing code: '{}'", err.what());
    return false;
  }

  Logger::debug("[LuaEnvironment] Executed code");

  return true;
}

bool LuaEnvironment::executeFromFile(const FilePath& filePath) const {
  ZoneScopedN("LuaEnvironment::executeFromFile");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  if (filePath.isEmpty())
    return false;

  Logger::debug("[LuaEnvironment] Executing code from file ('{}')...", filePath);

  try {
    LuaWrapper::getState().script_file(filePath.toUtf8(), *m_environment);
  } catch (const sol::error& err) {
    Logger::error("[LuaEnvironment] Error executing code from file: '{}'", err.what());
    return false;
  }

  Logger::debug("[LuaEnvironment] Executed code from file");

  return true;
}

bool LuaEnvironment::exists(const char* name) const {
  return get(name).valid();
}

void LuaEnvironment::clear() {
  Logger::debug("[LuaEnvironment] Clearing environment...");
  m_environment->clear();
  Logger::debug("[LuaEnvironment] Cleared environment");
}

LuaEnvironment::~LuaEnvironment() = default;

void LuaEnvironment::registerEntity(const Entity& entity, const std::string& name) {
  Logger::debug("[LuaEnvironment] Registering entity (ID: {}) as '{}'...", entity.getId(), name);
  m_environment->operator[](name) = &entity;
  Logger::debug("[LuaEnvironment] Registered entity");
}

sol::reference LuaEnvironment::get(const char* name) const {
  return m_environment->operator[](name);
}

} // namespace Raz
