#pragma once

#ifndef RAZ_TRIGGERVOLUME_HPP
#define RAZ_TRIGGERVOLUME_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <functional>
#include <variant>

namespace Raz {

/// Triggerer component, representing an entity that can interact with triggerable entities.
/// \see TriggerVolume
class Triggerer final : public Component {};

/// TriggerVolume component, holding a volume that can be triggered and actions that can be executed accordingly.
/// \see Triggerer, TriggerSystem
class TriggerVolume final : public Component {
  friend class TriggerSystem;

public:
  explicit TriggerVolume(const AABB& aabb) : m_volume{ aabb } {
    if (aabb.getMinPosition().x() >= aabb.getMaxPosition().x()
     || aabb.getMinPosition().y() >= aabb.getMaxPosition().y()
     || aabb.getMinPosition().z() >= aabb.getMaxPosition().z()) {
      throw std::invalid_argument("[TriggerVolume] The AABB's max position must be greater than the min on all axes");
    }
  }
  explicit TriggerVolume(const OBB& obb) : m_volume{ obb } {
    if (obb.getMinPosition().x() >= obb.getMaxPosition().x()
     || obb.getMinPosition().y() >= obb.getMaxPosition().y()
     || obb.getMinPosition().z() >= obb.getMaxPosition().z()) {
      throw std::invalid_argument("[TriggerVolume] The OBB's max position must be greater than the min on all axes");
    }
  }
  explicit TriggerVolume(const Sphere& sphere) : m_volume{ sphere } {
    if (sphere.getRadius() <= 0.f)
      throw std::invalid_argument("[TriggerVolume] The sphere's radius must be greater than 0");
  }

  void setEnterAction(std::function<void()> enterAction) { m_enterAction = std::move(enterAction); }
  void setStayAction(std::function<void()> stayAction) { m_stayAction = std::move(stayAction); }
  void setLeaveAction(std::function<void()> leaveAction) { m_leaveAction = std::move(leaveAction); }

  /// Changes the trigger volume's state.
  /// \param enabled True if the trigger volume should be enabled (triggerable), false otherwise.
  void enable(bool enabled = true) noexcept { m_enabled = enabled; }
  /// Disables the trigger volume, making it non-triggerable.
  void disable() noexcept { enable(false); }
  void resetEnterAction() { setEnterAction(nullptr); }
  void resetStayAction() { setStayAction(nullptr); }
  void resetLeaveAction() { setLeaveAction(nullptr); }

private:
  bool m_enabled = true;

  std::variant<AABB, OBB, Sphere> m_volume;
  std::function<void()> m_enterAction;
  std::function<void()> m_stayAction;
  std::function<void()> m_leaveAction;

  bool m_isCurrentlyTriggered = false;
};

} // namespace Raz

#endif // RAZ_TRIGGERVOLUME_HPP
