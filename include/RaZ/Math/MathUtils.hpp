#pragma once

#ifndef RAZ_MATHUTILS_HPP
#define RAZ_MATHUTILS_HPP

#include <algorithm>
#include <cassert>
#include <type_traits>

namespace Raz::MathUtils {

/// Computes the linear interpolation between two values, according to a coefficient.
/// \tparam T Type to compute the interpolation with.
/// \param min Minimum value (lower bound).
/// \param max Maximum value (upper bound).
/// \param coeff Coefficient between 0 (returns `min`) and 1 (returns `max`).
/// \return Computed linear interpolation between `min` and `max`.
template <typename T>
constexpr T interpolate(T min, T max, T coeff) noexcept {
  static_assert(std::is_floating_point_v<T>, "Error: Interpolation type must be floating point.");
  assert("Error: The interpolation coefficient must be between 0 & 1." && (coeff >= 0 && coeff <= 1));

  return min * (1 - coeff) + max * coeff;
}

/// Computes the [Hermite interpolation](https://en.wikipedia.org/wiki/Hermite_interpolation) between two thresholds.
///
/// Any value below `minThresh` will return 0, and any above `maxThresh` will return 1. Between both thresholds, a smooth interpolation is performed.
///
///       1.0    |               |___
///              |           .-~"|
///              |         ,^    |
///              |        /      |
///              |       /       |
///              |      /        |
///              |    ,v         |
///       0.0 ___|,.-"           |
///              ^               ^
///          minThresh       maxThresh
///
/// This is equivalent to [GLSL's smoothstep function](http://docs.gl/sl4/smoothstep).
/// \tparam T Type to compute the interpolation with.
/// \param minThresh Minimum threshold value.
/// \param maxThresh Maximum threshold value.
/// \param value Value to be interpolated.
/// \return 0 if `value` is lower than `minThresh`.
/// \return 1 if `value` is greater than `maxThresh`.
/// \return The interpolated value (between 0 & 1) otherwise.
template <typename T>
constexpr T smoothstep(T minThresh, T maxThresh, T value) noexcept {
  assert("Error: The smoothstep's maximum threshold must be greater than the minimum one." && maxThresh > minThresh);

  const T clampedVal = std::clamp((value - minThresh) / (maxThresh - minThresh), static_cast<T>(0), static_cast<T>(1));
  return clampedVal * clampedVal * (3 - 2 * clampedVal);
}

} // namespace Raz::MathUtils

#endif // RAZ_MATHUTILS_HPP
