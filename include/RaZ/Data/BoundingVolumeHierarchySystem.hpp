#pragma once

#ifndef RAZ_BOUNDINGVOLUMEHIERARCHYSYSTEM_HPP
#define RAZ_BOUNDINGVOLUMEHIERARCHYSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/Data/BoundingVolumeHierarchy.hpp"

namespace Raz {

/// System dedicated to managing a [Bounding Volume Hierarchy](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy) (BVH) of the scene,
///  automatically updating it from linked and unlinked entities.
/// \see BoundingVolumeHierarchy
class BoundingVolumeHierarchySystem final : public System {
public:
  /// Default constructor.
  BoundingVolumeHierarchySystem();

  const BoundingVolumeHierarchy& getBvh() const noexcept { return m_bvh; }

private:
  /// Links the entity to the system and rebuilds the BVH.
  /// \param entity Entity to be linked.
  void linkEntity(const EntityPtr& entity) override;
  /// Uninks the entity to the system and rebuilds the BVH.
  /// \param entity Entity to be unlinked.
  void unlinkEntity(const EntityPtr& entity) override;

  BoundingVolumeHierarchy m_bvh {};
};

} // namespace Raz

#endif // RAZ_BOUNDINGVOLUMEHIERARCHYSYSTEM_HPP
