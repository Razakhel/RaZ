#include "RaZ/System.hpp"

namespace Raz {

bool System::containsEntity(const EntityPtr& entity) {
  for (const auto& entityPtr : m_entities) {
    if (entityPtr->getId() == entity->getId())
      return true;
  }

  return false;
}

void System::linkEntity(const EntityPtr& entity) {
  if (entity->getId() >= m_entities.size())
    m_entities.resize(entity->getId() + 1);

  m_entities[entity->getId()] = entity.get();
}

void System::unlinkEntity(const EntityPtr& entity) {
  m_entities[entity->getId()] = nullptr;
}

std::size_t System::m_maxId = 0;

} // namespace Raz
