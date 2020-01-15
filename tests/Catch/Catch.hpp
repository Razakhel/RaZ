#pragma once

#ifndef RAZ_CATCH_HPP
#define RAZ_CATCH_HPP

#include "RaZ/Utils/FloatUtils.hpp"

#include <catch/catch.hpp>

#include <iomanip>
#include <sstream>

using namespace std::literals;

/// Custom Catch matcher, which checks for near-equality between floating point values.
/// \tparam T Type of the value to be compared to.
/// \tparam TolT Tolerance type, which may differ from the value type.
template <typename T, typename TolT = T>
class IsNearlyEqualTo : public Catch::MatcherBase<T> {
  static_assert(std::is_floating_point_v<T>, "Error: IsNearlyEqualTo's value type must be floating point.");
  static_assert(std::is_floating_point_v<TolT>, "Error: IsNearlyEqualTo's tolerance type must be floating point.");

public:
  /// Creates an instance of a near-equality check custom matcher.
  /// \param comparison Value to be compared with.
  /// \param absTol Absolute tolerance to compare the values with.
  constexpr explicit IsNearlyEqualTo(T comparison, TolT absTol = std::numeric_limits<TolT>::epsilon())
    : m_comparison{ comparison }, m_absTol{ absTol } {}

  /// Checks if the given value is nearly equal to the comparison one.
  /// \param base Base value to compare to.
  /// \return True if values are nearly equal to each other, false otherwise.
  constexpr bool match(const T& base) const override {
    return Raz::FloatUtils::areNearlyEqual(base, m_comparison, m_absTol);
  }

  /// Gets the description of the error if the match failed.
  /// \return Error string to be printed.
  std::string describe() const override {
    std::ostringstream stream;
    stream.precision(std::numeric_limits<T>::digits10 + 1);
    stream << "is not nearly equal to " << m_comparison;
    return stream.str();
  }

private:
  T m_comparison;
  TolT m_absTol;
};

/// Custom Catch matcher, which checks for near-equality between floating point matrices' values.
/// \tparam T Type of the matrix to be compared to.
/// \tparam TolT Tolerance type, which may differ from the value type.
template <typename T, std::size_t W, std::size_t H, typename TolT = T>
class IsNearlyEqualToMatrix : public Catch::MatcherBase<Raz::Matrix<T, W, H>> {
  static_assert(std::is_floating_point_v<T>, "Error: IsNearlyEqualToMatrix's value type must be floating point.");
  static_assert(std::is_floating_point_v<TolT>, "Error: IsNearlyEqualToMatrix's tolerance type must be floating point.");

public:
  /// Creates an instance of a near-equality matrix check custom matcher.
  /// \param comparison Matrix to be compared with.
  /// \param absTol Absolute tolerance to compare the values with.
  constexpr explicit IsNearlyEqualToMatrix(const Raz::Matrix<T, W, H>& comparison, TolT absTol = std::numeric_limits<TolT>::epsilon())
    : m_comparison{ comparison }, m_absTol{ absTol } {}

  /// Checks if the given matrix has nearly equal values compared to the comparison one.
  /// \param base Base Matrix to compare to.
  /// \return True if values are nearly equal to each other, false otherwise.
  constexpr bool match(const Raz::Matrix<T, W, H>& base) const override {
    const_cast<IsNearlyEqualToMatrix*>(this)->m_base = base;

    return Raz::FloatUtils::areNearlyEqual(m_base, m_comparison, m_absTol);
  }

  /// Gets the description of the error if the match failed.
  /// \return Error string to be printed.
  std::string describe() const override {
    std::ostringstream stream;
    stream.precision(std::numeric_limits<T>::digits10 + 1);

    for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
      const std::size_t finalHeightIndex = heightIndex * W;

      for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
        const std::size_t finalIndex = finalHeightIndex + widthIndex;

        if (!Raz::FloatUtils::areNearlyEqual(m_base[finalIndex], m_comparison[finalIndex], m_absTol))
          stream << "\n\tAt [" << widthIndex << "][" << heightIndex << "]: " << m_base[finalIndex] << " is not nearly equal to " << m_comparison[finalIndex];
      }
    }

    return stream.str();
  }

private:
  Raz::Matrix<T, W, H> m_comparison;
  Raz::Matrix<T, W, H> m_base;
  TolT m_absTol;
};

#endif // RAZ_CATCH_HPP
