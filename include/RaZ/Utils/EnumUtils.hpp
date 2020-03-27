#pragma once

#ifndef RAZ_ENUMUTILS_HPP
#define RAZ_ENUMUTILS_HPP

#include <type_traits>

/// Creates bitwise operators (NOT ~, AND &, OR |, XOR ^, and their assignment counterparts) for the given Enum type.
/// This allows an enum to be used as a flag.
/// \param Enum Enum type to create the operators for.
#define MAKE_ENUM_FLAG(Enum)                                                     \
  constexpr Enum operator~(Enum value) noexcept {                                \
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(value)); \
  }                                                                              \
                                                                                 \
  constexpr Enum operator&(Enum lhs, Enum rhs) noexcept {                        \
    using EnumT = std::underlying_type_t<Enum>;                                  \
    return static_cast<Enum>(static_cast<EnumT>(lhs) & static_cast<EnumT>(rhs)); \
  }                                                                              \
                                                                                 \
  constexpr Enum& operator&=(Enum& lhs, Enum rhs) noexcept {                     \
    lhs = lhs & rhs;                                                             \
    return lhs;                                                                  \
  }                                                                              \
                                                                                 \
  constexpr Enum operator|(Enum lhs, Enum rhs) noexcept {                        \
    using EnumT = std::underlying_type_t<Enum>;                                  \
    return static_cast<Enum>(static_cast<EnumT>(lhs) | static_cast<EnumT>(rhs)); \
  }                                                                              \
                                                                                 \
  constexpr Enum& operator|=(Enum& lhs, Enum rhs) noexcept {                     \
    lhs = lhs | rhs;                                                             \
    return lhs;                                                                  \
  }                                                                              \
                                                                                 \
  constexpr Enum operator^(Enum lhs, Enum rhs) noexcept {                        \
    using EnumT = std::underlying_type_t<Enum>;                                  \
    return static_cast<Enum>(static_cast<EnumT>(lhs) ^ static_cast<EnumT>(rhs)); \
  }                                                                              \
                                                                                 \
  constexpr Enum& operator^=(Enum& lhs, Enum rhs) noexcept {                     \
    lhs = lhs ^ rhs;                                                             \
    return lhs;                                                                  \
  }

#endif // RAZ_ENUMUTILS_HPP
