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

  constexpr explicit Degrees(T val) : value{ val } {}
  constexpr explicit Degrees(Radians<T> rad) : value{ rad.value * 180 / PI<T> } {}
  constexpr Degrees(const Degrees&) = default;
  constexpr Degrees(Degrees&&) noexcept = default;

  /// Default copy assignment operator.
  /// \return Reference to the copied degrees object.
  constexpr Degrees& operator=(const Degrees&) = default;
  /// Default move assignment operator.
  /// \return Reference to the moved degrees object.
  constexpr Degrees& operator=(Degrees&&) noexcept = default;
  /// Degrees negation operator.
  /// \return Negated degrees object.
  constexpr Degrees operator-() const { return Degrees(-value); }
  /// Degrees-value addition operator.
  /// \param val Value to be added.
  /// \return Result of the degrees object summed with the value.
  constexpr Degrees operator+(T val) const { return Degrees(value + val); }
  /// Degrees-value substraction operator.
  /// \param val Value to be substracted.
  /// \return Result of the degrees object substracted by the value.
  constexpr Degrees operator-(T val) const { return Degrees(value - val); }
  /// Degrees-value multiplication operator.
  /// \param val Value to be multiplied by.
  /// \return Result of the degrees object multiplied by the value.
  constexpr Degrees operator*(T val) const { return Degrees(value * val); }
  /// Degrees-value division operator.
  /// \param val Value to be divided by.
  /// \return Result of the degrees object divided by the value.
  constexpr Degrees operator/(T val) const { return Degrees(value / val); }
  /// Radians-value addition assignment operator.
  /// \param val Value to be added.
  /// \return Reference to the modified original degrees object.
  constexpr Degrees& operator+=(T val) { value += val; return *this; }
  /// Radians-value substraction assignment operator.
  /// \param val Value to be substracted.
  /// \return Reference to the modified original degrees object.
  constexpr Degrees& operator-=(T val) { value -= val; return *this; }
  /// Radians-value multiplication assignment operator.
  /// \param val Value to be multiplied by.
  /// \return Reference to the modified original degrees object.
  constexpr Degrees& operator*=(T val) { value *= val; return *this; }
  /// Radians-value division assignment operator.
  /// \param val Value to be divided by.
  /// \return Reference to the modified original degrees object.
  constexpr Degrees& operator/=(T val) { value /= val; return *this; }
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

  constexpr explicit Radians(T val) : value{ val } {}
  constexpr explicit Radians(Degrees<T> deg) : value{ deg.value * PI<T> / 180 } {}
  Radians(const Radians&) = default;
  Radians(Radians&&) noexcept = default;

  /// Default copy assignment operator.
  /// \return Reference to the copied radians object.
  Radians& operator=(const Radians&) = default;
  /// Default move assignment operator.
  /// \return Reference to the moved radians object.
  Radians& operator=(Radians&&) noexcept = default;
  /// Radians negation operator.
  /// \return Negated radians object.
  constexpr Radians operator-() const { return Radians(-value); }
  /// Radians-value addition operator.
  /// \param val Value to be added.
  /// \return Result of the radians object summed with the value.
  constexpr Radians operator+(T val) const { return Radians(value + val); }
  /// Radians-value substraction operator.
  /// \param val Value to be substracted.
  /// \return Result of the radians object substracted by the value.
  constexpr Radians operator-(T val) const { return Radians(value - val); }
  /// Radians-value multiplication operator.
  /// \param val Value to be multiplied by.
  /// \return Result of the radians object multiplied by the value.
  constexpr Radians operator*(T val) const { return Radians(value * val); }
  /// Radians-value division operator.
  /// \param val Value to be divided by.
  /// \return Result of the radians object divided by the value.
  constexpr Radians operator/(T val) const { return Radians(value / val); }
  /// Radians-value addition assignment operator.
  /// \param val Value to be added.
  /// \return Reference to the modified original radians object.
  constexpr Radians& operator+=(T val) { value += val; return *this; }
  /// Radians-value substraction assignment operator.
  /// \param val Value to be substracted.
  /// \return Reference to the modified original radians object.
  constexpr Radians& operator-=(T val) { value -= val; return *this; }
  /// Radians-value multiplication assignment operator.
  /// \param val Value to be multiplied by.
  /// \return Reference to the modified original radians object.
  constexpr Radians& operator*=(T val) { value *= val; return *this; }
  /// Radians-value division assignment operator.
  /// \param val Value to be divided by.
  /// \return Reference to the modified original radians object.
  constexpr Radians& operator/=(T val) { value /= val; return *this; }
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

  T value;
};

using Radiansf  = Radians<float>;
using Radiansd  = Radians<double>;
using Radiansld = Radians<long double>;

namespace Literals {

/// Degrees user-defined literal.
/// \param val Value in degrees.
/// \return Degrees object containing the given value.
constexpr Degreesld operator "" _deg(long double val) { return Degreesld(val); }
/// Radians user-defined literal.
/// \param val Value in radians.
/// \return Radians object containing the given value.
constexpr Radiansld operator "" _rad(long double val) { return Radiansld(val); }

} // namespace Literals

} // namespace Raz

#endif // RAZ_ANGLE_HPP
