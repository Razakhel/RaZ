#pragma once

#ifndef RAZ_COMPONENT_HPP
#define RAZ_COMPONENT_HPP

#include <memory>

namespace Raz {

class Component;
using ComponentPtr = std::unique_ptr<Component>;

class Component {
public:
  template <typename T> static std::size_t getId();

  virtual ~Component() = default;

protected:
  Component() = default;

private:
  static std::size_t m_maxId;
};

} // namespace Raz

#include "RaZ/Component.inl"

#endif // RAZ_COMPONENT_HPP
