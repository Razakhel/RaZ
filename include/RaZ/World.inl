#include <algorithm>

namespace Raz {

template <typename Sys>
bool World::hasSystem() const {
  static_assert(std::is_base_of<System, Sys>::value, "Error: Checked system must be derived from System.");

  const std::size_t sysId = System::getId<Sys>();
  return ((sysId < m_systems.size()) && m_systems[sysId]);
}

template <typename Sys>
Sys& World::getSystem() {
  static_assert(std::is_base_of<System, Sys>::value, "Error: Fetched system must be derived from System.");

  if (hasSystem<Sys>())
    return static_cast<Sys&>(*m_systems[System::getId<Sys>()]);

  throw std::runtime_error("Error: No system available of specified type");
}

template <typename Sys, typename... Args>
Sys& World::addSystem(Args&&... args) {
  static_assert(std::is_base_of<System, Sys>::value, "Error: Added system must be derived from System.");

  const std::size_t sysId = System::getId<Sys>();

  if (sysId >= m_systems.size())
    m_systems.resize(sysId + 1);

  m_systems[sysId] = std::make_unique<Sys>(std::forward<Args>(args)...);
  m_activeSystems.setBit(sysId);

  return static_cast<Sys&>(*m_systems[sysId]);
}

template <typename Sys>
std::tuple<Sys&> World::addSystems() {
  static_assert(std::is_base_of<System, Sys>::value, "Error: Added system must be derived from System.");

  return std::forward_as_tuple(addSystem<Sys>());
}

template <typename Sys1, typename Sys2, typename... S>
std::tuple<Sys1&, Sys2&, S...> World::addSystems() {
  static_assert(std::is_base_of<System, Sys1>::value, "Error: Added system must be derived from System.");

  return std::tuple_cat(std::forward_as_tuple(addSystem<Sys1>()), addSystems<Sys2, S...>());
}

template <typename Sys>
void World::removeSystem() {
  static_assert(std::is_base_of<System, Sys>::value, "Error: Removed system must be derived from System.");

  if (hasSystem<Sys>())
    m_systems[System::getId<Sys>()].reset();
}

template <typename Comp, typename... Args>
Entity& World::addEntityWithComponent(bool enabled, Args&&... args) {
  Entity& entity = addEntity(enabled);
  entity.addComponent<Comp>(std::forward<Args>(args)...);

  return entity;
}

template <typename... Comps>
Entity& World::addEntityWithComponents(bool enabled) {
  Entity& entity = addEntity(enabled);
  entity.addComponents<Comps...>();

  return entity;
}

} // namespace Raz
