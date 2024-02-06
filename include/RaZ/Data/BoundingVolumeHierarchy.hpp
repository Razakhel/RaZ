#pragma once

#ifndef RAZ_BOUNDINGVOLUMEHIERARCHY_HPP
#define RAZ_BOUNDINGVOLUMEHIERARCHY_HPP

#include "RaZ/Utils/Shape.hpp"

#include <memory>
#include <vector>

namespace Raz {

class Entity;

/// [Bounding Volume Hierarchy](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy) (BVH) node, holding the necessary information
///  to perform queries on the BVH.
/// \see BoundingVolumeHierarchy
class BoundingVolumeHierarchyNode {
  friend class BoundingVolumeHierarchy;

public:
  BoundingVolumeHierarchyNode() = default;
  BoundingVolumeHierarchyNode(const BoundingVolumeHierarchyNode&) = delete;
  BoundingVolumeHierarchyNode(BoundingVolumeHierarchyNode&&) noexcept = default;

  const AABB& getBoundingBox() const noexcept { return m_boundingBox; }
  bool hasLeftChild() const noexcept { return (m_leftChild != nullptr); }
  const BoundingVolumeHierarchyNode& getLeftChild() const noexcept { assert(hasLeftChild()); return *m_leftChild; }
  bool hasRightChild() const noexcept { return (m_rightChild != nullptr); }
  const BoundingVolumeHierarchyNode& getRightChild() const noexcept { assert(hasRightChild()); return *m_rightChild; }
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

  BoundingVolumeHierarchyNode& operator=(const BoundingVolumeHierarchyNode&) = delete;
  BoundingVolumeHierarchyNode& operator=(BoundingVolumeHierarchyNode&&) noexcept = default;

private:
  struct TriangleInfo {
    Triangle triangle = Triangle(Vec3f(0.f), Vec3f(0.f), Vec3f(0.f)); ///< Triangle contained by the node. Only valid if the node is a leaf.
    Entity* entity {}; ///< Entity containing the triangle. Only valid if the node is a leaf.
  };

  /// Builds the node and its children from a list of triangles.
  /// \param trianglesInfo List of triangles information to build the node from.
  /// \param beginIndex First index in the triangles' list.
  /// \param endIndex Past-the-end index in the triangles' list.
  void build(std::vector<TriangleInfo>& trianglesInfo, std::size_t beginIndex, std::size_t endIndex);

  AABB m_boundingBox = AABB(Vec3f(0.f), Vec3f(0.f));
  std::unique_ptr<BoundingVolumeHierarchyNode> m_leftChild {};
  std::unique_ptr<BoundingVolumeHierarchyNode> m_rightChild {};
  TriangleInfo m_triangleInfo; ///< Triangle/entity pair. Only valid if the node is a leaf.
};

/// [Bounding Volume Hierarchy](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy) (BVH) data structure, organized as a binary tree.
/// This can be used to perform efficient queries from a ray in the scene.
class BoundingVolumeHierarchy {
public:
  BoundingVolumeHierarchy() = default;
  BoundingVolumeHierarchy(const BoundingVolumeHierarchy&) = delete;
  BoundingVolumeHierarchy(BoundingVolumeHierarchy&&) noexcept = default;

  const BoundingVolumeHierarchyNode& getRootNode() const noexcept { return m_rootNode; }

  /// Builds the BVH from the given entities.
  /// \param entities Entities with which to build the BVH from. They must have a Mesh component in order to be used for the build.
  void build(const std::vector<Entity*>& entities);
  /// Queries the BVH to find the closest entity intersected by the given ray.
  /// \param ray Ray to query the BVH with.
  /// \param hit Optional ray intersection's information to recover (nullptr if unneeded).
  /// \return Closest entity intersected.
  Entity* query(const Ray& ray, RayHit* hit = nullptr) const { return m_rootNode.query(ray, hit); }

  BoundingVolumeHierarchy& operator=(const BoundingVolumeHierarchy&) = delete;
  BoundingVolumeHierarchy& operator=(BoundingVolumeHierarchy&&) noexcept = default;

private:
  BoundingVolumeHierarchyNode m_rootNode {};
};

} // namespace Raz

#endif // RAZ_BOUNDINGVOLUMEHIERARCHY_HPP
