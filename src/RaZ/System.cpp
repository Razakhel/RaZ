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
  m_entities.push_back(entity.get());
}

void System::unlinkEntity(const EntityPtr& entity) {
  for (std::size_t entityIndex = 0; entityIndex < m_entities.size(); ++entityIndex) {
    if (m_entities[entityIndex]->getId() == entity->getId()) {
      m_entities.erase(m_entities.begin() + entityIndex);
      break;
    }
  }
}

std::size_t System::m_maxId = 0;

} // namespace Raz
