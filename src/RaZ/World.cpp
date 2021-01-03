#include "RaZ/World.hpp"

namespace Raz {

Entity& World::addEntity(bool enabled) {
  m_entities.emplace_back(Entity::create(m_maxEntityIndex++, enabled));
  m_activeEntityCount += enabled;

  return *m_entities.back();
}

bool World::update(float deltaTime) {
  refresh();

  // The fixed time step may need to be user-definable later; moreover, it should probably be handled by the Application
  constexpr float fixedTimeStep = 0.016666f;

  for (std::size_t systemIndex = 0; systemIndex < m_systems.size(); ++systemIndex) {
    if (!m_activeSystems[systemIndex])
      continue;

    System& system = *m_systems[systemIndex];

    bool isSystemActive = system.update(deltaTime);
    m_remainingTime    += deltaTime;

    while (m_remainingTime >= fixedTimeStep) {
      isSystemActive   = system.step(fixedTimeStep) && isSystemActive;
      m_remainingTime -= fixedTimeStep;
    }

    if (!isSystemActive)
      m_activeSystems.setBit(systemIndex, false);
  }

  return !m_activeSystems.isEmpty();
}

void World::refresh() {
  if (m_entities.empty())
    return;

  sortEntities();

  for (std::size_t entityIndex = 0; entityIndex < m_activeEntityCount; ++entityIndex) {
    const EntityPtr& entity = m_entities[entityIndex];

    for (const SystemPtr& system : m_systems) {
      const Bitset matchingComponents = system->getAcceptedComponents() & entity->getEnabledComponents();

      // If the system doesn't contain the entity, check if it should (possesses the accepted components); if yes, link it
      // Else, if the system contains the entity but shouldn't, unlink it
      if (!system->containsEntity(entity)) {
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
  // Entities must be released before the systems, since their destruction may depend on those
  m_entities.clear();
  m_activeEntityCount = 0;
  m_maxEntityIndex    = 0;

  // This means that no entity must be used in any system destructor, since they will all be invalid
  // Their list is thus cleared to avoid any invalid usage
  for (SystemPtr& system : m_systems)
    system->m_entities.clear();

  m_systems.clear();
  m_activeSystems.clear();
}

void World::sortEntities() {
  // Reorganizing the entites, swapping enabled & disabled ones so that the enabled ones are in front
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

} // namespace Raz
