#pragma once

#ifndef RAZ_MESHDISTANCEFIELD_HPP
#define RAZ_MESHDISTANCEFIELD_HPP

#include "RaZ/Utils/Shape.hpp"

#include <vector>

namespace Raz {

class BoundingVolumeHierarchy;
class Image;

/// 3-dimensional structure of signed distances to the closest mesh geometry in a specific area. Distances inside a mesh will be negative.
class MeshDistanceField {
public:
  /// Creates a mesh distance field.
  /// \param area Area inside which the distances will be computed.
  /// \param width Number of divisions along the width; must be equal to or greater than 2.
  /// \param height Number of divisions along the height; must be equal to or greater than 2.
  /// \param depth Number of divisions along the depth; must be equal to or greater than 2.
  MeshDistanceField(const AABB& area, unsigned int width, unsigned int height, unsigned int depth);

  float getDistance(std::size_t widthIndex, std::size_t heightIndex, std::size_t depthIndex) const;

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
  constexpr std::size_t computeIndex(std::size_t widthIndex, std::size_t heightIndex, std::size_t depthIndex) const noexcept {
    assert("Error: The given width index is invalid." && widthIndex < m_width);
    assert("Error: The given height index is invalid." && heightIndex < m_height);
    assert("Error: The given channel depth is invalid." && depthIndex < m_depth);
    return depthIndex * m_height * m_width + heightIndex * m_width + widthIndex;
  }

  AABB m_area = AABB(Vec3f(0.f), Vec3f(0.f));
  unsigned int m_width {};
  unsigned int m_height {};
  unsigned int m_depth {};
  std::vector<float> m_distanceField {};
  const BoundingVolumeHierarchy* m_bvh = nullptr;
};

} // namespace Raz

#endif // RAZ_MESHDISTANCEFIELD_HPP
