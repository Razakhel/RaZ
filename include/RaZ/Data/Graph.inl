#include <algorithm>
#include <cassert>
#include <stdexcept>

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
template <typename... OtherNodesTs>
void GraphNode<T>::addParents(GraphNode& node, OtherNodesTs&&... otherNodes) {
  if (&node == this)
    throw std::invalid_argument("Error: A graph node cannot be a parent of itself");

  if (std::find(m_parents.cbegin(), m_parents.cend(), &node) == m_parents.cend())
    m_parents.emplace_back(static_cast<T*>(&node));

  if (std::find(node.m_children.cbegin(), node.m_children.cend(), this) == node.m_children.cend())
    node.m_children.emplace_back(static_cast<T*>(this));

  // Stop the recursive unpacking if no more nodes are to be added as parents
  if constexpr (sizeof...(otherNodes) > 0)
    addParents(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename T>
template <typename... OtherNodesTs>
void GraphNode<T>::removeParents(GraphNode& node, OtherNodesTs&&... otherNodes) {
  unlinkParent(node);
  node.unlinkChild(*this);

  // Stop the recursive unpacking if no more nodes are to be removed as parents
  if constexpr (sizeof...(otherNodes) > 0)
    removeParents(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename T>
template <typename... OtherNodesTs>
void GraphNode<T>::addChildren(GraphNode& node, OtherNodesTs&&... otherNodes) {
  if (&node == this)
    throw std::invalid_argument("Error: A graph node cannot be a child of itself");

  if (std::find(m_children.cbegin(), m_children.cend(), &node) == m_children.cend())
    m_children.emplace_back(static_cast<T*>(&node));

  if (std::find(node.m_parents.cbegin(), node.m_parents.cend(), this) == node.m_parents.cend())
    node.m_parents.emplace_back(static_cast<T*>(this));

  // Stop the recursive unpacking if no more nodes are to be added as children
  if constexpr (sizeof...(otherNodes) > 0)
    addChildren(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename T>
template <typename... OtherNodesTs>
void GraphNode<T>::removeChildren(GraphNode& node, OtherNodesTs&&... otherNodes) {
  unlinkChild(node);
  node.unlinkParent(*this);

  // Stop the recursive unpacking if no more nodes are to be removed as children
  if constexpr (sizeof...(otherNodes) > 0)
    removeChildren(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename T>
void GraphNode<T>::unlinkParent(const GraphNode& node) {
  if (&node == this)
    throw std::invalid_argument("Error: A graph node cannot be unlinked from itself");

  const auto parentIt = std::find(m_parents.cbegin(), m_parents.cend(), &node);
  if (parentIt != m_parents.cend())
    m_parents.erase(parentIt);
}

template <typename T>
void GraphNode<T>::unlinkChild(const GraphNode& node) {
  if (&node == this)
    throw std::invalid_argument("Error: A graph node cannot be unlinked from itself");

  const auto childIt = std::find(m_children.cbegin(), m_children.cend(), &node);
  if (childIt != m_children.cend())
    m_children.erase(childIt);
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

template <typename NodeT>
void Graph<NodeT>::removeNode(NodeT& node) {
  const auto nodeIt = std::find_if(m_nodes.cbegin(), m_nodes.cend(), [&node] (const NodePtr& nodePtr) { return (nodePtr.get() == &node); });

  if (nodeIt == m_nodes.cend())
    throw std::invalid_argument("Error: The graph node to be removed does not exist");

  for (NodeT* parent : node.m_parents)
    parent->unlinkChild(node);

  for (NodeT* child : node.m_children)
    child->unlinkParent(node);

  m_nodes.erase(nodeIt);
}

} // namespace Raz
