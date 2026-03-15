#include "RaZ/World.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

Entity& World::addEntity(bool enabled) {
  m_entities.emplace_back(Entity::create(m_maxEntityIndex++, enabled));
  m_activeEntityCount += enabled;

  return *m_entities.back();
}

bool World::update(const FrameTimeInfo& timeInfo) {
  ZoneScopedN("World::update");

  refresh();

  for (std::size_t systemIndex = 0; systemIndex < m_systems.size(); ++systemIndex) {
    if (!m_activeSystems[systemIndex])
      continue;

    const bool isSystemActive = m_systems[systemIndex]->update(timeInfo);

    if (!isSystemActive)
      m_activeSystems.setBit(systemIndex, false);
  }

  return !m_activeSystems.isEmpty();
}

void World::refresh() {
  ZoneScopedN("World::refresh");

  cleanEntities();

  if (m_entities.empty())
    return;

  sortEntities();

  for (std::size_t entityIndex = 0; entityIndex < m_activeEntityCount; ++entityIndex) {
    const EntityPtr& entity = m_entities[entityIndex];

    if (!entity->isEnabled())
      continue;

    for (std::size_t systemIndex = 0; systemIndex < m_systems.size(); ++systemIndex) {
      const SystemPtr& system = m_systems[systemIndex];

      if (system == nullptr || !m_activeSystems[systemIndex])
        continue;

      const Bitset matchingComponents = system->getAcceptedComponents() & entity->getEnabledComponents();

      // If the system does not contain the entity, check if it should (if it possesses the accepted components); if yes, link it
      // Else, if the system contains the entity but should not, unlink it
      if (!system->containsEntity(*entity)) {
        if (!matchingComponents.isEmpty())
          system->linkEntity(entity);
      } else {
        if (matchingComponents.isEmpty())
          system->unlinkEntity(entity);
      }
    }
  }
}

void World::destroy() {
  ZoneScopedN("World::destroy");

  // Entities must be released before the systems, since their destruction may depend on those
  m_entities.clear();
  m_activeEntityCount = 0;
  m_maxEntityIndex    = 0;

  // This means that no entity must be used in any system destructor, since they will all be invalid
  // Their list is thus cleared to avoid any invalid usage
  for (const SystemPtr& system : m_systems) {
    if (system)
      system->m_entities.clear();
  }

  m_systems.clear();
  m_activeSystems.clear();
}

void World::sortEntities() {
  ZoneScopedN("World::sortEntities");

  // Reorganizing the entities, swapping enabled & disabled ones so that the enabled ones are in front
  auto firstEntity = m_entities.begin();
  auto lastEntity  = m_entities.end() - 1;

  while (firstEntity != lastEntity) {
    // Iterating from the beginning to the end, trying to find a disabled entity
    if ((*firstEntity)->isEnabled()) {
      ++firstEntity;
      continue;
    }

    // Iterating from the end to the beginning, trying to find an enabled entity
    while (firstEntity != lastEntity && (*lastEntity == nullptr || !(*lastEntity)->isEnabled()))
      --lastEntity;

    // If both iterators are equal to each other, the list is sorted
    if (firstEntity == lastEntity)
      break;

    std::swap(*firstEntity, *lastEntity);
    --lastEntity;
  }

  m_activeEntityCount = static_cast<std::size_t>(std::distance(m_entities.begin(), lastEntity) + 1);
}

void World::cleanEntities() {
  for (const Entity* entity : m_entitiesToRemove) {
    const auto entityIter = std::ranges::find_if(m_entities, [entity] (const EntityPtr& entityPtr) noexcept {
      return (entity == entityPtr.get());
    });

    if (entityIter == m_entities.end())
      throw std::invalid_argument("[World] The entity to be removed isn't owned by this world");

    for (const SystemPtr& system : m_systems)
      system->unlinkEntity(*entityIter);

    m_entities.erase(entityIter);
  }

  m_entitiesToRemove.clear();
}

} // namespace Raz
