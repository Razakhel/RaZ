#pragma once

#ifndef RAZ_FLOATUTILS_HPP
#define RAZ_FLOATUTILS_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Raz {

template <typename T, std::size_t Size>
class Vector;

template <typename T, std::size_t W, std::size_t H>
class Matrix;

template <typename T>
class Quaternion;

namespace FloatUtils {

/// Checks if two given floating point values are nearly equal to each other.
/// \tparam T Type of both values to check; must be a floating point type.
/// \tparam TolT Tolerance type, which may differ from the values type; must be a floating point type.
/// \param val1 First value to compare.
/// \param val2 Second value to compare.
/// \param absTol Absolute tolerance to compare the values with.
/// \return True if values are nearly equal to each other, false otherwise.
template <typename T, typename TolT = T>
constexpr bool areNearlyEqual(T val1, T val2, TolT absTol = std::numeric_limits<TolT>::epsilon()) noexcept {
  static_assert(std::is_floating_point_v<T>, "Error: Values type must be floating point.");
  static_assert(std::is_floating_point_v<TolT>, "Error: Tolerance type must be floating point.");

  // Using absolute & relative tolerances for floating points types: http://www.realtimecollisiondetection.net/pubs/Tolerances/
  // Could be a better idea to use ULPs checking. May be slower though?
  const T absDiff = std::abs(val1 - val2);
  return (absDiff <= static_cast<T>(absTol) * std::max(static_cast<T>(1), std::max(std::abs(val1), std::abs(val2))));
}

/// Checks if two given floating point vectors are nearly equal to each other.
/// \tparam T Type of both vectors' values to check; must be a floating point type.
/// \tparam Size Vectors' size.
/// \tparam TolT Tolerance type, which may differ from value type; must be a floating point type.
/// \param vec1 First vector to compare.
/// \param vec2 Second vector to compare.
/// \param absTol Absolute tolerance to compare the vectors' values with.
/// \return True if vectors are nearly equal to each other, false otherwise.
template <typename T, std::size_t Size, typename TolT = T>
constexpr bool areNearlyEqual(const Vector<T, Size>& vec1, const Vector<T, Size>& vec2, TolT absTol = std::numeric_limits<TolT>::epsilon()) noexcept {
  for (std::size_t i = 0; i < Size; ++i) {
    if (!areNearlyEqual(vec1[i], vec2[i], absTol))
      return false;
  }

  return true;
}

/// Checks if two given floating point matrices are nearly equal to each other.
/// \tparam T Type of both matrices' values to check; must be a floating point type.
/// \tparam W Matrices' width.
/// \tparam H Matrices' height.
/// \tparam TolT Tolerance type, which may differ from value type; must be a floating point type.
/// \param mat1 First matrix to compare.
/// \param mat2 Second matrix to compare.
/// \param absTol Absolute tolerance to compare the matrices' values with.
/// \return True if matrices are nearly equal to each other, false otherwise.
template <typename T, std::size_t W, std::size_t H, typename TolT = T>
constexpr bool areNearlyEqual(const Matrix<T, W, H>& mat1, const Matrix<T, W, H>& mat2, TolT absTol = std::numeric_limits<TolT>::epsilon()) noexcept {
  for (std::size_t i = 0; i < W * H; ++i) {
    if (!areNearlyEqual(mat1[i], mat2[i], absTol))
      return false;
  }

  return true;
}

template <typename T, typename TolT = T>
constexpr bool areNearlyEqual(const Quaternion<T>& quat1, const Quaternion<T>& quat2, TolT absTol = std::numeric_limits<TolT>::epsilon()) noexcept {
  if (!areNearlyEqual(quat1.w(), quat2.w(), absTol)
   || !areNearlyEqual(quat1.x(), quat2.x(), absTol)
   || !areNearlyEqual(quat1.y(), quat2.y(), absTol)
   || !areNearlyEqual(quat1.z(), quat2.z(), absTol))
    return false;

  return true;
}

} // namespace FloatUtils

} // namespace Raz

#endif // RAZ_FLOATUTILS_HPP
