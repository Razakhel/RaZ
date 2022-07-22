#include "RaZ/Data/BvhSystem.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"

namespace Raz {

namespace {

enum CutAxis {
  AXIS_X = 0,
  AXIS_Y = 1,
  AXIS_Z = 2
};

} // namespace

Entity* BvhNode::query(const Ray& ray, RayHit* hit) const {
  if (!ray.intersects(m_boundingBox, hit))
    return nullptr;

  if (isLeaf() && ray.intersects(m_triangleInfo.triangle, hit))
    return m_triangleInfo.entity;

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

void BvhNode::build(std::vector<TriangleInfo>& trianglesInfo, std::size_t beginIndex, std::size_t endIndex) {
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
  const float halfCutPos  = m_boundingBox.getMinPosition()[cutAxis] + (maxLength / 2.f);
  const auto midShapeIter = std::partition(trianglesInfo.begin() + beginIndex, trianglesInfo.begin() + endIndex, [cutAxis, halfCutPos] (const TriangleInfo& triangleInfo) {
    return triangleInfo.triangle.computeCentroid()[cutAxis] < halfCutPos;
  });

  auto midIndex = static_cast<std::size_t>(std::distance(trianglesInfo.begin(), midShapeIter));
  if (midIndex == beginIndex || midIndex == endIndex)
    midIndex = (beginIndex + endIndex) / 2;

  m_leftChild = std::make_unique<BvhNode>();
  m_leftChild->build(trianglesInfo, beginIndex, midIndex);

  m_rightChild = std::make_unique<BvhNode>();
  m_rightChild->build(trianglesInfo, midIndex, endIndex);
}

BvhSystem::BvhSystem() {
  registerComponents<Mesh>();
}

void BvhSystem::build() {
  m_rootNode = BvhNode();

  // Storing all triangles in a list to build the BVH from

  std::size_t totalTriangleCount = 0;

  for (const Entity* entity : m_entities) {
    if (!entity->isEnabled())
      continue;

    totalTriangleCount += entity->getComponent<Mesh>().recoverTriangleCount();
  }

  if (totalTriangleCount == 0)
    return; // No triangle to build the BVH from

  std::vector<BvhNode::TriangleInfo> triangles;
  triangles.reserve(totalTriangleCount);

  for (Entity* entity : m_entities) {
    if (!entity->isEnabled())
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

        triangles.emplace_back(BvhNode::TriangleInfo{ triangle, entity });
      }
    }
  }

  m_rootNode.build(triangles, 0, totalTriangleCount);
}

void BvhSystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);
  build(); // TODO: if N entities are linked one after the other, the BVH will be rebuilt as many times
}

void BvhSystem::unlinkEntity(const EntityPtr& entity) {
  System::unlinkEntity(entity);
  build(); // TODO: if N entities are unlinked one after the other, the BVH will be rebuilt as many times
}

} // namespace Raz
