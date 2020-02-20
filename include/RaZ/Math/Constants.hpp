#pragma once

#ifndef RAZ_CONSTANTS_HPP
#define RAZ_CONSTANTS_HPP

namespace Raz {

/// Pi constant.
/// \tparam T Type of Pi.
template <typename T>
constexpr T Pi = static_cast<T>(3.14159265358979323846L);

/// Golden ratio constant. Equivalent to (1 + sqrt(5)) / 2.
/// \tparam T Type of the ratio.
template <typename T>
constexpr T GoldenRatio = static_cast<T>(1.61803398874989484820L);

} // namespace Raz

#endif // RAZ_CONSTANTS_HPP
