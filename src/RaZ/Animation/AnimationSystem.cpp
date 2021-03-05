#include "RaZ/Animation/Animation.hpp"
#include "RaZ/Animation/AnimationSystem.hpp"

namespace Raz {

AnimationSystem::AnimationSystem() {
  m_acceptedComponents.setBit(Component::getId<Animation>());
}

bool AnimationSystem::update(float) {
  return true;
}

} // namespace Raz
