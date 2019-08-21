#pragma once

#ifndef RAZ_QUATERNION_HPP
#define RAZ_QUATERNION_HPP

#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

/// Quaternion representing a rotation in 3D space.
/// Quaternions are used to avoid <a href="https://en.wikipedia.org/wiki/Gimbal_lock">gimbal locks</a>, present with Euler angles.
/// \tparam T Type of the values to be held by the quaternion.
template <typename T = float>
class Quaternion {
  static_assert(std::is_floating_point_v<T>, "Error: Quaternion's type must be floating point.");

public:
  Quaternion(Radians<T> angle, const Vec3<T>& axis);
  Quaternion(Radians<T> angle, float axisX, float axisY, float axisZ) : Quaternion(angle, Vec3<T>({ axisX, axisY, axisZ })) {}
  Quaternion(const Quaternion&) = default;
  Quaternion(Quaternion&&) noexcept = default;

  /// Computes the norm of the quaternion.
  /// Calculating the actual norm requires a square root operation to be involved, which is expensive.
  /// As such, this function should be used if actual length is needed; otherwise, prefer computeSquaredNorm().
  /// \return Quaternion's norm.
  T computeNorm() const { return std::sqrt(computeSquaredNorm()); }
  /// Computes the squared norm of the quaternion.
  /// The squared norm is equal to the addition of all components (real & complexes alike) squared.
  /// This calculation does not involve a square root; it is then to be preferred over computeNorm() for faster operations.
  /// \return Quaternion's squared norm.
  T computeSquaredNorm() const { return (m_real * m_real + m_complexes.computeSquaredLength()); }
  /// Computes the normalized quaternion to make it a unit one.
  /// A unit quaternion is also called a <a href="https://en.wikipedia.org/wiki/Versor">versor</a>.
  /// \return Normalized quaternion.
  Quaternion<T> normalize() const;
  /// Computes the conjugate of the quaternion.
  /// A quaternion's conjugate is simply computed by multiplying the complex components by -1.
  /// \return Quaternion's conjugate.
  Quaternion<T> conjugate() const;
  /// Computes the inverse (or reciprocal) of the quaternion.
  /// Inversing a quaternion consists of dividing the components of the conjugate by the squared norm.
  /// \return Quaternion's inverse.
  Quaternion<T> inverse() const;
  /// Computes the rotation matrix represented by the quaternion.
  /// This operation automatically scales the matrix so that it returns a unit one.
  /// \return Rotation matrix.
  Mat4<T> computeMatrix() const;

  /// Default copy assignment operator.
  /// \return Reference to the copied quaternion.
  Quaternion& operator=(const Quaternion&) = default;
  /// Default move assignment operator.
  /// \return Reference to the moved quaternion.
  Quaternion& operator=(Quaternion&&) noexcept = default;

private:
  T m_real {};
  Vec3<T> m_complexes {};
};

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

} // namespace Raz

#include "RaZ/Math/Quaternion.inl"

#endif // RAZ_QUATERNION_HPP
