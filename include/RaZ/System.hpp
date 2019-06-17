#pragma once

#ifndef RAZ_SYSTEM_HPP
#define RAZ_SYSTEM_HPP

#include <vector>

#include "RaZ/Entity.hpp"
#include "RaZ/Utils/Bitset.hpp"

namespace Raz {

class System;
using SystemPtr = std::unique_ptr<System>;

/// System class representing a base System to be inherited.
class System {
public:
  const Bitset& getAcceptedComponents() const { return m_acceptedComponents; }

  /// Gets the ID of the given system.
  /// It uses CRTP to assign a different ID to each system it is called with.
  /// This function will be compiled every time it is called with a different system type, incrementing the assigned index.
  /// Note that it must be called directly from System, and a derived class must be given (System::getId<DerivedSystem>()).
  /// \tparam T Type of the system to get the ID for.
  /// \return Given system's ID.
  template <typename T> static std::size_t getId();
  /// Checks if the system contains the given entity.
  /// \param entity Entity to be checked.
  /// \return True if the system contains the entity, false otherwise.
  bool containsEntity(const EntityPtr& entity);
  /// Links the entity to the system.
  /// \param entity Entity to be linked.
  virtual void linkEntity(const EntityPtr& entity);
  /// Unlinks the entity from the system.
  /// \param entity Entity to be unlinked.
  virtual void unlinkEntity(const EntityPtr& entity);
  /// Updates the system.
  /// This function is pure virtual and so must be reimplemented in the derived classes.
  /// \param deltaTime Time elapsed since the last update.
  /// \return True if the system is still active, false otherwise.
  virtual bool update(float deltaTime) = 0;
  /// Destroys the system.
  virtual void destroy() {}

  virtual ~System() = default;

protected:
  System() = default;

  std::vector<Entity*> m_entities {};
  Bitset m_acceptedComponents {};

private:
  static inline std::size_t m_maxId = 0;
};

} // namespace Raz

#include "RaZ/System.inl"

#endif // RAZ_SYSTEM_HPP

