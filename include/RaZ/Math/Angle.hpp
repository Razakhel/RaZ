#pragma once

#ifndef RAZ_ANGLE_HPP
#define RAZ_ANGLE_HPP

#include "RaZ/Math/Constants.hpp"
#include "RaZ/Utils/FloatUtils.hpp"

namespace Raz {

template <typename T> struct Degrees;
template <typename T> struct Radians;

/// Degrees class, handling mathematical angles in degrees.
/// \tparam T Type of the degrees' value.
template <typename T>
struct Degrees {
  static_assert(std::is_floating_point_v<T>, "Error: Degrees' type must be floating point.");

  explicit constexpr Degrees(T value) : value{ value } {}
  explicit constexpr Degrees(Radians<T> rad) : value{ rad * 180 / PI<T> } {}

  /// Checks if the current degrees angle is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param deg Degrees to be compared with.
  /// \return True if degrees' values are nearly equal, false otherwise.
  constexpr bool operator==(const Degrees& deg) const { return FloatUtils::areNearlyEqual(value, deg.value); }
  /// Checks if the current degrees angle is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param deg Degrees to be compared with.
  /// \return True if degrees' values are different, false otherwise.
  constexpr bool operator!=(const Degrees& deg) const { return !(*this == deg); }
  /// Implicit degrees' value type conversion operator.
  /// \tparam T2 Type to convert the value into.
  /// \return Degrees object of the new type.
  template <typename T2> constexpr operator Degrees<T2>() const { return Degrees<T2>(static_cast<T2>(value)); }
  /// Implicit degrees to radians conversion operator.
  /// \tparam T2 Type to convert the value into.
  /// \return Radians object of the new type.
  template <typename T2> constexpr operator Radians<T2>() const { return Radians<T2>(Degrees<T2>(static_cast<T2>(value))); }
  /// Implicit degrees to value conversion operator.
  constexpr operator T() const { return value; }

  T value;
};

using Degreesf  = Degrees<float>;
using Degreesd  = Degrees<double>;
using Degreesld = Degrees<long double>;

/// Radians class, handling mathematical angles in radians.
/// \tparam T Type of the radians' value.
template <typename T>
struct Radians {
  static_assert(std::is_floating_point_v<T>, "Error: Radians' type must be floating point.");

  explicit constexpr Radians(T value) : value{ value } {}
  explicit constexpr Radians(Degrees<T> deg) : value{ deg * PI<T> / 180 } {}

  /// Checks if the current radians angle is equal to another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param rad Radians to be compared with.
  /// \return True if radians' values are nearly equal, false otherwise.
  constexpr bool operator==(const Radians& rad) const { return FloatUtils::areNearlyEqual(value, rad.value); }
  /// Checks if the current radians angle is different from another given one.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param rad Radians to be compared with.
  /// \return True if radians' values are different, false otherwise.
  constexpr bool operator!=(const Radians& rad) const { return !(*this == rad); }
  /// Implicit radians' value type conversion operator.
  /// \tparam T2 Type to convert the value into.
  /// \return Radians object of the new type.
  template <typename T2> constexpr operator Radians<T2>() const { return Radians<T2>(static_cast<T2>(value)); }
  /// Implicit radians to degrees conversion operator.
  /// \tparam T2 Type to convert the value into.
  /// \return Degrees object of the new type.
  template <typename T2> constexpr operator Degrees<T2>() const { return Degrees<T2>(Radians<T2>(static_cast<T2>(value))); }
  /// Implicit radians to value conversion operator.
  constexpr operator T() const { return value; }

  T value;
};

using Radiansf  = Radians<float>;
using Radiansd  = Radians<double>;
using Radiansld = Radians<long double>;

namespace Literals {

/// Degrees user-defined literal.
/// \param value Value in degrees.
/// \return Degrees object containing the given value.
constexpr Degreesld operator "" _deg(long double value) { return Degreesld(value); }
/// Radians user-defined literal.
/// \param value Value in radians.
/// \return Radians object containing the given value.
constexpr Radiansld operator "" _rad(long double value) { return Radiansld(value); }

} // namespace Literals

} // namespace Raz

#endif // RAZ_ANGLE_HPP
