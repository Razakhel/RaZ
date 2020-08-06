#pragma once

#ifndef RAZ_GRAPH_HPP
#define RAZ_GRAPH_HPP

#include <memory>
#include <vector>

namespace Raz {

/// GraphNode class, representing a base node in a Graph. Must be inherited to be used by a Graph.
/// \tparam T Graph node's specific type. Must be the derived class itself.
template <typename T>
class GraphNode {
public:
  GraphNode(const GraphNode&) = delete;
  GraphNode(GraphNode&&) noexcept = default;

  const std::vector<T*>& getChildren() const noexcept { return m_children; }
  bool isRoot() const noexcept { return m_isRoot; }

  /// Links the given nodes as children of the current one.
  /// \tparam NodeT Type of the first node to link.
  /// \tparam OtherNodesTs Types of the other nodes to link.
  /// \param node First node to link.
  /// \param otherNodes Other nodes to link.
  template<typename NodeT, typename... OtherNodesTs>
  void addChildren(NodeT&& node, OtherNodesTs&&... otherNodes);
  /// Links the given nodes as parents of the current one.
  /// \tparam NodeT Type of the first node to link.
  /// \tparam OtherNodesTs Types of the other nodes to link.
  /// \param node First node to link.
  /// \param otherNodes Other nodes to link.
  template<typename NodeT, typename... OtherNodesTs>
  void addParents(NodeT&& node, OtherNodesTs&&... otherNodes);

  GraphNode& operator=(const GraphNode&) = delete;
  GraphNode& operator=(GraphNode&&) noexcept = default;

  virtual ~GraphNode() = default;

protected:
  GraphNode() = default;

  std::vector<T*> m_children {};
  bool m_isRoot = true;
};

/// Graph class, representing a <a href="https://en.wikipedia.org/wiki/Directed_graph">directed graph</a>.
/// This class is implemented as an <a href="https://en.wikipedia.org/wiki/Adjacency_list">adjacency list</a>.
/// \tparam NodeT Type of the graph's nodes. Must be a type derived from GraphNode.
template <typename NodeT>
class Graph {
  static_assert(std::is_base_of_v<GraphNode<NodeT>, NodeT>, "Error: Graph node type must be derived from GraphNode.");

  using NodePtr = std::unique_ptr<NodeT>;

public:
  /// Default constructor.
  Graph() = default;
  /// Creates a Graph while preallocating the given amount of nodes.
  /// \param nodeCount Amount of nodes to reserve.
  explicit Graph(std::size_t nodeCount) { m_nodes.reserve(nodeCount); }
  Graph(const Graph&) = delete;
  Graph(Graph&&) noexcept = default;

  const std::vector<const NodePtr>& getNodes() const noexcept { return m_nodes; }
  const std::vector<NodePtr>& getNodes() noexcept { return m_nodes; }

  /// Adds a node into the graph.
  /// \tparam Args Types of the arguments to be forwarded to the node's constructor.
  /// \param args Arguments to be forwarded to the node's constructor.
  /// \return Reference to the newly added node.
  template <typename... Args>
  NodeT& addNode(Args&&... args);

  Graph& operator=(const Graph&) = delete;
  Graph& operator=(Graph&&) noexcept = default;

protected:
  std::vector<NodePtr> m_nodes {};
};

} // namespace Raz

#include "RaZ/Utils/Graph.inl"

#endif // RAZ_GRAPH_HPP
