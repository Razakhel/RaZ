#include "RaZ/Animation/Animation.hpp"
#include "RaZ/Animation/AnimationSystem.hpp"

namespace Raz {

AnimationSystem::AnimationSystem() {
  m_acceptedComponents.setBit(Component::getId<Animation>());
}

bool AnimationSystem::update(float deltaTime) {
  for (Entity* entity : m_entities) {
    if (!entity->isEnabled())
      continue;

    if (entity->hasComponent<Animation>()) {
      auto& animation = entity->getComponent<Animation>();

      if (animation.isPlaying()) {
        float& currentTime = m_timers[entity];
        currentTime += animation.getFrameTime() * deltaTime;
        // TODO: roll properly around the animation
        //currentTime = std::fmod(currentTime, animation.getFrameTime() * static_cast<float>(animation.getKeyframeCount()));

        // TODO: process animation
      }
    }
  }

  return true;
}

void AnimationSystem::linkEntity(const EntityPtr& entity) {
  m_timers.emplace(entity.get(), 0.f);

  System::linkEntity(entity);
}

void AnimationSystem::unlinkEntity(const EntityPtr& entity) {
  m_timers.erase(entity.get());

  System::unlinkEntity(entity);
}

} // namespace Raz
