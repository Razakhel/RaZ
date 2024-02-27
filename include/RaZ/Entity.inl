#include <stdexcept>

namespace Raz {

template <typename CompT, typename... Args>
CompT& Entity::addComponent(Args&&... args) {
  static_assert(std::is_base_of_v<Component, CompT>, "Error: The added component must be derived from Component.");

  const std::size_t compId = Component::getId<CompT>();

  if (compId >= m_components.size())
    m_components.resize(compId + 1);

  m_components[compId] = std::make_unique<CompT>(std::forward<Args>(args)...);
  m_enabledComponents.setBit(compId);

  return static_cast<CompT&>(*m_components[compId]);
}

template <typename CompT>
std::tuple<CompT&> Entity::addComponents() {
  static_assert(std::is_base_of_v<Component, CompT>, "Error: The added component must be derived from Component.");

  return std::forward_as_tuple(addComponent<CompT>());
}

template <typename CompT1, typename CompT2, typename... C>
std::tuple<CompT1&, CompT2&, C...> Entity::addComponents() {
  static_assert(std::is_base_of_v<Component, CompT1>, "Error: The added component must be derived from Component.");

  return std::tuple_cat(std::forward_as_tuple(addComponent<CompT1>()), addComponents<CompT2, C...>());
}

template <typename CompT>
bool Entity::hasComponent() const {
  static_assert(std::is_base_of_v<Component, CompT>, "Error: The checked component must be derived from Component.");

  const std::size_t compId = Component::getId<CompT>();
  return ((compId < m_components.size()) && m_enabledComponents[compId]);
}

template <typename CompT>
const CompT& Entity::getComponent() const {
  static_assert(std::is_base_of_v<Component, CompT>, "Error: The fetched component must be derived from Component.");

  if (hasComponent<CompT>())
    return static_cast<const CompT&>(*m_components[Component::getId<CompT>()]);

  throw std::runtime_error("Error: No component available of specified type");
}

template <typename CompT>
void Entity::removeComponent() {
  static_assert(std::is_base_of_v<Component, CompT>, "Error: The removed component must be derived from Component.");

  if (hasComponent<CompT>()) {
    const std::size_t compId = Component::getId<CompT>();

    m_components[compId].reset();
    m_enabledComponents.setBit(compId, false);
  }
}

} // namespace Raz
