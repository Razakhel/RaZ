#pragma once

#ifndef RAZ_OWNERVALUE_HPP
#define RAZ_OWNERVALUE_HPP

#include <utility>

namespace Raz {

template <typename T, T InvalidValue = T()>
class OwnerValue {
public:
  constexpr OwnerValue() = default;
  constexpr explicit OwnerValue(T&& value) : m_value{ std::forward<T>(value) } {}
  constexpr OwnerValue(const OwnerValue&) = delete;
  constexpr OwnerValue(OwnerValue&& owner) noexcept : m_value{ std::exchange(owner.m_value, InvalidValue) } {}

  constexpr const T& get() const { return m_value; }
  constexpr T& get() { return m_value; }
  constexpr bool isValid() const { return (m_value != InvalidValue); }
  constexpr void reset() { m_value = InvalidValue; }

  constexpr OwnerValue& operator=(const OwnerValue&) = delete;
  constexpr OwnerValue& operator=(OwnerValue&& owner) noexcept { std::swap(m_value, owner.m_value); return *this; }
  constexpr OwnerValue& operator=(T&& value) { m_value = std::forward<T>(value); return *this; }
  constexpr operator const T&() const { return m_value; }
  constexpr operator T&() { return m_value; }

private:
  T m_value = InvalidValue;
};

} // namespace Raz

#endif // RAZ_OWNERVALUE_HPP
