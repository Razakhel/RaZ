namespace Raz {

template <typename Comp>
std::size_t Component::getId() {
  static_assert(std::is_base_of<Component, Comp>::value, "Error: Fetched component must be derived from Component.");
  static_assert(!std::is_same<Component, Comp>::value, "Error: Fetched component must not be of specific type 'Component'.");

  static const std::size_t id = m_maxId++;
  return id;
}

} // namespace Raz
