#include "RaZ/Entity.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Utils/TriggerSystem.hpp"
#include "RaZ/Utils/TriggerVolume.hpp"

namespace Raz {

TriggerSystem::TriggerSystem() {
  registerComponents<Triggerer, TriggerVolume>();
}

bool TriggerSystem::update(const FrameTimeInfo&) {
  for (const Entity* triggererEntity : m_entities) {
    if (!triggererEntity->hasComponent<Triggerer>() || !triggererEntity->hasComponent<Transform>())
      continue;

    const auto& triggererTransform = triggererEntity->getComponent<Transform>();

    for (Entity* triggerVolumeEntity : m_entities) {
      if (!triggerVolumeEntity->hasComponent<TriggerVolume>())
        continue;

      auto& triggerVolume = triggerVolumeEntity->getComponent<TriggerVolume>();

      if (!triggerVolume.m_enabled)
        continue;

      processTrigger(triggerVolume, triggererTransform);
    }
  }

  return true;
}

void TriggerSystem::processTrigger(TriggerVolume& triggerVolume, const Transform& triggererTransform) {
  const bool wasBeingTriggered = triggerVolume.m_isCurrentlyTriggered;

  triggerVolume.m_isCurrentlyTriggered = std::visit([&triggererTransform] (const auto& volume) {
    // TODO: handle all transform info for both the triggerer & the volume
    return volume.contains(triggererTransform.getPosition());
  }, triggerVolume.m_volume);

  if (!wasBeingTriggered && !triggerVolume.m_isCurrentlyTriggered)
    return;

  const std::function<void()>& action = (!wasBeingTriggered && triggerVolume.m_isCurrentlyTriggered ? triggerVolume.m_enterAction
                                      : (wasBeingTriggered && triggerVolume.m_isCurrentlyTriggered  ? triggerVolume.m_stayAction
                                                                                                    : triggerVolume.m_leaveAction));

  if (action)
    action();
}

} // namespace Raz
