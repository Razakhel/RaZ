#include "RaZ/Script/LuaEnvironment.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 0
#include "sol/sol.hpp"

namespace Raz {

LuaEnvironment::LuaEnvironment()
  : m_environment{ std::make_unique<sol::environment>(LuaWrapper::getState(), sol::create, LuaWrapper::getState().globals()) } {}

bool LuaEnvironment::execute(const std::string& code) const {
  if (code.empty())
    return false;

  try {
    LuaWrapper::getState().script(code, *m_environment);
  } catch (const sol::error& err) {
    Logger::error("[LuaEnvironment] Error running code: '" + std::string(err.what()) + "'.");
    return false;
  }

  return true;
}

bool LuaEnvironment::executeFromFile(const FilePath& filePath) const {
  if (filePath.isEmpty())
    return false;

  try {
    LuaWrapper::getState().script_file(filePath.toUtf8(), *m_environment);
  } catch (const sol::error& err) {
    Logger::error("[LuaEnvironment] Error running file: '" + std::string(err.what()) + "'.");
    return false;
  }

  return true;
}

bool LuaEnvironment::exists(const char* name) const {
  return get(name).valid();
}

void LuaEnvironment::clear() {
  m_environment->clear();
}

LuaEnvironment::~LuaEnvironment() = default;

sol::object LuaEnvironment::get(const char* name) const {
  return m_environment->operator[](name);
}

} // namespace Raz
