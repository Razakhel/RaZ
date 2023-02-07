#include <type_traits>

namespace Raz {

template <typename CompT>
std::size_t Component::getId() {
  static_assert(std::is_base_of_v<Component, CompT>, "Error: The fetched component must be derived from Component.");
  static_assert(!std::is_same_v<Component, CompT>, "Error: The fetched component must not be of specific type 'Component'.");

  static const std::size_t id = s_maxId++;
  return id;
}

} // namespace Raz
