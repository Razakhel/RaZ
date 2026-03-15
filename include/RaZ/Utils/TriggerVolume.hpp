#pragma once

#ifndef RAZ_TRIGGERVOLUME_HPP
#define RAZ_TRIGGERVOLUME_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Data/Bitset.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <functional>
#include <unordered_set>
#include <variant>

namespace Raz {

class Entity;

/// Triggerer component, representing an entity that can interact with triggerable entities.
/// \see TriggerVolume
class Triggerer final : public Component {
public:
  Triggerer() = default;

  const Bitset& getTriggerableComponents() const noexcept { return m_triggerableComponents; }

  /// Registers components that can be triggered by this triggerer.
  /// Only entities containing at least one of these components (along with a TriggerVolume) will be triggerable.
  /// \tparam CompTs Types of the components to register.
  template <typename... CompTs>
  void registerComponents() { (m_triggerableComponents.setBit(Component::getId<CompTs>(), true), ...); }
  /// Unregisters components that can be triggered with this triggerer.
  /// \tparam CompTs Types of the components to unregister.
  template <typename... CompTs>
  void unregisterComponents() { (m_triggerableComponents.setBit(Component::getId<CompTs>(), false), ...); }

private:
  Bitset m_triggerableComponents;
};

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
    if (obb.getOriginalBox().getMinPosition().x() >= obb.getOriginalBox().getMaxPosition().x()
     || obb.getOriginalBox().getMinPosition().y() >= obb.getOriginalBox().getMaxPosition().y()
     || obb.getOriginalBox().getMinPosition().z() >= obb.getOriginalBox().getMaxPosition().z()) {
      throw std::invalid_argument("[TriggerVolume] The OBB's max position must be greater than the min on all axes");
    }
  }
  explicit TriggerVolume(const Sphere& sphere) : m_volume{ sphere } {
    if (sphere.getRadius() <= 0.f)
      throw std::invalid_argument("[TriggerVolume] The sphere's radius must be greater than 0");
  }

  /// Sets the action to be executed when a triggering entity enters the volume.
  /// \param enterAction Action to be executed on entering the volume. Takes the triggering entity as argument.
  void setEnterAction(std::function<void(Entity&)> enterAction) { m_enterAction = std::move(enterAction); }
  /// Sets the action to be executed when a triggering entity stays in the volume.
  /// \param stayAction Action to be executed on staying in the volume. Takes the triggering entity as argument.
  void setStayAction(std::function<void(Entity&)> stayAction) { m_stayAction = std::move(stayAction); }
  /// Sets the action to be executed when a triggering entity leaves the volume.
  /// \param leaveAction Action to be executed on leaving the volume. Takes the triggering entity as argument.
  void setLeaveAction(std::function<void(Entity&)> leaveAction) { m_leaveAction = std::move(leaveAction); }

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
  std::function<void(Entity&)> m_enterAction;
  std::function<void(Entity&)> m_stayAction;
  std::function<void(Entity&)> m_leaveAction;

  std::unordered_set<const Entity*> m_triggeringEntities;
};

} // namespace Raz

#endif // RAZ_TRIGGERVOLUME_HPP
