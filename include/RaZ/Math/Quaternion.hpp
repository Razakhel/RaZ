#pragma once

#ifndef RAZ_QUATERNION_HPP
#define RAZ_QUATERNION_HPP

#include "RaZ/Math/Vector.hpp"

namespace Raz {

template <typename T>
struct Degrees;

template <typename T, std::size_t W, std::size_t H>
class Matrix;
template <typename T>
using Mat4 = Matrix<T, 4, 4>;

template <typename T>
class Quaternion;

template <typename T>
struct Radians;

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Quaternion<T>& quat);

/// Quaternion representing a rotation in 3D space.
/// Quaternions are used to avoid [gimbal locks](https://en.wikipedia.org/wiki/Gimbal_lock), present with Euler angles.
/// \tparam T Type of the values to be held by the quaternion.
template <typename T = float>
class Quaternion {
  static_assert(std::is_floating_point_v<T>, "Error: Quaternion's type must be floating point.");

public:
  constexpr Quaternion(T w, T x, T y, T z) noexcept : m_real{ w }, m_complexes(x, y, z) {}
  constexpr Quaternion(Radians<T> angle, const Vec3<T>& axis) noexcept;
  constexpr Quaternion(Radians<T> angle, T axisX, T axisY, T axisZ) noexcept : Quaternion(angle, Vec3<T>(axisX, axisY, axisZ)) {}
  constexpr Quaternion(const Quaternion&) noexcept = default;
  constexpr Quaternion(Quaternion&&) noexcept = default;

  constexpr T w() const noexcept { return m_real; }
  constexpr T x() const noexcept { return m_complexes.x(); }
  constexpr T y() const noexcept { return m_complexes.y(); }
  constexpr T z() const noexcept { return m_complexes.z(); }

  /// Creates a quaternion representing an identity transformation.
  /// \return Identity quaternion.
  static constexpr Quaternion identity() noexcept { return Quaternion(1, 0, 0, 0); }

  /// Computes the dot product between quaternions.
  /// \param quat Quaternion to compute the dot product with.
  /// \return Quaternions' dot product.
  constexpr T dot(const Quaternion& quat) const noexcept { return (m_real * quat.m_real + m_complexes.dot(quat.m_complexes)); }
  /// Computes the norm of the quaternion.
  /// Calculating the actual norm requires a square root operation to be involved, which is expensive.
  /// As such, this function should be used if actual length is needed; otherwise, prefer computeSquaredNorm().
  /// \return Quaternion's norm.
  constexpr T computeNorm() const noexcept { return std::sqrt(computeSquaredNorm()); }
  /// Computes the squared norm of the quaternion.
  /// The squared norm is equal to the addition of all components (real & complexes alike) squared.
  /// This calculation does not involve a square root; it is then to be preferred over computeNorm() for faster operations.
  /// \return Quaternion's squared norm.
  constexpr T computeSquaredNorm() const noexcept { return dot(*this); }
  /// Computes the normalized quaternion to make it a unit one.
  /// A unit quaternion is also called a [versor](https://en.wikipedia.org/wiki/Versor).
  /// \return Normalized quaternion.
  constexpr Quaternion normalize() const noexcept;
  /// Computes the linear interpolation between quaternions, according to a coefficient.
  /// \param quat Quaternion to be interpolated with.
  /// \param coeff Coefficient between 0 (returns the current quaternion) and 1 (returns the given quaternion).
  /// \return Linearly interpolated quaternion.
  constexpr Quaternion lerp(const Quaternion& quat, T coeff) const noexcept;
  /// Computes the normalized linear interpolation between quaternions, according to a coefficient.
  /// \param quat Quaternion to be interpolated with.
  /// \param coeff Coefficient between 0 (returns the normalized current quaternion) and 1 (returns the normalized given quaternion).
  /// \return Normalized linearly interpolated quaternion.
  constexpr Quaternion nlerp(const Quaternion& quat, T coeff) const noexcept;
  /// Computes the spherical linear interpolation between normalized quaternions, according to a coefficient.
  /// \note Both quaternions (the current & given ones) must be normalized before calling this function.
  /// \param quat Quaternion to be interpolated with. Must be normalized.
  /// \param coeff Coefficient between 0 (returns the current quaternion) and 1 (returns the given quaternion).
  /// \return Spherically interpolated quaternion.
  constexpr Quaternion slerp(const Quaternion& quat, T coeff) const noexcept;
  /// Computes the conjugate of the quaternion.
  /// A quaternion's conjugate is simply computed by multiplying the complex components by -1.
  /// \return Quaternion's conjugate.
  constexpr Quaternion conjugate() const noexcept;
  /// Computes the inverse (or reciprocal) of the quaternion.
  /// Inversing a quaternion consists of dividing the components of the conjugate by the squared norm.
  /// \return Quaternion's inverse.
  constexpr Quaternion inverse() const noexcept;
  /// Computes the rotation matrix represented by the quaternion.
  /// This operation automatically scales the matrix so that it returns a unit one.
  /// \return Rotation matrix.
  constexpr Mat4<T> computeMatrix() const noexcept;

  /// Default copy assignment operator.
  /// \return Reference to the copied quaternion.
  constexpr Quaternion& operator=(const Quaternion&) noexcept = default;
  /// Default move assignment operator.
  /// \return Reference to the moved quaternion.
  constexpr Quaternion& operator=(Quaternion&&) noexcept = default;
  /// Quaternions multiplication.
  /// \param quat Quaternion to be multiplied by.
  /// \return Result of the multiplied quaternions.
  constexpr Quaternion operator*(const Quaternion& quat) const noexcept;
  /// Quaternions multiplication.
  /// \param quat Quaternion to be multiplied by.
  /// \return Reference to the modified original quaternion.
  constexpr Quaternion& operator*=(const Quaternion& quat) noexcept;
  /// Quaternion equality comparison operator.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param quat Quaternion to be compared with.
  /// \return True if quaternions are nearly equal, else otherwise.
  constexpr bool operator==(const Quaternion& quat) const noexcept;
  /// Quaternion inequality comparison operator.
  /// Uses a near-equality check to take floating-point errors into account.
  /// \param quat Quaternion to be compared with.
  /// \return True if quaternions are different, else otherwise.
  constexpr bool operator!=(const Quaternion& quat) const noexcept { return !(*this == quat); }
  /// Matrix conversion operator; computes the rotation matrix represented by the quaternion.
  /// \return Rotation matrix.
  constexpr operator Mat4<T>() const noexcept { return computeMatrix(); }
  /// Output stream operator.
  /// \param stream Stream to output into.
  /// \param quat Quaternion to be output.
  friend std::ostream& operator<< <>(std::ostream& stream, const Quaternion& quat);

private:
  constexpr Quaternion lerp(const Quaternion& quat, T currCoeff, T otherCoeff) const noexcept;

  T m_real {};
  Vec3<T> m_complexes {};
};

/// Quaternion-vector multiplication. Rotates the vector by the rotation represented by the quaternion. The quaternion must be normalized.
/// \tparam T Type of the quaternion's & vector's data.
/// \param quat Quaternion to rotate by.
/// \param vec Vector to be rotated.
/// \return Rotated vector.
template <typename T>
constexpr Vec3<T> operator*(const Quaternion<T>& quat, const Vec3<T>& vec) noexcept;

/// Vector-quaternion multiplication. Rotates the vector by the inverse rotation represented by the quaternion. The quaternion must be normalized.
/// \tparam T Type of the vector's & quaternion's data.
/// \param vec Vector to be rotated.
/// \param quat Quaternion to rotate by.
/// \return Rotated vector.
template <typename T>
constexpr Vec3<T> operator*(const Vec3<T>& vec, const Quaternion<T>& quat) noexcept;

// Deduction guides

template <typename T>
Quaternion(Degrees<T>, ...) -> Quaternion<T>;

// Aliases

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

} // namespace Raz

#include "RaZ/Math/Quaternion.inl"

#endif // RAZ_QUATERNION_HPP
