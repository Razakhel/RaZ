#pragma once

#ifndef RAZ_BVHSYSTEM_HPP
#define RAZ_BVHSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <memory>
#include <vector>

namespace Raz {

/// BVH node, holding the necessary information to perform queries on the BVH.
class BvhNode {
  friend class BvhSystem;

  struct TriangleInfo {
    Triangle triangle = Triangle(Vec3f(0.f), Vec3f(0.f), Vec3f(0.f)); ///< Triangle contained by the node. Only valid if the node is a leaf.
    Entity* entity {}; ///< Entity containing the triangle. Only valid if the node is a leaf.
  };

public:
  BvhNode() = default;
  BvhNode(const BvhNode&) = delete;
  BvhNode(BvhNode&&) noexcept = default;

  const AABB& getBoundingBox() const noexcept { return m_boundingBox; }
  bool hasLeftChild() const noexcept { return (m_leftChild != nullptr); }
  const BvhNode& getLeftChild() const noexcept { assert(hasLeftChild()); return *m_leftChild; }
  bool hasRightChild() const noexcept { return (m_rightChild != nullptr); }
  const BvhNode& getRightChild() const noexcept { assert(hasRightChild()); return *m_rightChild; }
  const Triangle& getTriangle() const noexcept { return m_triangleInfo.triangle; }
  /// Checks if the current node is a leaf, that is, a node without any child.
  /// \note This is a requirement for the triangle information to be valid.
  /// \return True if it is a leaf node, false otherwise.
  bool isLeaf() const noexcept { return m_leftChild == nullptr && m_rightChild == nullptr; }

  /// Queries the BVH node to find the closest entity intersected by the given ray.
  /// \param ray Ray to query the BVH node with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return Closest entity intersected.
  Entity* query(const Ray& ray, RayHit* hit = nullptr) const;

  BvhNode& operator=(const BvhNode&) = delete;
  BvhNode& operator=(BvhNode&&) noexcept = default;

private:
  /// Builds the node and its children from a list of triangles.
  /// \param nodes Nodes to be linked with each other.
  /// \param nodeIndex Current index of the node in the list.
  /// \param trianglesInfo List of triangles information to build the node from.
  /// \param beginIndex First index in the triangles' list.
  /// \param endIndex Past-the-end index in the triangles' list.
  void build(std::vector<BvhNode>& nodes, std::size_t nodeIndex, std::vector<TriangleInfo>& trianglesInfo, std::size_t beginIndex, std::size_t endIndex);

  AABB m_boundingBox = AABB(Vec3f(0.f), Vec3f(0.f));
  BvhNode* m_leftChild {};
  BvhNode* m_rightChild {};
  TriangleInfo m_triangleInfo; ///< Triangle/entity pair. Only valid if the node is a leaf.
};

/// [Bounding Volume Hierarchy](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy) data structure, organized as a binary tree.
/// This can be used to perform efficient queries from a ray in the scene.
class BvhSystem final : public System {
public:
  /// Default constructor.
  BvhSystem();

  const BvhNode& getRootNode() const noexcept { return m_rootNode; }

  /// Builds the BVH.
  void build();
  /// Queries the BVH to find the closest entity intersected by the given ray.
  /// \param ray Ray to query the BVH with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return Closest entity intersected.
  Entity* query(const Ray& ray, RayHit* hit = nullptr) const { return m_rootNode.query(ray, hit); }

private:
  /// Links the entity to the system and rebuilds the BVH.
  /// \param entity Entity to be linked.
  void linkEntity(const EntityPtr& entity) override;
  /// Uninks the entity to the system and rebuilds the BVH.
  /// \param entity Entity to be unlinked.
  void unlinkEntity(const EntityPtr& entity) override;

  BvhNode m_rootNode {};
  std::vector<BvhNode> m_nodes {};
};

} // namespace Raz

#endif // RAZ_BVHSYSTEM_HPP
