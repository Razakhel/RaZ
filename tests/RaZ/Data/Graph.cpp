#include "Catch.hpp"

#include "RaZ/Data/Graph.hpp"

namespace {

class TestNode final : public Raz::GraphNode<TestNode> {};

} // namespace

TEST_CASE("Graph linking test") {
  Raz::Graph<TestNode> graph(5);

  TestNode& root   = graph.addNode();
  TestNode& node1  = graph.addNode();
  TestNode& node11 = graph.addNode();
  TestNode& node12 = graph.addNode();
  TestNode& node2  = graph.addNode();

  CHECK(graph.getNodeCount() == 5);

  // root -> node1
  root.addChildren(node1);

  CHECK(root.isRoot());
  CHECK_FALSE(node1.isRoot());

  CHECK(root.getChildCount() == 1);
  CHECK(node1.getChildCount() == 0);
  CHECK(&root.getChild(0) == &node1);

  // Unchanged, node1 is already a child of root
  node1.addParents(root);

  CHECK(root.isRoot());
  CHECK_FALSE(node1.isRoot());

  CHECK(root.getChildCount() == 1);
  CHECK(node1.getChildCount() == 0);
  CHECK(&root.getChild(0) == &node1);

  //              / node11
  // root -> node1
  //              \ node12
  node1.addChildren(node11, node12);

  CHECK(root.isRoot());
  CHECK_FALSE(node1.isRoot());
  CHECK_FALSE(node11.isRoot());
  CHECK_FALSE(node12.isRoot());

  CHECK(root.getChildCount() == 1);
  CHECK(node1.getChildCount() == 2);
  CHECK(node11.getChildCount() == 0);
  CHECK(node12.getChildCount() == 0);
  CHECK(&node1.getChild(0) == &node11);
  CHECK(&node1.getChild(1) == &node12);

  //            / node11
  //     / node1
  // root       \ node12
  //     \ node2
  node2.addParents(root);

  CHECK(root.isRoot());
  CHECK_FALSE(node1.isRoot());
  CHECK_FALSE(node11.isRoot());
  CHECK_FALSE(node12.isRoot());
  CHECK_FALSE(node2.isRoot());

  CHECK(root.getChildCount() == 2);
  CHECK(node1.getChildCount() == 2);
  CHECK(node11.getChildCount() == 0);
  CHECK(node12.getChildCount() == 0);
  CHECK(node2.getChildCount() == 0);
  CHECK(&root.getChild(0) == &node1);
  CHECK(&root.getChild(1) == &node2);
}

TEST_CASE("Graph root test") {
  Raz::Graph<TestNode> graph(5);

  TestNode& root  = graph.addNode();
  TestNode& node1 = graph.addNode();
  TestNode& node2 = graph.addNode();
  TestNode& node3 = graph.addNode();
  TestNode& leaf  = graph.addNode();

  // All nodes are roots, since no parenting has been set up yet
  CHECK(root.isRoot());
  CHECK(node1.isRoot());
  CHECK(node2.isRoot());
  CHECK(node3.isRoot());
  CHECK(leaf.isRoot());

  // node1
  //       \
  // node2 -> leaf
  //       /
  // node3
  leaf.addParents(node1, node2, node3);

  CHECK(node1.isRoot());
  CHECK(node2.isRoot());
  CHECK(node3.isRoot());
  CHECK_FALSE(leaf.isRoot());

  //         node1
  //       /       \
  // root -> node2 -> leaf
  //       \       /
  //         node3
  root.addChildren(node1, node2, node3);

  CHECK(root.isRoot());
  CHECK_FALSE(node1.isRoot());
  CHECK_FALSE(node2.isRoot());
  CHECK_FALSE(node3.isRoot());
  CHECK_FALSE(leaf.isRoot());
}
