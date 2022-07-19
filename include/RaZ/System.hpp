#pragma once

#ifndef RAZ_SYSTEM_HPP
#define RAZ_SYSTEM_HPP

#include "RaZ/Entity.hpp"
#include "RaZ/Data/Bitset.hpp"

#include <vector>

namespace Raz {

class System;
using SystemPtr = std::unique_ptr<System>;

/// System class representing a base System to be inherited.
class System {
  friend class World;

public:
  System(const System&) = delete;
  System(System&&) noexcept = delete;

  const Bitset& getAcceptedComponents() const { return m_acceptedComponents; }

  /// Gets the ID of the given system type.
  /// It uses CRTP to assign a different ID to each system type it is called with.
  /// This function will be instantiated every time it is called with a different type, incrementing the assigned index.
  /// Note that it must be called directly from System, and a derived class must be given (System::getId<DerivedSystem>()).
  /// \tparam SysT Type of the system to get the ID of.
  /// \return Given system's ID.
  template <typename SysT> static std::size_t getId();
  /// Checks if the system contains the given entity.
  /// \param entity Entity to be checked.
  /// \return True if the system contains the entity, false otherwise.
  bool containsEntity(const Entity& entity) const noexcept;
  /// Updates the system with a variable time step. For a constant step update, use step().
  /// \param deltaTime Time elapsed since the last update.
  /// \return True if the system is still active, false otherwise.
  virtual bool update([[maybe_unused]] float deltaTime) { return true; }
  /// Updates the system with a fixed time step, independent of the frame rate. If fixed steps are not needed, use update().
  /// \param deltaTime Step time elapsed since the last update.
  /// \return True if the system is still active, false otherwise.
  virtual bool step([[maybe_unused]] float deltaTime) { return true; }
  /// Destroys the system.
  virtual void destroy() {}

  System& operator=(const System&) = delete;
  System& operator=(System&&) noexcept = delete;

  virtual ~System() = default;

protected:
  System() = default;

  /// Adds the given component types as accepted by the current system.
  /// \tparam CompTs Types of the components to accept.
  template <typename... CompTs> void registerComponents() { (m_acceptedComponents.setBit(Component::getId<CompTs>()), ...); }
  /// Removes the given component types as accepted by the current system.
  /// \tparam CompTs Types of the components to deny.
  template <typename... CompTs> void unregisterComponents() { (m_acceptedComponents.setBit(Component::getId<CompTs>(), false), ...); }
  /// Links the entity to the system.
  /// \param entity Entity to be linked.
  virtual void linkEntity(const EntityPtr& entity);
  /// Unlinks the entity from the system.
  /// \param entity Entity to be unlinked.
  virtual void unlinkEntity(const EntityPtr& entity);

  std::vector<Entity*> m_entities {};
  Bitset m_acceptedComponents {};

private:
  static inline std::size_t m_maxId = 0;
};

} // namespace Raz

#include "RaZ/System.inl"

#endif // RAZ_SYSTEM_HPP

