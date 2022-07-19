#pragma once

#ifndef RAZ_COMPONENT_HPP
#define RAZ_COMPONENT_HPP

#include <memory>

namespace Raz {

class Component;
using ComponentPtr = std::unique_ptr<Component>;

/// Component class representing a base Component to be inherited.
class Component {
public:
  /// Gets the ID of the given component type.
  /// It uses CRTP to assign a different ID to each component type it is called with.
  /// This function will be instantiated every time it is called with a different type, incrementing the assigned index.
  /// Note that it must be called directly from Component, and a derived class must be given (Component::getId<DerivedComponent>()).
  /// \tparam CompT Type of the component to get the ID of.
  /// \return Given component's ID.
  template <typename CompT> static std::size_t getId();

  virtual ~Component() = default;

protected:
  Component() = default;
  Component(const Component&) = default;
  Component(Component&&) noexcept = default;

  Component& operator=(const Component&) = default;
  Component& operator=(Component&&) noexcept = default;

private:
  static inline std::size_t m_maxId = 0;
};

} // namespace Raz

#include "RaZ/Component.inl"

#endif // RAZ_COMPONENT_HPP
