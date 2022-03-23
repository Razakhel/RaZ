#include <algorithm>
#include <cassert>

namespace Raz {

template <typename T>
const T& GraphNode<T>::getParent(std::size_t index) const noexcept {
  assert("Error: The requested parent node is out of bounds." && index < m_parents.size());
  return *m_parents[index];
}

template <typename T>
T& GraphNode<T>::getParent(std::size_t index) noexcept {
  assert("Error: The requested parent node is out of bounds." && index < m_parents.size());
  return *m_parents[index];
}

template <typename T>
const T& GraphNode<T>::getChild(std::size_t index) const noexcept {
  assert("Error: The requested child node is out of bounds." && index < m_children.size());
  return *m_children[index];
}

template <typename T>
T& GraphNode<T>::getChild(std::size_t index) noexcept {
  assert("Error: The requested child node is out of bounds." && index < m_children.size());
  return *m_children[index];
}

template <typename T>
template <typename NodeT, typename... OtherNodesTs>
void GraphNode<T>::addParents(NodeT&& node, OtherNodesTs&&... otherNodes) {
  static_assert(std::is_same_v<T, std::decay_t<NodeT>>, "Error: The given objects' type must be the same as the node's one.");
  static_assert(std::is_base_of_v<GraphNode, std::decay_t<NodeT>>,
                "Error: A graph node can only have other objects of types derived from GraphNode as parents.");

  assert("Error: A graph node cannot have itself as a parent." && &node != this);

  if (std::find(m_parents.cbegin(), m_parents.cend(), &node) == m_parents.cend())
    m_parents.emplace_back(&node);

  if (std::find(node.m_children.cbegin(), node.m_children.cend(), this) == node.m_children.cend())
    node.m_children.emplace_back(static_cast<T*>(this));

  // Stop the recursive unpacking if no more nodes are to be added as parents
  if constexpr (sizeof...(otherNodes) > 0)
    addParents(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename T>
template <typename NodeT, typename... OtherNodesTs>
void GraphNode<T>::addChildren(NodeT&& node, OtherNodesTs&&... otherNodes) {
  static_assert(std::is_same_v<T, std::decay_t<NodeT>>, "Error: The given objects' type must be the same as the node's one.");
  static_assert(std::is_base_of_v<GraphNode, std::decay_t<NodeT>>,
                "Error: A graph node can only have other objects of types derived from GraphNode as children.");

  assert("Error: A graph node cannot be a child of itself." && &node != this);

  if (std::find(m_children.cbegin(), m_children.cend(), &node) == m_children.cend())
    m_children.emplace_back(&node);

  if (std::find(node.m_parents.cbegin(), node.m_parents.cend(), this) == node.m_parents.cend())
    node.m_parents.emplace_back(static_cast<T*>(this));

  // Stop the recursive unpacking if no more nodes are to be added as children
  if constexpr (sizeof...(otherNodes) > 0)
    addChildren(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename NodeT>
const NodeT& Graph<NodeT>::getNode(std::size_t index) const noexcept {
  assert("Error: The requested node is out of bounds." && index < m_nodes.size());
  return *m_nodes[index];
}

template <typename NodeT>
NodeT& Graph<NodeT>::getNode(std::size_t index) noexcept {
  assert("Error: The requested node is out of bounds." && index < m_nodes.size());
  return *m_nodes[index];
}

template <typename NodeT>
template <typename... Args>
NodeT& Graph<NodeT>::addNode(Args&& ... args) {
  m_nodes.emplace_back(std::make_unique<NodeT>(std::forward<Args>(args)...));
  return *m_nodes.back();
}

} // namespace Raz
