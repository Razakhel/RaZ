#pragma once

#ifndef RAZ_MATHUTILS_HPP
#define RAZ_MATHUTILS_HPP

#include "RaZ/Math/Constants.hpp"
#include "RaZ/Math/Vector.hpp"

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <vector>

namespace Raz::MathUtils {

/// Computes the linear interpolation between two values, according to a coefficient.
/// \tparam T Type to compute the interpolation with.
/// \tparam CoeffT Type of the coefficient to apply.
/// \param min Minimum value (lower bound).
/// \param max Maximum value (upper bound).
/// \param coeff Coefficient between 0 (returns `min`) and 1 (returns `max`).
/// \return Computed linear interpolation between `min` and `max`.
template <typename T, typename CoeffT>
constexpr T lerp(T min, T max, CoeffT coeff) noexcept {
  static_assert(std::is_floating_point_v<CoeffT>, "Error: The coefficient type must be floating point.");

  return static_cast<T>(static_cast<CoeffT>(min) * (1 - coeff) + static_cast<CoeffT>(max) * coeff);
}

/// Computes the component-wise linear interpolation between two vectors, according to a coefficient.
/// \tparam T Type of the vectors' values to compute the interpolation with.
/// \tparam Size Size of the vectors to compute the interpolation with.
/// \tparam CoeffT Type of the coefficient to apply.
/// \param min Minimum value (lower bound).
/// \param max Maximum value (upper bound).
/// \param coeff Coefficient between 0 (returns `min`) and 1 (returns `max`).
/// \return Computed linear interpolation between `min` and `max`.
template <typename T, std::size_t Size, typename CoeffT>
constexpr Vector<T, Size> lerp(const Vector<T, Size>& min, const Vector<T, Size>& max, CoeffT coeff) noexcept {
  static_assert(std::is_floating_point_v<CoeffT>, "Error: The coefficient type must be floating point.");

  Vector<T, Size> res {};

  for (std::size_t i = 0; i < Size; ++i)
    res[i] = lerp(min[i], max[i], coeff);

  return res;
}

/// Computes the cubic [Hermite interpolation](https://en.wikipedia.org/wiki/Hermite_interpolation) of a value.
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
///             0.0             1.0
///
/// \tparam T Type to compute the interpolation with.
/// \param value Value to be interpolated (must be between 0 & 1).
/// \return The interpolated value (between 0 & 1).
template <typename T>
constexpr T smoothstep(T value) noexcept {
  static_assert(std::is_floating_point_v<T>, "Error: The interpolation type must be floating point.");
  assert("Error: The value must be between 0 & 1." && (value >= 0 && value <= 1));

  return value * value * (3 - 2 * value);
}

/// Computes the cubic [Hermite interpolation](https://en.wikipedia.org/wiki/Hermite_interpolation) between two thresholds.
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
  static_assert(std::is_floating_point_v<T>, "Error: The interpolation type must be floating point.");
  assert("Error: The smoothstep's maximum threshold must be greater than the minimum one." && maxThresh > minThresh);

  const T clampedVal = std::clamp((value - minThresh) / (maxThresh - minThresh), static_cast<T>(0), static_cast<T>(1));
  return smoothstep(clampedVal);
}

/// Computes a quintic interpolation of a value.
///
/// This is Ken Perlin's [smoothstep variation](https://en.wikipedia.org/wiki/Smoothstep#Variations), which produces a slightly smoother smoothstep.
/// \tparam T Type to compute the interpolation with.
/// \param value Value to be interpolated (must be between 0 & 1).
/// \return The interpolated value (between 0 & 1).
template <typename T>
constexpr T smootherstep(T value) noexcept {
  static_assert(std::is_floating_point_v<T>, "Error: The interpolation type must be floating point.");
  assert("Error: The value must be between 0 & 1." && (value >= 0 && value <= 1));

  return value * value * value * (value * (value * 6 - 15) + 10);
}

/// Computes a quintic interpolation between two thresholds.
///
/// This is Ken Perlin's [smoothstep variation](https://en.wikipedia.org/wiki/Smoothstep#Variations), which produces a slightly smoother smoothstep.
/// \tparam T Type to compute the interpolation with.
/// \param minThresh Minimum threshold value.
/// \param maxThresh Maximum threshold value.
/// \param value Value to be interpolated.
/// \return 0 if `value` is lower than `minThresh`.
/// \return 1 if `value` is greater than `maxThresh`.
/// \return The interpolated value (between 0 & 1) otherwise.
template <typename T>
constexpr T smootherstep(T minThresh, T maxThresh, T value) noexcept {
  static_assert(std::is_floating_point_v<T>, "Error: The interpolation type must be floating point.");
  assert("Error: The smootherstep's maximum threshold must be greater than the minimum one." && maxThresh > minThresh);

  const T clampedVal = std::clamp((value - minThresh) / (maxThresh - minThresh), static_cast<T>(0), static_cast<T>(1));
  return smootherstep(clampedVal);
}

/// Computes an [orthonormal basis from a single vector](https://graphics.pixar.com/library/OrthonormalB/), according to the right hand rule.
///
/// Note that the example diagram below represents one possible solution; the vectors may not necessarily be in these directions relatively to the input.
///
///           axis3
///            ^
///            |
///            +---> axis2
///           /
///          v
///       input
///
/// \tparam T Type of the basis' vectors.
/// \param input Base vector from which to compute the basis. Must be normalized.
/// \param axis2 Second vector of the computed basis.
/// \param axis3 Third vector of the computed basis.
template <typename T>
void computeOrthonormalBasis(const Vec3<T>& input, Vec3<T>& axis2, Vec3<T>& axis3) {
  static_assert(std::is_floating_point_v<T>, "Error: Vectors must be of a floating-point type to compute an orthonormal basis from.");

  const T sign = std::copysign(static_cast<T>(1), input.z());
  const T a    = static_cast<T>(-1) / (sign + input.z());
  const T b    = input.x() * input.y() * a;

  axis2 = Vec3<T>(static_cast<T>(1) + sign * input.x() * input.x() * a, sign * b, -sign * input.x());
  axis3 = Vec3<T>(b, sign + input.y() * input.y() * a, -input.y());
}

/// Computes points on the Fibonacci sphere, giving evenly distributed points around a unit sphere.
/// \param pointCount Number of points to compute.
/// \return Points on the Fibonacci sphere. All vectors are of length 1.
inline std::vector<Vec3f> computeFibonacciSpherePoints(std::size_t pointCount) {
  std::vector<Vec3f> fiboPoints(pointCount);

  for (std::size_t i = 0; i < pointCount; ++i) {
    const float theta  = 2.f * Pi<float> * static_cast<float>(i) / GoldenRatio<float>;
    const float cosPhi = 1.f - 2.f * (static_cast<float>(i) + 0.5f) / static_cast<float>(pointCount);
    const float sinPhi = std::sqrt(1.f - cosPhi * cosPhi);

    fiboPoints[i] = Vec3f(std::cos(theta) * sinPhi, cosPhi, std::sin(theta) * sinPhi);
  }

  return fiboPoints;
}

} // namespace Raz::MathUtils

#endif // RAZ_MATHUTILS_HPP
