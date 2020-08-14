#pragma once

#ifndef RAZ_MATHUTILS_HPP
#define RAZ_MATHUTILS_HPP

#include <type_traits>

namespace Raz::MathUtils {

/// Computes the linear interpolation between two values, according to a coefficient.
/// \tparam T Type to compute the interpolation with.
/// \param min Minimum value (lower bound).
/// \param max Maximum value (upper bound).
/// \param coeff Coefficient between 0 (returns min) and 1 (returns max).
/// \return Computed linear interpolation between min and max.
template <typename T>
constexpr T interpolate(T min, T max, T coeff) noexcept {
  static_assert(std::is_floating_point_v<T>, "Error: Interpolation type must be floating point.");
  assert("Error: The interpolation coefficient must be between 0 & 1." && (coeff >= 0 && coeff <= 1));

  return min * (1 - coeff) + max * coeff;
}

} // namespace Raz::MathUtils

#endif // RAZ_MATHUTILS_HPP
