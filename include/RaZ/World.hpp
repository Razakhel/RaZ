#pragma once

#ifndef RAZ_WORLD_HPP
#define RAZ_WORLD_HPP

#include "RaZ/Entity.hpp"
#include "RaZ/System.hpp"

namespace Raz {

/// World class handling systems & entities.
class World {
public:
  World() = default;
  explicit World(std::size_t entityCount) { m_entities.reserve(entityCount); }
  World(const World&) = delete;
  World(World&&) noexcept = default;

  const std::vector<SystemPtr>& getSystems() const { return m_systems; }
  const std::vector<EntityPtr>& getEntities() const { return m_entities; }

  /// Tells if a given system exists within the world.
  /// \tparam Sys Type of the system to be checked.
  /// \return True if the given system is present, false otherwise.
  template <typename Sys> bool hasSystem() const;
  /// Gets a given system contained by the world.
  /// This system must be present within the world. If not, an exception is thrown.
  /// \tparam Sys Type of the system to be fetched.
  /// \return Reference to the found system.
  template <typename Sys> Sys& getSystem();
  /// Adds a given system to the world.
  /// \tparam Sys Type of the system to be added.
  /// \tparam Args Types of the arguments to be forwarded to the given system.
  /// \param args Arguments to be forwarded to the given system.
  /// \return Reference to the newly added system.
  template <typename Sys, typename... Args> Sys& addSystem(Args&&... args);
  /// Removes the given system from the world.
  /// \tparam Sys Type of the system to be removed.
  template <typename Sys> void removeSystem();
  /// Adds an entity into the world.
  /// \param enabled True if the entity should be active immediately, false otherwise.
  /// \return Reference to the newly created entity.
  Entity& addEntity(bool enabled = true);
  /// Adds an entity into the world with a given component. This entity will be automatically enabled.
  /// \tparam Comp Type of the component to be added into the entity.
  /// \tparam Args Types of the arguments to be forwarded to the given component.
  /// \param args Arguments to be forwarded to the given component.
  /// \return Reference to the newly added entity.
  template <typename Comp, typename... Args> Entity& addEntityWithComponent(Args&&... args);
  /// Adds an entity into the world with several components at once.
  /// \tparam Comps Types of the components to be added into the entity.
  /// \param enabled True if the entity should be active immediately, false otherwise.
  /// \return Reference to the newly added entity.
  template <typename... Comps> Entity& addEntityWithComponents(bool enabled = true);
  /// Updates the world, updating all the systems it contains.
  /// \param deltaTime Time elapsed since the last update.
  /// \return True if the world still has active systems, false otherwise.
  bool update(float deltaTime);
  /// Refreshes the world, reorganizing its entities to optimize caching by moving the active entities in front.
  void refresh();

  World& operator=(const World&) = delete;
  World& operator=(World&&) noexcept = default;

private:
  std::vector<SystemPtr> m_systems {};
  Bitset m_activeSystems {};

  std::vector<EntityPtr> m_entities {};
  std::size_t m_activeEntityCount = 0;
  std::size_t m_maxEntityIndex = 0;
};

} // namespace Raz

#include "RaZ/World.inl"

#endif // RAZ_WORLD_HPP
