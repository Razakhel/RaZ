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

class Entity {
public:
  explicit Entity(std::size_t index, bool enabled = true) : m_id{ index }, m_enabled{ enabled } {}

  std::size_t getId() const { return m_id; }
  bool isEnabled() const { return m_enabled; }
  const std::vector<ComponentPtr>& getComponents() const { return m_components; }
  const Bitset& getEnabledComponents() const { return m_enabledComponents; }

  template <typename... Args> static EntityPtr create(Args&&... args) { return std::make_unique<Entity>(std::forward<Args>(args)...); }

  template <typename Comp> bool hasComponent() const;
  template <typename Comp> const Comp& getComponent() const;
  template <typename Comp> Comp& getComponent() { return const_cast<Comp&>(static_cast<const Entity*>(this)->getComponent<Comp>()); }
  template <typename Comp, typename... Args> Comp& addComponent(Args&&... args);
  template <typename Comp> std::tuple<Comp&> addComponents();
  template <typename Comp1, typename Comp2, typename... C> std::tuple<Comp1&, Comp2&, C...> addComponents();
  template <typename Comp> void removeComponent();
  void enable(bool enabled = true) { m_enabled = enabled; }
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
