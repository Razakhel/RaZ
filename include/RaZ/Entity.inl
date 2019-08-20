namespace Raz {

template <typename Comp>
bool Entity::hasComponent() const {
  static_assert(std::is_base_of_v<Component, Comp>, "Error: Checked component must be derived from Component.");

  const std::size_t compId = Component::getId<Comp>();
  return ((compId < m_components.size()) && m_enabledComponents[compId]);
}

template <typename Comp>
const Comp& Entity::getComponent() const {
  static_assert(std::is_base_of_v<Component, Comp>, "Error: Fetched component must be derived from Component.");

  if (hasComponent<Comp>())
    return static_cast<const Comp&>(*m_components[Component::getId<Comp>()]);

  throw std::runtime_error("Error: No component available of specified type");
}

template <typename Comp, typename... Args>
Comp& Entity::addComponent(Args&&... args) {
  static_assert(std::is_base_of_v<Component, Comp>, "Error: Added component must be derived from Component.");

  const std::size_t compId = Component::getId<Comp>();

  if (compId >= m_components.size())
    m_components.resize(compId + 1);

  m_components[compId] = std::make_unique<Comp>(std::forward<Args>(args)...);
  m_enabledComponents.setBit(compId);

  return static_cast<Comp&>(*m_components[compId]);
}

template <typename Comp>
std::tuple<Comp&> Entity::addComponents() {
  static_assert(std::is_base_of_v<Component, Comp>, "Error: Added component must be derived from Component.");

  return std::forward_as_tuple(addComponent<Comp>());
}

template <typename Comp1, typename Comp2, typename... C>
std::tuple<Comp1&, Comp2&, C...> Entity::addComponents() {
  static_assert(std::is_base_of_v<Component, Comp1>, "Error: Added component must be derived from Component.");

  return std::tuple_cat(std::forward_as_tuple(addComponent<Comp1>()), addComponents<Comp2, C...>());
}

template <typename Comp>
void Entity::removeComponent() {
  static_assert(std::is_base_of_v<Component, Comp>, "Error: Removed component must be derived from Component.");

  if (hasComponent<Comp>()) {
    const std::size_t compId = Component::getId<Comp>();

    m_components[compId].reset();
    m_enabledComponents.setBit(compId, false);
  }
}

} // namespace Raz
