#pragma once

#ifndef RAZ_WORLD_HPP
#define RAZ_WORLD_HPP

#include "RaZ/Entity.hpp"
#include "RaZ/System.hpp"

namespace Raz {

class World {
public:
  explicit World(std::size_t entityCount) { m_entities.reserve(entityCount); }

  const std::vector<SystemPtr>& getSystems() const { return m_systems; }
  const std::vector<EntityPtr>& getEntities() const { return m_entities; }

  template <typename Sys> bool hasSystem() const;
  template <typename Sys> Sys& getSystem();
  template <typename Sys, typename... Args> Sys& addSystem(Args&&... args);
  template <typename Sys> std::tuple<Sys&> addSystems();
  template <typename Sys1, typename Sys2, typename... S> std::tuple<Sys1&, Sys2&, S...> addSystems();
  template <typename Sys> void removeSystem();
  Entity& addEntity(bool enabled = true);
  template <typename Comp, typename... Args> Entity& addEntityWithComponent(bool enabled = true, Args&&... args);
  template <typename... C> Entity& addEntityWithComponents(bool enabled = true);
  void update(float deltaTime);
  void refresh();
  void destroy();

private:
  std::vector<SystemPtr> m_systems {};
  std::vector<EntityPtr> m_entities {};
  std::size_t m_enabledEntityCount = 0;
  std::size_t m_maxEntityIndex = 0;
};

} // namespace Raz

#include "RaZ/World.inl"

#endif // RAZ_WORLD_HPP
