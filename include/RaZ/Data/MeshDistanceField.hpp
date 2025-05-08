#pragma once

#ifndef RAZ_MESHDISTANCEFIELD_HPP
#define RAZ_MESHDISTANCEFIELD_HPP

#include "RaZ/Data/Grid3.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace Raz {

class BoundingVolumeHierarchy;
class Image;

/// 3-dimensional structure of signed distances to the closest mesh geometry in a specific area. Distances inside a mesh will be negative.
class MeshDistanceField : public Grid3f {
public:
  /// Creates a mesh distance field.
  /// \param area Area inside which the distances will be computed.
  /// \param width Number of divisions along the width; must be equal to or greater than 2.
  /// \param height Number of divisions along the height; must be equal to or greater than 2.
  /// \param depth Number of divisions along the depth; must be equal to or greater than 2.
  MeshDistanceField(const AABB& area, std::size_t width, std::size_t height, std::size_t depth);

  void setBvh(const BoundingVolumeHierarchy& bvh) { m_bvh = &bvh; }

  /// Computes the distance field's values for each point within the grid.
  /// \param sampleCount Number of directions to sample around each point; a higher count will result in a better definition.
  /// \note This requires a BVH to have been set.
  /// \see setBvh()
  void compute(std::size_t sampleCount);
  /// Recovers the distance field's values in a list of 2D floating-point images.
  /// \return Images of each slice of the field along the depth.
  std::vector<Image> recoverSlices() const;

private:
  AABB m_area = AABB(Vec3f(0.f), Vec3f(0.f));
  const BoundingVolumeHierarchy* m_bvh = nullptr;
};

} // namespace Raz

#endif // RAZ_MESHDISTANCEFIELD_HPP
