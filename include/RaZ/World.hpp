#pragma once

#ifndef RAZ_WORLD_HPP
#define RAZ_WORLD_HPP

#include "RaZ/Entity.hpp"
#include "RaZ/System.hpp"

namespace Raz {

struct FrameTimeInfo;
class World;
using WorldPtr = std::unique_ptr<World>;

/// World class handling systems & entities.
class World {
public:
  World() = default;
  explicit World(std::size_t entityCount) { m_entities.reserve(entityCount); }
  World(const World&) = delete;
  World(World&&) noexcept = default;

  const std::vector<SystemPtr>& getSystems() const { return m_systems; }
  const std::vector<EntityPtr>& getEntities() const { return m_entities; }

  /// Adds a given system to the world.
  /// \tparam SysT Type of the system to be added.
  /// \tparam Args Types of the arguments to be forwarded to the given system.
  /// \param args Arguments to be forwarded to the given system.
  /// \return Reference to the newly added system.
  template <typename SysT, typename... Args> SysT& addSystem(Args&&... args);
  /// Tells if a given system exists within the world.
  /// \tparam SysT Type of the system to be checked.
  /// \return True if the given system is present, false otherwise.
  template <typename SysT> bool hasSystem() const;
  /// Gets a given system contained by the world.
  /// This system must be present within the world. If not, an exception is thrown.
  /// \tparam SysT Type of the system to be fetched.
  /// \return Constant reference to the found system.
  template <typename SysT> const SysT& getSystem() const;
  /// Gets a given system contained by the world.
  /// This system must be present within the world. If not, an exception is thrown.
  /// \tparam SysT Type of the system to be fetched.
  /// \return Reference to the found system.
  template <typename SysT> SysT& getSystem() { return const_cast<SysT&>(static_cast<const World*>(this)->getSystem<SysT>()); }
  /// Removes the given system from the world.
  /// \tparam SysT Type of the system to be removed.
  template <typename SysT> void removeSystem();
  /// Adds an entity into the world.
  /// \param enabled True if the entity should be active immediately, false otherwise.
  /// \return Reference to the newly created entity.
  Entity& addEntity(bool enabled = true);
  /// Adds an entity into the world with a given component. This entity will be automatically enabled.
  /// \tparam CompT Type of the component to be added into the entity.
  /// \tparam Args Types of the arguments to be forwarded to the given component.
  /// \param args Arguments to be forwarded to the given component.
  /// \return Reference to the newly added entity.
  template <typename CompT, typename... Args> Entity& addEntityWithComponent(Args&&... args);
  /// Adds an entity into the world with several components at once.
  /// \tparam CompsTs Types of the components to be added into the entity.
  /// \param enabled True if the entity should be active immediately, false otherwise.
  /// \return Reference to the newly added entity.
  template <typename... CompsTs> Entity& addEntityWithComponents(bool enabled = true);
  /// Fetches entities which contain specific component(s).
  /// \tparam CompsTs Types of the components to query.
  /// \return List of entities containing all given components.
  template <typename... CompsTs> std::vector<Entity*> recoverEntitiesWithComponents();
  /// Removes an entity from the world. It *must* be an entity created by this world.
  /// \param entity Entity to be removed.
  void removeEntity(const Entity& entity);
  /// Updates the world, updating all the systems it contains.
  /// \param timeInfo Time-related frame information.
  /// \return True if the world still has active systems, false otherwise.
  bool update(const FrameTimeInfo& timeInfo);
  /// Refreshes the world, optimizing the entities & linking/unlinking entities to systems if needed.
  void refresh();
  /// Destroys the world, releasing all its entities & systems.
  void destroy();

  World& operator=(const World&) = delete;
  World& operator=(World&&) noexcept = default;

  ~World() { destroy(); }

private:
  /// Sorts entities so that the disabled ones are packed to the end of the list.
  void sortEntities();

  std::vector<SystemPtr> m_systems {};
  Bitset m_activeSystems {};

  std::vector<EntityPtr> m_entities {};
  std::size_t m_activeEntityCount = 0;
  std::size_t m_maxEntityIndex = 0;
};

} // namespace Raz

#include "RaZ/World.inl"

#endif // RAZ_WORLD_HPP
