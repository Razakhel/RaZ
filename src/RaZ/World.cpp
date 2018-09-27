#include "RaZ/World.hpp"

namespace Raz {

Entity& World::addEntity(bool enabled) {
  m_entities.push_back(Entity::create(m_maxEntityIndex++, enabled));

  if (enabled)
    ++m_enabledEntityCount;

  return *m_entities.back();
}

void World::update(float deltaTime) {
  refresh();

  for (auto& system : m_systems)
    system->update(deltaTime);
}

void World::refresh() {
  // Reorganizing the entites, swapping enabled & disabled ones so that the enabled ones are in front
  auto entityBegin = m_entities.begin();
  auto entityEnd   = (m_entities.begin() + m_enabledEntityCount - 1);

  while (entityBegin != entityEnd) {
    if (!(*entityBegin)->isEnabled()) {
      while (entityBegin != entityEnd && (*entityEnd == nullptr || !(*entityEnd)->isEnabled()))
        --entityEnd;

      if (entityBegin == entityEnd) {
        break;
      } else {
        std::swap(*entityBegin, *entityEnd);
        --entityEnd;
      }
    } else {
      ++entityBegin;
    }
  }

  m_enabledEntityCount = static_cast<std::size_t>(std::distance(m_entities.begin(), entityEnd) + 1);

  for (std::size_t entityIndex = 0; entityIndex < m_enabledEntityCount; ++entityIndex) {
    const auto& entity = m_entities[entityIndex];

    for (auto& system : m_systems) {
      const Bitset matchingComponents = system->getAcceptedComponents() & entity->getEnabledComponents();

      // If the system doesn't contain the entity, check if it should (if the entity possesses the components needed by the system)
      // Else, if the system shouldn't contain the entity, unlink it
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
  for (auto& system : m_systems)
    system->destroy();
}

} // namespace Raz
