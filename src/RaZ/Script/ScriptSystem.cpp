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

  auto& luaScript = entity->getComponent<LuaScript>();
  luaScript.registerEntity(*entity, "this");
  luaScript.setup();
}

} // namespace Raz
