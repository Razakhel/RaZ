#include <algorithm>

namespace Raz {

template <typename SysT, typename... Args>
SysT& World::addSystem(Args&&... args) {
  static_assert(std::is_base_of_v<System, SysT>, "Error: The added system must be derived from System.");

  const std::size_t systemId = System::getId<SysT>();

  if (systemId >= m_systems.size())
    m_systems.resize(systemId + 1);

  m_systems[systemId] = std::make_unique<SysT>(std::forward<Args>(args)...);
  m_activeSystems.setBit(systemId);

  return static_cast<SysT&>(*m_systems[systemId]);
}

template <typename SysT>
bool World::hasSystem() const {
  static_assert(std::is_base_of_v<System, SysT>, "Error: The checked system must be derived from System.");

  const std::size_t systemId = System::getId<SysT>();
  return ((systemId < m_systems.size()) && m_systems[systemId]);
}

template <typename SysT>
const SysT& World::getSystem() const {
  static_assert(std::is_base_of_v<System, SysT>, "Error: The fetched system must be derived from System.");

  if (hasSystem<SysT>())
    return static_cast<const SysT&>(*m_systems[System::getId<SysT>()]);

  throw std::runtime_error("Error: No system available of specified type");
}

template <typename SysT>
void World::removeSystem() {
  static_assert(std::is_base_of_v<System, SysT>, "Error: The removed system must be derived from System.");

  if (hasSystem<SysT>())
    m_systems[System::getId<SysT>()].reset();
}

template <typename CompT, typename... Args>
Entity& World::addEntityWithComponent(Args&&... args) {
  Entity& entity = addEntity();
  entity.addComponent<CompT>(std::forward<Args>(args)...);

  return entity;
}

template <typename... CompsTs>
Entity& World::addEntityWithComponents(bool enabled) {
  Entity& entity = addEntity(enabled);
  entity.addComponents<CompsTs...>();

  return entity;
}

template <typename... CompsTs>
std::vector<Entity*> World::recoverEntitiesWithComponents() {
  static_assert((std::is_base_of_v<Component, CompsTs> && ...), "Error: The components to query the entity with must all be derived from Component.");

  std::vector<Entity*> entities;

  for (const EntityPtr& entity : m_entities) {
    if ((entity->hasComponent<CompsTs>() && ...))
      entities.emplace_back(entity.get());
  }

  return entities;
}

} // namespace Raz
