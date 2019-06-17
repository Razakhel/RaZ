#pragma once

#ifndef RAZ_ENTITY_HPP
#define RAZ_ENTITY_HPP

#include <memory>
#include <type_traits>
#include <vector>

#include "RaZ/Component.hpp"
#include "RaZ/Utils/Bitset.hpp"

namespace Raz {

class Entity;
using EntityPtr = std::unique_ptr<Entity>;

/// Entity class representing an aggregate of Component objects.
class Entity {
public:
  explicit Entity(std::size_t index, bool enabled = true) : m_id{ index }, m_enabled{ enabled } {}

  std::size_t getId() const { return m_id; }
  bool isEnabled() const { return m_enabled; }
  const std::vector<ComponentPtr>& getComponents() const { return m_components; }
  const Bitset& getEnabledComponents() const { return m_enabledComponents; }

  template <typename... Args> static EntityPtr create(Args&&... args) { return std::make_unique<Entity>(std::forward<Args>(args)...); }

  /// Tells if a given component is held by the entity.
  /// \tparam Comp Type of the component to be checked.
  /// \return True if the entity holds the given component, false otherwise.
  template <typename Comp> bool hasComponent() const;
  /// Gets a given component held by the entity.
  /// The entity must have this component. If not, an exception is thrown.
  /// \tparam Comp Type of the component to be fetched.
  /// \return Reference to the found component.
  template <typename Comp> const Comp& getComponent() const;
  /// Gets a given component held by the entity.
  /// The entity must have this component. If not, an exception is thrown.
  /// \tparam Comp Type of the component to be fetched.
  /// \return Reference to the found component.
  template <typename Comp> Comp& getComponent() { return const_cast<Comp&>(static_cast<const Entity*>(this)->getComponent<Comp>()); }
  /// Adds a component to be held by the entity.
  /// \tparam Comp Type of the component to be added.
  /// \tparam Args Types of the arguments to be forwarded to the given component.
  /// \param args Arguments to be forwarded to the given component.
  /// \return Reference to the newly added component.
  template <typename Comp, typename... Args> Comp& addComponent(Args&&... args);
  /// Adds a last component to be held by the entity.
  /// \tparam Comp Type of the last component to be added.
  /// \return A tuple containing a reference to the last newly added component.
  template <typename Comp> std::tuple<Comp&> addComponents();
  /// Adds several components at once to be held by the entity.
  /// \tparam Comp1 Type of the first component to be added.
  /// \tparam Comp2 Type of the second component to be added.
  /// \tparam C Types of the other components to be added.
  /// \return A tuple containing references to all the newly added components.
  template <typename Comp1, typename Comp2, typename... C> std::tuple<Comp1&, Comp2&, C...> addComponents();
  /// Removes the given component from the entity.
  /// \tparam Comp Type of the component to be removed.
  template <typename Comp> void removeComponent();
  /// Changes the entity's enabled state.
  /// Enables or disables the entity according to the given parameter.
  /// \param enabled True if the entity should be enabled, false if it should be disabled.
  void enable(bool enabled = true) { m_enabled = enabled; }
  /// Disables the entity.
  void disable() { enable(false); }

protected:
  Entity() = default;

private:
  std::size_t m_id {};
  bool m_enabled {};
  std::vector<ComponentPtr> m_components {};
  Bitset m_enabledComponents {};
};

} // namespace Raz

#include "RaZ/Entity.inl"

#endif // RAZ_ENTITY_HPP
