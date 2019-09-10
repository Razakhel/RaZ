#pragma once

#ifndef RAZ_FLOATUTILS_HPP
#define RAZ_FLOATUTILS_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Raz::FloatUtils {

/// Checks if two given floating point values are nearly equal to each other.
/// \tparam T Type of both values to check; it must be a floating point type.
/// \tparam TolT Tolerance type, which may differ from the values type; it must be a floating point type.
/// \param val1 First value to compare.
/// \param val2 Second value to compare.
/// \param absTol Absolute tolerance to compare the values with.
/// \return True if values are nearly equal to each other, false otherwise.
template <typename T, typename TolT = T>
inline constexpr bool areNearlyEqual(T val1, T val2, TolT absTol = std::numeric_limits<TolT>::epsilon()) {
  static_assert(std::is_floating_point_v<T>, "Error: Values type must be floating point.");
  static_assert(std::is_floating_point_v<TolT>, "Error: Tolerance type must be floating point.");

  // Using absolute & relative tolerances for floating points types: http://www.realtimecollisiondetection.net/pubs/Tolerances/
  // Could be a better idea to use ULPs checking. May be slower though?
  const T absDiff = std::abs(val1 - val2);
  return (absDiff <= absTol * std::max({ static_cast<T>(1), std::abs(val1), std::abs(val2) }));
}

} // namespace Raz::FloatUtils

#endif // RAZ_FLOATUTILS_HPP
