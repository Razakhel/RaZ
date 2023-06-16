#include "RaZ/Application.hpp"
#include "RaZ/Script/LuaScript.hpp"
#include "RaZ/Utils/FileUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_SAFE_GETTER 0 // Allowing implicit conversion to bool
#define SOL_PRINT_ERRORS 0
#include "sol/sol.hpp"

namespace Raz {

void LuaScript::loadCode(const std::string& code) {
  m_environment.clear();

  if (!m_environment.execute(code))
    throw std::invalid_argument("Error: The given Lua script is invalid");

  if (m_environment.get("update").get_type() != sol::type::function)
    throw std::invalid_argument("Error: A Lua script must have an update() function");
}

void LuaScript::loadCodeFromFile(const FilePath& filePath) {
  loadCode(FileUtils::readFile(filePath));
}

bool LuaScript::update(const FrameTimeInfo& timeInfo) const {
  const sol::function updateFunc = m_environment.get("update");
  return updateFunc(timeInfo);
}

} // namespace Raz
