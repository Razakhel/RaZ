namespace Raz {

template <typename T>
template <typename NodeT, typename... OtherNodesTs>
void GraphNode<T>::addChildren(NodeT&& node, OtherNodesTs&&... otherNodes) {
  static_assert(std::is_same_v<T, std::decay_t<NodeT>>, "Error: The given objects' type must be the same as the node's one.");
  static_assert(std::is_base_of_v<GraphNode, std::decay_t<NodeT>>,
                "Error: A graph node can only have other objects of types derived from GraphNode as children.");

  assert("Error: A GraphNode cannot be a child of itself." && &node != this);

  if (std::find(m_children.cbegin(), m_children.cend(), &node) == m_children.cend()) {
    m_children.emplace_back(&node);
    node.m_isRoot = false;
  }

  // Stop the recursive unpacking if no more nodes are to be added as children
  if constexpr (sizeof...(otherNodes) > 0)
    addChildren(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename T>
template <typename NodeT, typename... OtherNodesTs>
void GraphNode<T>::addParents(NodeT&& node, OtherNodesTs&&... otherNodes) {
  static_assert(std::is_same_v<T, std::decay_t<NodeT>>, "Error: The given objects' type must be the same as the node's one.");
  static_assert(std::is_base_of_v<GraphNode, std::decay_t<NodeT>>,
                "Error: A graph node can only have other objects of types derived from GraphNode as parents.");

  assert("Error: A GraphNode cannot have itself as a parent." && &node != this);

  if (std::find(node.m_children.cbegin(), node.m_children.cend(), this) == node.m_children.cend()) {
    node.m_children.emplace_back(static_cast<T*>(this));
    m_isRoot = false;
  }

  // Stop the recursive unpacking if no more nodes are to be added as parents
  if constexpr (sizeof...(otherNodes) > 0)
    addParents(std::forward<OtherNodesTs>(otherNodes)...);
}

template <typename NodeT>
template <typename... Args>
NodeT& Graph<NodeT>::addNode(Args&& ... args) {
  m_nodes.emplace_back(std::make_unique<NodeT>(std::forward<Args>(args)...));
  return *m_nodes.back();
}

} // namespace Raz
