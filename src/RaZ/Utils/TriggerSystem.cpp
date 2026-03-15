#include "RaZ/Entity.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Utils/TriggerSystem.hpp"
#include "RaZ/Utils/TriggerVolume.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

TriggerSystem::TriggerSystem() {
  registerComponents<Triggerer, TriggerVolume>();
}

bool TriggerSystem::update(const FrameTimeInfo&) {
  ZoneScopedN("TriggerSystem::update");

  for (Entity* triggererEntity : m_entities) {
    if (!triggererEntity->hasComponent<Triggerer>() || !triggererEntity->hasComponent<Transform>())
      continue;

    const auto& triggerer          = triggererEntity->getComponent<Triggerer>();
    const auto& triggererTransform = triggererEntity->getComponent<Transform>();
    const Vec3f triggererPos       = triggererTransform.getRotation() * triggererTransform.getPosition();

    for (Entity* triggerVolumeEntity : m_entities) {
      if (!triggerVolumeEntity->hasComponent<TriggerVolume>() || !triggerVolumeEntity->hasComponent<Transform>())
        continue;

      if ((triggerer.getTriggerableComponents() & triggerVolumeEntity->getEnabledComponents()).isEmpty())
        continue;

      auto& triggerVolume = triggerVolumeEntity->getComponent<TriggerVolume>();

      if (!triggerVolume.m_enabled)
        continue;

      processTrigger(triggerVolume, *triggererEntity, triggererPos, triggerVolumeEntity->getComponent<Transform>());
    }
  }

  return true;
}

void TriggerSystem::processTrigger(TriggerVolume& triggerVolume,
                                   Entity& triggererEntity,
                                   const Vec3f& triggererPos,
                                   const Transform& triggerVolumeTransform) {
  ZoneScopedN("TriggerSystem::processTrigger");

  const bool wasBeingTriggered = triggerVolume.m_triggeringEntities.contains(&triggererEntity);

  const Vec3f triggerVolumeAbsPos = triggerVolumeTransform.getRotation() * triggerVolumeTransform.getPosition();
  const Vec3f triggererRelPos     = triggererPos - triggerVolumeAbsPos; // Triggerer position in the volume's space
  const bool isCurrentlyTriggered = std::visit([&triggererRelPos] (const auto& volume) {
    return volume.contains(triggererRelPos);
  }, triggerVolume.m_volume);

  if (isCurrentlyTriggered)
    triggerVolume.m_triggeringEntities.emplace(&triggererEntity);
  else
    triggerVolume.m_triggeringEntities.erase(&triggererEntity);

  if (!wasBeingTriggered && !isCurrentlyTriggered)
    return;

  const std::function<void(Entity&)>& action = (!wasBeingTriggered   ? triggerVolume.m_enterAction
                                             : (isCurrentlyTriggered ? triggerVolume.m_stayAction
                                                                     : triggerVolume.m_leaveAction));

  if (action)
    action(triggererEntity);
}

} // namespace Raz
