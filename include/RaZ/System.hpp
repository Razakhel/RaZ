#pragma once

#ifndef RAZ_SYSTEM_HPP
#define RAZ_SYSTEM_HPP

#include <vector>

#include "RaZ/Entity.hpp"
#include "RaZ/Utils/Bitset.hpp"

namespace Raz {

class System;
using SystemPtr = std::unique_ptr<System>;

class System {
public:
  template <typename T> static std::size_t getId();

  const Bitset& getAcceptedComponents() const { return m_acceptedComponents; }

  bool containsEntity(const EntityPtr& entity);
  virtual void linkEntity(const EntityPtr& entity);
  virtual void unlinkEntity(const EntityPtr& entity);
  virtual void update(float deltaTime) = 0;
  virtual void destroy() {}

  virtual ~System() = default;

protected:
  System() = default;

  std::vector<Entity*> m_entities {};
  Bitset m_acceptedComponents {};

private:
  static std::size_t m_maxId;
};

} // namespace Raz

#include "RaZ/System.inl"

#endif // RAZ_SYSTEM_HPP

