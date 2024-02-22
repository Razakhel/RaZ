#include "Catch.hpp"

#include "RaZ/Data/Graph.hpp"

namespace {

class TestNode final : public Raz::GraphNode<TestNode> {};

} // namespace

TEST_CASE("Graph linking test", "[data]") {
  Raz::Graph<TestNode> graph(5);

  TestNode& root   = graph.addNode();
  TestNode& node1  = graph.addNode();
  TestNode& node11 = graph.addNode();
  TestNode& node12 = graph.addNode();
  TestNode& node2  = graph.addNode();

  CHECK(graph.getNodeCount() == 5);

  // root -> node1
  root.addChildren(node1);

  CHECK(root.getParentCount() == 0);
  CHECK(root.getChildCount() == 1);
  CHECK(&root.getChild(0) == &node1);

  CHECK(node1.getParentCount() == 1);
  CHECK(node1.getChildCount() == 0);
  CHECK(&node1.getParent(0) == &root);

  // Unchanged, node1 is already a child of root
  node1.addParents(root);

  CHECK(root.getParentCount() == 0);
  CHECK(root.getChildCount() == 1);
  CHECK(&root.getChild(0) == &node1);

  CHECK(node1.getParentCount() == 1);
  CHECK(&node1.getParent(0) == &root);
  CHECK(node1.getChildCount() == 0);

  //              / node11
  // root -> node1
  //              \ node12
  node1.addChildren(node11, node12);

  CHECK(root.getParentCount() == 0);
  CHECK(root.getChildCount() == 1);
  CHECK(&root.getChild(0) == &node1);

  CHECK(node1.getParentCount() == 1);
  CHECK(&node1.getParent(0) == &root);
  CHECK(node1.getChildCount() == 2);
  CHECK(&node1.getChild(0) == &node11);
  CHECK(&node1.getChild(1) == &node12);

  CHECK(node11.getParentCount() == 1);
  CHECK(&node11.getParent(0) == &node1);
  CHECK(node11.getChildCount() == 0);

  CHECK(node12.getParentCount() == 1);
  CHECK(&node12.getParent(0) == &node1);
  CHECK(node12.getChildCount() == 0);

  //            / node11
  //     / node1
  // root       \ node12
  //     \ node2
  node2.addParents(root);

  CHECK(root.getParentCount() == 0);
  CHECK(root.getChildCount() == 2);
  CHECK(&root.getChild(0) == &node1);
  CHECK(&root.getChild(1) == &node2);

  CHECK(node1.getParentCount() == 1);
  CHECK(&node1.getParent(0) == &root);
  CHECK(node1.getChildCount() == 2);
  CHECK(&node1.getChild(0) == &node11);
  CHECK(&node1.getChild(1) == &node12);

  CHECK(node11.getParentCount() == 1);
  CHECK(&node11.getParent(0) == &node1);
  CHECK(node11.getChildCount() == 0);

  CHECK(node12.getParentCount() == 1);
  CHECK(&node12.getParent(0) == &node1);
  CHECK(node12.getChildCount() == 0);

  CHECK(node2.getParentCount() == 1);
  CHECK(&node2.getParent(0) == &root);
  CHECK(node2.getChildCount() == 0);

  //     / node1 -> node11
  // root
  //     \ node2
  //
  // node12
  node12.removeParents(node1);

  CHECK(root.getParentCount() == 0);
  CHECK(root.getChildCount() == 2);
  CHECK(&root.getChild(0) == &node1);
  CHECK(&root.getChild(1) == &node2);

  CHECK(node1.getParentCount() == 1);
  CHECK(&node1.getParent(0) == &root);
  CHECK(node1.getChildCount() == 1);
  CHECK(&node1.getChild(0) == &node11);

  CHECK(node11.getParentCount() == 1);
  CHECK(&node11.getParent(0) == &node1);
  CHECK(node11.getChildCount() == 0);

  CHECK(node12.isIsolated());

  CHECK(node2.getParentCount() == 1);
  CHECK(&node2.getParent(0) == &root);
  CHECK(node2.getChildCount() == 0);

  // root -> node2
  //
  // node11
  graph.removeNode(node1);

  CHECK(root.getParentCount() == 0);
  CHECK(root.getChildCount() == 1);
  CHECK(&root.getChild(0) == &node2);

  CHECK(node11.isIsolated());

  CHECK(node2.getParentCount() == 1);
  CHECK(&node2.getParent(0) == &root);
  CHECK(node2.getChildCount() == 0);

  // Trying to add or remove the current node as child or parent throws
  CHECK_THROWS(node2.addChildren(node11, node2, node12));
  CHECK(&node2.getChild(0) == &node11); // The arguments given before the current node have been set, but not those after
  CHECK_THROWS(node2.removeChildren(node11, node2));
  CHECK_THROWS(node11.addParents(node2, node11, node12));
  CHECK(&node11.getParent(0) == &node2);
  CHECK_THROWS(node11.removeParents(node2, node11));

  // Trying to remove a node which is not in the graph throws
  TestNode testNode;
  CHECK_THROWS(graph.removeNode(testNode));
}

TEST_CASE("Graph extremities test", "[data]") {
  Raz::Graph<TestNode> graph(5);

  TestNode& root  = graph.addNode();
  TestNode& node1 = graph.addNode();
  TestNode& node2 = graph.addNode();
  TestNode& node3 = graph.addNode();
  TestNode& leaf  = graph.addNode();

  // All nodes are isolated, since no parenting has been set up yet
  CHECK(root.isIsolated());
  CHECK(node1.isIsolated());
  CHECK(node2.isIsolated());
  CHECK(node3.isIsolated());
  CHECK(leaf.isIsolated());

  // node1
  //       \
  // node2 -> leaf
  //       /
  // node3
  leaf.addParents(node1, node2, node3);

  CHECK(root.isIsolated());

  CHECK(node1.isRoot());
  CHECK(node2.isRoot());
  CHECK(node3.isRoot());
  CHECK_FALSE(leaf.isRoot());

  CHECK_FALSE(node1.isLeaf());
  CHECK_FALSE(node2.isLeaf());
  CHECK_FALSE(node3.isLeaf());
  CHECK(leaf.isLeaf());

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

  CHECK_FALSE(root.isLeaf());
  CHECK_FALSE(node1.isLeaf());
  CHECK_FALSE(node2.isLeaf());
  CHECK_FALSE(node3.isLeaf());
  CHECK(leaf.isLeaf());

  // node1
  //       \
  // node2 -> leaf
  //       /
  // node3
  graph.removeNode(root); // Removing a node automatically unlinks it from both its parents & children

  CHECK(node1.isRoot());
  CHECK(node2.isRoot());
  CHECK(node3.isRoot());
  CHECK_FALSE(leaf.isRoot());

  CHECK_FALSE(node1.isLeaf());
  CHECK_FALSE(node2.isLeaf());
  CHECK_FALSE(node3.isLeaf());
  CHECK(leaf.isLeaf());

  // node1
  //
  // node2
  //
  // node3
  graph.removeNode(leaf);

  CHECK(node1.isIsolated());
  CHECK(node2.isIsolated());
  CHECK(node3.isIsolated());
}
