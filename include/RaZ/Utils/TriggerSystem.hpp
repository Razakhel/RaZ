#pragma once

#ifndef RAZ_TRIGGERSYSTEM_HPP
#define RAZ_TRIGGERSYSTEM_HPP

#include "RaZ/System.hpp"

namespace Raz {

class Transform;
class TriggerVolume;

class TriggerSystem final : public System {
public:
  TriggerSystem();

  bool update(const FrameTimeInfo& timeInfo) override;

private:
  static void processTrigger(TriggerVolume& triggerVolume, const Transform& triggererTransform);
};

} // namespace Raz

#endif // RAZ_TRIGGERSYSTEM_HPP
