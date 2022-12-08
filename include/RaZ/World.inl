#include <algorithm>

namespace Raz {

template <typename Sys, typename... Args>
Sys& World::addSystem(Args&&... args) {
  static_assert(std::is_base_of_v<System, Sys>, "Error: The added system must be derived from System.");

  const std::size_t sysId = System::getId<Sys>();

  if (sysId >= m_systems.size())
    m_systems.resize(sysId + 1);

  m_systems[sysId] = std::make_unique<Sys>(std::forward<Args>(args)...);
  m_activeSystems.setBit(sysId);

  return static_cast<Sys&>(*m_systems[sysId]);
}

template <typename Sys>
bool World::hasSystem() const {
  static_assert(std::is_base_of_v<System, Sys>, "Error: The checked system must be derived from System.");

  const std::size_t sysId = System::getId<Sys>();
  return ((sysId < m_systems.size()) && m_systems[sysId]);
}

template <typename Sys>
const Sys& World::getSystem() const {
  static_assert(std::is_base_of_v<System, Sys>, "Error: The fetched system must be derived from System.");

  if (hasSystem<Sys>())
    return static_cast<const Sys&>(*m_systems[System::getId<Sys>()]);

  throw std::runtime_error("Error: No system available of specified type");
}

template <typename Sys>
void World::removeSystem() {
  static_assert(std::is_base_of_v<System, Sys>, "Error: The removed system must be derived from System.");

  if (hasSystem<Sys>())
    m_systems[System::getId<Sys>()].reset();
}

template <typename Comp, typename... Args>
Entity& World::addEntityWithComponent(Args&&... args) {
  Entity& entity = addEntity();
  entity.addComponent<Comp>(std::forward<Args>(args)...);

  return entity;
}

template <typename... Comps>
Entity& World::addEntityWithComponents(bool enabled) {
  Entity& entity = addEntity(enabled);
  entity.addComponents<Comps...>();

  return entity;
}

template <typename... Comps>
std::vector<Entity*> World::recoverEntitiesWithComponents() {
  static_assert((std::is_base_of_v<Component, Comps> && ...), "Error: Components to query the entity with must all be derived from Component.");

  std::vector<Entity*> entities;

  for (const EntityPtr& entity : m_entities) {
    if ((entity->hasComponent<Comps>() && ...))
      entities.emplace_back(entity.get());
  }

  return entities;
}

} // namespace Raz
