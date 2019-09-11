#pragma once

#ifndef RAZ_CATCH_HPP
#define RAZ_CATCH_HPP

#include "catch/catch.hpp"
#include "RaZ/Utils/FloatUtils.hpp"

#include <iomanip>
#include <sstream>

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
  constexpr explicit IsNearlyEqualTo(T comparison, TolT absTol = std::numeric_limits<TolT>::epsilon()) : m_comparison{ comparison }, m_absTol{ absTol } {
    // Use 'double' if T is defined to 'long double', otherwise use the given type
    // 'long double' is not defined (yet?) for Catch's functions
    using NarrowT = std::conditional_t<std::is_same_v<T, long double>, double, T>;

    Catch::StringMaker<NarrowT>::precision = std::numeric_limits<NarrowT>::digits10 + 1;
  }

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

#endif // RAZ_CATCH_HPP
