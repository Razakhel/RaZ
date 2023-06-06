#pragma once

#ifndef RAZ_CATCH_HPP
#define RAZ_CATCH_HPP

#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FloatUtils.hpp"

#include <catch/catch.hpp>

#include <iomanip>
#include <sstream>

namespace Raz { class Image; }

/// Custom Catch matcher, which checks for near-equality between floating point values.
/// \tparam T Type of the value to be compared to.
/// \tparam TolT Tolerance type, which may differ from the value type.
template <typename T, typename TolT = T>
class IsNearlyEqualTo final : public Catch::MatcherBase<T> {
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
    stream.precision(std::numeric_limits<T>::digits10 + 3);

    stream << "is not nearly equal to " << m_comparison;
    return stream.str();
  }

private:
  T m_comparison;
  TolT m_absTol;
};

template <typename T, typename TolT>
IsNearlyEqualTo(T, TolT) -> IsNearlyEqualTo<T, TolT>;

/// Custom Catch matcher, which checks for near-equality between floating point vectors' values.
/// \tparam T Type of the vector to be compared to.
/// \tparam Size Size of the vectors to be compared.
/// \tparam TolT Tolerance type, which may differ from the value type.
template <typename T, std::size_t Size, typename TolT = T>
class IsNearlyEqualToVector final : public Catch::MatcherBase<Raz::Vector<T, Size>> {
  static_assert(std::is_floating_point_v<T>, "Error: IsNearlyEqualToVector's value type must be floating point.");
  static_assert(std::is_floating_point_v<TolT>, "Error: IsNearlyEqualToVector's tolerance type must be floating point.");

public:
  /// Creates an instance of a near-equality vector check custom matcher.
  /// \param comparison Vector to be compared with.
  /// \param absTol Absolute tolerance to compare the values with.
  constexpr explicit IsNearlyEqualToVector(const Raz::Vector<T, Size>& comparison, TolT absTol = std::numeric_limits<TolT>::epsilon())
      : m_comparison{ comparison }, m_absTol{ absTol } {}

  /// Checks if the given vector has nearly equal values compared to the comparison one.
  /// \param base Base vector to compare to.
  /// \return True if values are nearly equal to each other, false otherwise.
  constexpr bool match(const Raz::Vector<T, Size>& base) const override {
    const_cast<IsNearlyEqualToVector*>(this)->m_base = base;

    return Raz::FloatUtils::areNearlyEqual(m_base, m_comparison, m_absTol);
  }

  /// Gets the description of the error if the match failed.
  /// \return Error string to be printed.
  std::string describe() const override {
    std::ostringstream stream;
    stream.precision(std::numeric_limits<T>::digits10 + 3);

    for (std::size_t i = 0; i < Size; ++i) {
      if (!Raz::FloatUtils::areNearlyEqual(m_base[i], m_comparison[i], m_absTol))
        stream << "\n\tAt [" << i << "]: " << m_base[i] << " is not nearly equal to " << m_comparison[i];
    }

    return stream.str();
  }

private:
  Raz::Vector<T, Size> m_comparison;
  Raz::Vector<T, Size> m_base;
  TolT m_absTol;
};

template <typename T, std::size_t Size, typename TolT>
IsNearlyEqualToVector(Raz::Vector<T, Size>, TolT) -> IsNearlyEqualToVector<T, Size, TolT>;

/// Custom Catch matcher, which checks for near-equality between floating point matrices' values.
/// \tparam T Type of the matrix to be compared to.
/// \tparam W Width of the matrices to be compared.
/// \tparam H Height of the matrices to be compared.
/// \tparam TolT Tolerance type, which may differ from the value type.
template <typename T, std::size_t W, std::size_t H, typename TolT = T>
class IsNearlyEqualToMatrix final : public Catch::MatcherBase<Raz::Matrix<T, W, H>> {
  static_assert(std::is_floating_point_v<T>, "Error: IsNearlyEqualToMatrix's value type must be floating point.");
  static_assert(std::is_floating_point_v<TolT>, "Error: IsNearlyEqualToMatrix's tolerance type must be floating point.");

public:
  /// Creates an instance of a near-equality matrix check custom matcher.
  /// \param comparison Matrix to be compared with.
  /// \param absTol Absolute tolerance to compare the values with.
  constexpr explicit IsNearlyEqualToMatrix(const Raz::Matrix<T, W, H>& comparison, TolT absTol = std::numeric_limits<TolT>::epsilon())
    : m_comparison{ comparison }, m_absTol{ absTol } {}

  /// Checks if the given matrix has nearly equal values compared to the comparison one.
  /// \param base Base matrix to compare to.
  /// \return True if values are nearly equal to each other, false otherwise.
  constexpr bool match(const Raz::Matrix<T, W, H>& base) const override {
    const_cast<IsNearlyEqualToMatrix*>(this)->m_base = base;

    return Raz::FloatUtils::areNearlyEqual(m_base, m_comparison, m_absTol);
  }

  /// Gets the description of the error if the match failed.
  /// \return Error string to be printed.
  std::string describe() const override {
    std::ostringstream stream;
    stream.precision(std::numeric_limits<T>::digits10 + 3);

    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
      const std::size_t finalWidthIndex = widthIndex * H;

      for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
        const std::size_t finalIndex = finalWidthIndex + heightIndex;

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

template <typename T, std::size_t W, std::size_t H, typename TolT>
IsNearlyEqualToMatrix(Raz::Matrix<T, W, H>, TolT) -> IsNearlyEqualToMatrix<T, W, H, TolT>;

/// Custom Catch matcher, which checks for near-equality between floating point quaternions' values.
/// \tparam T Type of the quaternion to be compared to.
/// \tparam TolT Tolerance type, which may differ from the value type.
template <typename T, typename TolT = T>
class IsNearlyEqualToQuaternion final : public Catch::MatcherBase<Raz::Quaternion<T>> {
  static_assert(std::is_floating_point_v<T>, "Error: IsNearlyEqualToQuaternion's value type must be floating point.");
  static_assert(std::is_floating_point_v<TolT>, "Error: IsNearlyEqualToQuaternion's tolerance type must be floating point.");

public:
  /// Creates an instance of a near-equality quaternion check custom matcher.
  /// \param comparison Quaternion to be compared with.
  /// \param absTol Absolute tolerance to compare the values with.
  constexpr explicit IsNearlyEqualToQuaternion(const Raz::Quaternion<T>& comparison, TolT absTol = std::numeric_limits<TolT>::epsilon())
    : m_comparison{ comparison }, m_absTol{ absTol } {}

  /// Checks if the given quaternion has nearly equal values compared to the comparison one.
  /// \param base Base quaternion to compare to.
  /// \return True if values are nearly equal to each other, false otherwise.
  constexpr bool match(const Raz::Quaternion<T>& base) const override {
    const_cast<IsNearlyEqualToQuaternion*>(this)->m_base = base;

    return Raz::FloatUtils::areNearlyEqual(m_base, m_comparison, m_absTol);
  }

  /// Gets the description of the error if the match failed.
  /// \return Error string to be printed.
  std::string describe() const override {
    std::ostringstream stream;
    stream.precision(std::numeric_limits<T>::digits10 + 3);

    if (!Raz::FloatUtils::areNearlyEqual(m_base.w(), m_comparison.w(), m_absTol))
      stream << "\n\tOn w: " << m_base.w() << " is not nearly equal to " << m_comparison.w();

    if (!Raz::FloatUtils::areNearlyEqual(m_base.x(), m_comparison.x(), m_absTol))
      stream << "\n\tOn x: " << m_base.x() << " is not nearly equal to " << m_comparison.x();

    if (!Raz::FloatUtils::areNearlyEqual(m_base.y(), m_comparison.y(), m_absTol))
      stream << "\n\tOn y: " << m_base.y() << " is not nearly equal to " << m_comparison.y();

    if (!Raz::FloatUtils::areNearlyEqual(m_base.z(), m_comparison.z(), m_absTol))
      stream << "\n\tOn z: " << m_base.z() << " is not nearly equal to " << m_comparison.z();

    return stream.str();
  }

private:
  Raz::Quaternion<T> m_comparison;
  Raz::Quaternion<T> m_base = Raz::Quaternion<T>::identity(); // m_base needs an instantiation, since a Quaternion doesn't have a default constructor
  TolT m_absTol;
};

template <typename T, typename TolT>
IsNearlyEqualToQuaternion(Raz::Quaternion<T>, TolT) -> IsNearlyEqualToQuaternion<T, TolT>;

/// Custom Catch matcher, which checks for near-equality between images.
class IsNearlyEqualToImage final : public Catch::MatcherBase<Raz::Image> {
public:
  /// Creates an instance of a near-equality image check custom matcher.
  /// \param comparison Image to be compared with.
  /// \param ratioTol Tolerance ratio to compare the images with, between [0; 1].
  /// \param diffImgPath Path to the image containing the differences to be output. If empty, no image will be saved.
  explicit IsNearlyEqualToImage(const Raz::Image& comparison, float ratioTol = 0.04f, Raz::FilePath diffImgPath = {})
    : m_comparison{ comparison }, m_ratioTol{ ratioTol }, m_diffImgPath{ std::move(diffImgPath) } {}

  /// Checks if the given image has nearly equal values compared to the comparison one.
  /// \param base Base image to compare to.
  /// \return True if images are nearly equal to each other, false otherwise.
  bool match(const Raz::Image& base) const override;

  /// Gets the description of the error if the match failed.
  /// \return Error string to be printed.
  std::string describe() const override;

private:
  void matchByte(const Raz::Image& base, Raz::Image& diffImg) const;
  void matchFloat(const Raz::Image& base, Raz::Image& diffImg) const;

  const Raz::Image& m_comparison;
  mutable std::size_t m_diffValueCount = 0;
  mutable float m_minDiff = std::numeric_limits<float>::max();
  mutable float m_maxDiff {};
  mutable float m_avgDiff {};
  float m_ratioTol {};
  Raz::FilePath m_diffImgPath {};
};

#endif // RAZ_CATCH_HPP
