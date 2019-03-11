#include "RaZ/World.hpp"

namespace Raz {

Entity& World::addEntity(bool enabled) {
  m_entities.push_back(Entity::create(m_maxEntityIndex++, enabled));

  m_activeEntityCount += enabled;

  return *m_entities.back();
}

bool World::update(float deltaTime) {
  refresh();

  for (std::size_t systemIndex = 0; systemIndex < m_systems.size(); ++systemIndex) {
    if (m_activeSystems[systemIndex]) {
      if (!m_systems[systemIndex]->update(deltaTime))
        m_activeSystems.setBit(systemIndex, false);
    }
  }

  return !m_activeSystems.isEmpty();
}

void World::refresh() {
  // Reorganizing the entites, swapping enabled & disabled ones so that the enabled ones are in front
  auto entityBegin = m_entities.begin();
  auto entityEnd   = m_entities.end() - 1;

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

  m_activeEntityCount = static_cast<std::size_t>(std::distance(m_entities.begin(), entityEnd) + 1);

  for (std::size_t entityIndex = 0; entityIndex < m_activeEntityCount; ++entityIndex) {
    const auto& entity = m_entities[entityIndex];

    for (auto& system : m_systems) {
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

} // namespace Raz
