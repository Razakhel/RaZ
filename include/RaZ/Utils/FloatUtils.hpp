#pragma once

#ifndef RAZ_FLOATUTILS_HPP
#define RAZ_FLOATUTILS_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Raz {

namespace FloatUtils {

// Near-equality floating point check
template <typename T>
inline constexpr bool areNearlyEqual(T val1, T val2) {
  static_assert(std::is_floating_point<T>::value, "Error: Values' type must be floating point.");

  // Using absolute & relative tolerances for floating points types: http://www.realtimecollisiondetection.net/pubs/Tolerances/
  // Could be a better idea to use ULPs checking. May be slower though?
  const T absDiff = std::abs(val1 - val2);
  return (absDiff <= std::numeric_limits<T>::epsilon() * std::max({ static_cast<T>(1), std::abs(val1), std::abs(val2) }));
}

} // namespace FloatUtils

} // namespace Raz

#endif // RAZ_FLOATUTILS_HPP
