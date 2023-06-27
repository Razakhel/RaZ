#include "RaZ/Script/LuaScript.hpp"
#include "RaZ/Script/ScriptSystem.hpp"

namespace Raz {

ScriptSystem::ScriptSystem() {
  registerComponents<LuaScript>();
}

bool ScriptSystem::update(const FrameTimeInfo& timeInfo) {
  bool res = true;

  for (const Entity* entity : m_entities) {
    res = entity->getComponent<LuaScript>().update(timeInfo) && res;
  }

  return res;
}

void ScriptSystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);

  if (!entity->getComponent<LuaScript>().setup())
    throw std::invalid_argument("Error: The Lua script failed to be setup");
}

} // namespace Raz
