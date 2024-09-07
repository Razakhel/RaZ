#include "RaZ/Entity.hpp"
#include "RaZ/Data/BoundingVolumeHierarchy.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

namespace {

enum CutAxis {
  AXIS_X = 0,
  AXIS_Y = 1,
  AXIS_Z = 2
};

} // namespace

Entity* BoundingVolumeHierarchyNode::query(const Ray& ray, RayHit* hit) const {
  // The following call can produce way too many zones, *drastically* increasing the profiling time & memory consumption
  //ZoneScopedN("BoundingVolumeHierarchyNode::query");

  if (isLeaf()) {
    if (ray.intersects(m_triangleInfo.triangle, hit))
      return m_triangleInfo.entity;

    return nullptr;
  }

  if (!ray.intersects(m_boundingBox, hit))
    return nullptr;

  RayHit leftHit;
  RayHit rightHit;
  Entity* leftEntity  = (m_leftChild != nullptr ? m_leftChild->query(ray, &leftHit) : nullptr);
  Entity* rightEntity = (m_rightChild != nullptr ? m_rightChild->query(ray, &rightHit) : nullptr);

  const bool isLeftCloser = (leftHit.distance < rightHit.distance);

  if (hit)
    *hit = (isLeftCloser ? leftHit : rightHit);

  // If both entities have been hit, return the closest one
  if (leftEntity && rightEntity)
    return (isLeftCloser ? leftEntity : rightEntity);

  // Otherwise, return whichever has been hit, or none
  return (leftEntity != nullptr ? leftEntity : rightEntity);
}

void BoundingVolumeHierarchyNode::build(std::vector<TriangleInfo>& trianglesInfo, std::size_t beginIndex, std::size_t endIndex) {
  // The following call can produce way too many zones, *drastically* increasing the profiling time & memory consumption
  //ZoneScopedN("BoundingVolumeHierarchyNode::build");

  m_boundingBox = trianglesInfo[beginIndex].triangle.computeBoundingBox();

  if (endIndex - beginIndex <= 1) {
    m_triangleInfo = trianglesInfo[beginIndex];
    return;
  }

  // TODO: wait for a parallel/reduce to be implemented in order to optimize the following loop
  for (std::size_t i = beginIndex + 1; i < endIndex; ++i) {
    const AABB triangleBox = trianglesInfo[i].triangle.computeBoundingBox();

    const float xMin = std::min(triangleBox.getMinPosition().x(), m_boundingBox.getMinPosition().x());
    const float yMin = std::min(triangleBox.getMinPosition().y(), m_boundingBox.getMinPosition().y());
    const float zMin = std::min(triangleBox.getMinPosition().z(), m_boundingBox.getMinPosition().z());

    const float xMax = std::max(triangleBox.getMaxPosition().x(), m_boundingBox.getMaxPosition().x());
    const float yMax = std::max(triangleBox.getMaxPosition().y(), m_boundingBox.getMaxPosition().y());
    const float zMax = std::max(triangleBox.getMaxPosition().z(), m_boundingBox.getMaxPosition().z());

    m_boundingBox = AABB(Vec3f(xMin, yMin, zMin), Vec3f(xMax, yMax, zMax));
  }

  float maxLength = m_boundingBox.getMaxPosition().x() - m_boundingBox.getMinPosition().x();
  CutAxis cutAxis = AXIS_X;

  const float maxYLength = m_boundingBox.getMaxPosition().y() - m_boundingBox.getMinPosition().y();
  if (maxYLength > maxLength) {
    maxLength = maxYLength;
    cutAxis   = AXIS_Y;
  }

  const float maxZLength = m_boundingBox.getMaxPosition().z() - m_boundingBox.getMinPosition().z();
  if (maxZLength > maxLength) {
    maxLength = maxZLength;
    cutAxis   = AXIS_Z;
  }

  // Reorganizing triangles by splitting them over the cut axis, according to their centroid
  const float halfCutPos  = m_boundingBox.getMinPosition()[cutAxis] + (maxLength * 0.5f);
  const auto midShapeIter = std::partition(trianglesInfo.begin() + static_cast<std::ptrdiff_t>(beginIndex),
                                           trianglesInfo.begin() + static_cast<std::ptrdiff_t>(endIndex),
                                           [cutAxis, halfCutPos] (const TriangleInfo& triangleInfo) {
                                             return (triangleInfo.triangle.computeCentroid()[cutAxis] < halfCutPos);
                                           });

  auto midIndex = static_cast<std::size_t>(std::distance(trianglesInfo.begin(), midShapeIter));
  if (midIndex == beginIndex || midIndex == endIndex)
    midIndex = (beginIndex + endIndex) / 2;

  m_leftChild = std::make_unique<BoundingVolumeHierarchyNode>();
  m_leftChild->build(trianglesInfo, beginIndex, midIndex);

  m_rightChild = std::make_unique<BoundingVolumeHierarchyNode>();
  m_rightChild->build(trianglesInfo, midIndex, endIndex);
}

void BoundingVolumeHierarchy::build(const std::vector<Entity*>& entities) {
  ZoneScopedN("BoundingVolumeHierarchy::build");

  m_rootNode = BoundingVolumeHierarchyNode();

  // Storing all triangles in a list to build the BVH from

  std::size_t totalTriangleCount = 0;

  for (const Entity* entity : entities) {
    if (!entity->isEnabled() || !entity->hasComponent<Mesh>())
      continue;

    totalTriangleCount += entity->getComponent<Mesh>().recoverTriangleCount();
  }

  if (totalTriangleCount == 0)
    return; // No triangle to build the BVH from

  std::vector<BoundingVolumeHierarchyNode::TriangleInfo> triangles;
  triangles.reserve(totalTriangleCount);

  for (Entity* entity : entities) {
    if (!entity->isEnabled() || !entity->hasComponent<Mesh>())
      continue;

    const bool hasTransform    = entity->hasComponent<Transform>();
    const Mat4f transformation = (hasTransform ? entity->getComponent<Transform>().computeTransformMatrix() : Mat4f());

    for (const Submesh& submesh : entity->getComponent<Mesh>().getSubmeshes()) {
      for (std::size_t i = 0; i < submesh.getTriangleIndexCount(); i += 3) {
        Triangle triangle(submesh.getVertices()[submesh.getTriangleIndices()[i    ]].position,
                          submesh.getVertices()[submesh.getTriangleIndices()[i + 1]].position,
                          submesh.getVertices()[submesh.getTriangleIndices()[i + 2]].position);

        if (hasTransform) {
          triangle = Triangle(Vec3f(transformation * Vec4f(triangle.getFirstPos(), 1.f)),
                              Vec3f(transformation * Vec4f(triangle.getSecondPos(), 1.f)),
                              Vec3f(transformation * Vec4f(triangle.getThirdPos(), 1.f)));
        }

        triangles.emplace_back(BoundingVolumeHierarchyNode::TriangleInfo{ triangle, entity });
      }
    }
  }

  m_rootNode.build(triangles, 0, totalTriangleCount);
}

} // namespace Raz
