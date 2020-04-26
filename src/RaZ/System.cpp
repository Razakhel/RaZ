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
  m_entities.emplace_back(entity.get());
}

void System::unlinkEntity(const EntityPtr& entity) {
  for (std::size_t entityIndex = 0; entityIndex < m_entities.size(); ++entityIndex) {
    if (m_entities[entityIndex]->getId() == entity->getId()) {
      m_entities.erase(m_entities.begin() + static_cast<std::ptrdiff_t>(entityIndex));
      break;
    }
  }
}

} // namespace Raz
