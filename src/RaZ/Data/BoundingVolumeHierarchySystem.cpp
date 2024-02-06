#include "RaZ/Data/BoundingVolumeHierarchySystem.hpp"
#include "RaZ/Data/Mesh.hpp"

namespace Raz {

BoundingVolumeHierarchySystem::BoundingVolumeHierarchySystem() {
  registerComponents<Mesh>();
}

void BoundingVolumeHierarchySystem::linkEntity(const EntityPtr& entity) {
  System::linkEntity(entity);
  m_bvh.build(m_entities); // TODO: if N entities are linked one after the other, the BVH will be rebuilt as many times
}

void BoundingVolumeHierarchySystem::unlinkEntity(const EntityPtr& entity) {
  System::unlinkEntity(entity);
  m_bvh.build(m_entities); // TODO: if N entities are unlinked one after the other, the BVH will be rebuilt as many times
}

} // namespace Raz
