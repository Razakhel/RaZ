#pragma once

#ifndef RAZ_VECTOR_HPP
#define RAZ_VECTOR_HPP

#include <array>
#include <cmath>
#include <iostream>
#include <limits>

namespace Raz {

// Forward declaration of Matrix, to allow its usage into functions
template <typename T, std::size_t W, std::size_t H>
class Matrix;

template <typename T, std::size_t Size>
class Vector;

template <typename T, std::size_t Size>
std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vec);

/// Vector class, representing a mathematical vector, with generic type and size.
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
template <typename T, std::size_t Size>
class Vector {
public:
  constexpr Vector() noexcept = default;
  constexpr explicit Vector(const Vector<T, Size + 1>& vec) noexcept;
  constexpr Vector(const Vector<T, Size - 1>& vec, T val) noexcept;
  constexpr explicit Vector(T val) noexcept;
  template <typename... Args,
            typename = std::enable_if_t<sizeof...(Args) == Size>, // There can't be more or less values than Size
            typename = std::enable_if_t<(std::is_convertible_v<std::decay_t<Args>, T> && ...)>> // Given values must be of a convertible type
  constexpr explicit Vector(Args&&... args) noexcept : m_data{ static_cast<T>(args)... } {}
  constexpr Vector(const Vector&) noexcept = default;
  constexpr Vector(Vector&&) noexcept = default;

  constexpr std::size_t getSize() const noexcept { return Size; }
  constexpr const std::array<T, Size>& getData() const noexcept { return m_data; }
  constexpr std::array<T, Size>& getData() noexcept { return m_data; }
  constexpr const T* getDataPtr() const noexcept { return m_data.data(); }
  constexpr T* getDataPtr() noexcept { return m_data.data(); }

  /// Gets the vector's X component.
  /// \return Constant reference to the X component.
  constexpr const T& x() const noexcept;
  /// Gets the vector's X component.
  /// \return Reference to the X component.
  constexpr T& x() noexcept;
  /// Gets the vector's Y component.
  /// \return Constant reference to the Y component.
  constexpr const T& y() const noexcept;
  /// Gets the vector's Y component.
  /// \return Reference to the Y component.
  constexpr T& y() noexcept;
  /// Gets the vector's Z component.
  /// \return Constant reference to the Z component.
  constexpr const T& z() const noexcept;
  /// Gets the vector's Z component.
  /// \return Reference to the Z component.
  constexpr T& z() noexcept;
  /// Gets the vector's W component.
  /// \return Constant reference to the W component.
  constexpr const T& w() const noexcept;
  /// Gets the vector's W component.
  /// \return Reference to the W component.
  constexpr T& w() noexcept;
  /// Computes the dot product (scalar product) between the current vector & the given one.
  /// The dot product calculates the projection of one of the vectors onto the other; the order doesn't matter.
  /// On normalized vectors, the returned value represents the cosine of the angle (in radians) between them.
  /// \param vec Vector to compute the dot product with.
  /// \return Dot product value.
  constexpr T dot(const Vector& vec) const noexcept;
  /// Computes the cross product (vector product) between the current vector & the given one.
  /// The cross product generates a vector which is orthogonal to the two others.
  /// \param vec Vector to compute the cross product with.
  /// \return Computed orthogonal vector.
  constexpr Vector cross(const Vector& vec) const noexcept;
  /// Computes the reflection of the current vector over a direction.
  /// The calling vector is assumed to be incident (directed to the surface it is reflected on).
  /// \imageSize{vector_reflect.jpg, height: 20%; width: 20%;}
  /// \image html vector_reflect.jpg "Incident vector reflection"
  /// \param normal Direction to compute the reflection over.
  /// \return Vector's reflection.
  constexpr Vector reflect(const Vector& normal) const noexcept { return (*this - normal * dot(normal) * 2); }
  /// Computes the length of the vector.
  /// Calculating the actual length requires a square root operation to be involved, which is expensive.
  /// As such, this function should be used if actual length is needed; otherwise, prefer computeSquaredLength().
  /// \return Vector's length.
  constexpr float computeLength() const noexcept { return std::sqrt(computeSquaredLength()); }
  /// Computes the squared length of the vector.
  /// The squared length is equal to the dot product of the vector with itself.
  /// This calculation does not involve a square root; it is then to be preferred over computeLength() for faster operations.
  /// \return Vector's squared length.
  constexpr float computeSquaredLength() const noexcept { return dot(*this); }
  /// Computes the normalized vector.
  /// Normalizing a vector makes it of length 1.
  /// \return Normalized vector.
  constexpr Vector normalize() const noexcept;
  /// Computes the linear interpolation between vectors, according to a coefficient.
  /// \param vec Vector to be interpolated with.
  /// \param coeff Coefficient between 0 (returns the current vector) and 1 (returns the given vector).
  /// \return Linearly interpolated vector.
  constexpr Vector lerp(const Vector& vec, float coeff) const noexcept;
  /// Computes the normalized linear interpolation between vectors, according to a coefficient.
  /// \param vec Vector to be interpolated with.
  /// \param coeff Coefficient between 0 (returns the normalized current vector) and 1 (returns the normalized given vector).
  /// \return Normalized linearly interpolated vector.
  constexpr Vector nlerp(const Vector& vec, float coeff) const noexcept { return lerp(vec, coeff).normalize(); }
  /// Checks for strict equality between the current vector & the given one.
  /// \param vec Vector to be compared with.
  /// \return True if vectors are strictly equal to each other, false otherwise.
  constexpr bool strictlyEquals(const Vector& vec) const noexcept;
  /// Computes the unique hash of the current vector.
  /// \param seed Value to use as a hash seed.
  /// \return Vector's hash.
  constexpr std::size_t hash(std::size_t seed = 0) const noexcept;

  /// Default copy assignment operator.
  /// \return Reference to the copied vector.
  constexpr Vector& operator=(const Vector&) noexcept = default;
  /// Default move assignment operator.
  /// \return Reference to the moved vector.
  constexpr Vector& operator=(Vector&&) noexcept = default;
  /// Vector negation operator.
  /// This unary minus negates the components of the vector, reversing its direction.
  /// \return Negated vector.
  constexpr Vector operator-() const noexcept { return (*this * -1); }
  /// Element-wise vector-vector addition operator.
  /// \param vec Vector to be added.
  /// \return Result of the summed vectors.
  constexpr Vector operator+(const Vector& vec) const noexcept;
  /// Element-wise vector-value addition operator.
  /// \param val Value to be added.
  /// \return Result of the vector summed with the value.
  constexpr Vector operator+(T val) const noexcept;
  /// Element-wise vector-vector substraction operator.
  /// \param vec Vector to be substracted.
  /// \return Result of the substracted vectors.
  constexpr Vector operator-(const Vector& vec) const noexcept;
  /// Element-wise vector-value substraction operator.
  /// \param val Value to be substracted.
  /// \return Result of the vector substracted by the value.
  constexpr Vector operator-(T val) const noexcept;
  /// Element-wise vector-vector multiplication operator.
  /// \param vec Vector to be multiplied by.
  /// \return Result of the multiplied vectors.
  constexpr Vector operator*(const Vector& vec) const noexcept;
  /// Element-wise vector-value multiplication operator.
  /// \tparam T2 Type of the given value.
  /// \param val Value to be multiplied by.
  /// \return Result of the vector multiplied by the value.
  template <typename T2> constexpr Vector operator*(T2 val) const noexcept;
  /// Element-wise vector-vector division operator.
  /// \param vec Vector to be divided by.
  /// \return Result of the summed vectors.
  constexpr Vector operator/(const Vector& vec) const noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559);
  /// Element-wise vector-value division operator.
  /// \param val Value to be divided by.
  /// \return Result of the vector divided by the value.
  constexpr Vector operator/(T val) const noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559);
  /// Vector-matrix multiplication operator (assumes the vector to be horizontal).
  /// \param mat Matrix to be multiplied by.
  /// \return Result of the vector-matrix multiplication.
  template <std::size_t H> constexpr Vector operator*(const Matrix<T, Size, H>& mat) const noexcept;
  /// Element-wise vector-vector addition assignment operator.
  /// \param vec Vector to be added.
  /// \return Reference to the modified original vector.
  constexpr Vector& operator+=(const Vector& vec) noexcept;
  /// Element-wise vector-value addition assignment operator.
  /// \param val Value to be added.
  /// \return Reference to the modified original vector.
  constexpr Vector& operator+=(T val) noexcept;
  /// Element-wise vector-vector substraction assignment operator.
  /// \param vec Vector to be substracted.
  /// \return Reference to the modified original vector.
  constexpr Vector& operator-=(const Vector& vec) noexcept;
  /// Element-wise vector-value substraction assignment operator.
  /// \param val Value to be substracted.
  /// \return Reference to the modified original vector.
  constexpr Vector& operator-=(T val) noexcept;
  /// Element-wise vector-vector multiplication assignment operator.
  /// \param vec Vector to be multiplied by.
  /// \return Reference to the modified original vector.
  constexpr Vector& operator*=(const Vector& vec) noexcept;
  /// Element-wise vector-value multiplication assignment operator.
  /// \tparam T2 Type of the given value.
  /// \param val Value to be multiplied by.
  /// \return Reference to the modified original vector.
  template <typename T2> constexpr Vector& operator*=(T2 val) noexcept;
  /// Element-wise vector-vector division assignment operator.
  /// \param vec Vector to be divided by.
  /// \return Reference to the modified original vector.
  constexpr Vector& operator/=(const Vector& vec) noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559);
  /// Element-wise vector-value division assignment operator.
  /// \param val Value to be divided by.
  /// \return Reference to the modified original vector.
  constexpr Vector& operator/=(T val) noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559);
  /// Element fetching operator given its index.
  /// \param index Element's index.
  /// \return Constant reference to the fetched element.
  constexpr const T& operator[](std::size_t index) const noexcept { return m_data[index]; }
  /// Element fetching operator given its index.
  /// \param index Element's index.
  /// \return Reference to the fetched element.
  constexpr T& operator[](std::size_t index) noexcept { return m_data[index]; }
  /// Vector equality comparison operator.
  /// Uses a near-equality check on floating types to take floating-point errors into account.
  /// \param vec Vector to be compared with.
  /// \return True if vectors are [nearly] equal, else otherwise.
  constexpr bool operator==(const Vector& vec) const noexcept;
  /// Vector inequality comparison operator.
  /// Uses a near-equality check on floating types to take floating-point errors into account.
  /// \param vec Vector to be compared with.
  /// \return True if vectors are different, else otherwise.
  constexpr bool operator!=(const Vector& vec) const noexcept { return !(*this == vec); }
  /// Output stream operator.
  /// \param stream Stream to output into.
  /// \param vec Vector to be output.
  friend std::ostream& operator<< <>(std::ostream& stream, const Vector& vec);

private:
  std::array<T, Size> m_data {};
};

/// Element-wise value-vector addition operator (of the form val + vec).
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
/// \param val Value to be added to the vectors's element.
/// \param vec Vector to be additioned.
/// \return Additioned vector.
template <typename T, std::size_t Size>
constexpr Vector<T, Size> operator+(T val, const Vector<T, Size>& vec) noexcept { return vec + val; }

/// Element-wise value-vector multiplication operator (of the form val * vec).
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
/// \param val Value to be multiplied by the vectors's element.
/// \param vec Vector to be multiplied.
/// \return Multiplied vector.
template <typename T, std::size_t Size>
constexpr Vector<T, Size> operator*(T val, const Vector<T, Size>& vec) noexcept { return vec * val; }

// Deduction guide, to automatically deduce the vector's type & size from the given arguments
template <typename T, typename... Args>
Vector(T&&, Args&&... args) -> Vector<std::decay_t<T>, sizeof...(args) + 1>;

// Aliases

template <typename T> using Vec2 = Vector<T, 2>;
template <typename T> using Vec3 = Vector<T, 3>;
template <typename T> using Vec4 = Vector<T, 4>;

using Vec2b = Vec2<uint8_t>;
using Vec3b = Vec3<uint8_t>;
using Vec4b = Vec4<uint8_t>;

using Vec2i = Vec2<int>;
using Vec3i = Vec3<int>;
using Vec4i = Vec4<int>;

using Vec2u = Vec2<uint32_t>;
using Vec3u = Vec3<uint32_t>;
using Vec4u = Vec4<uint32_t>;

using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;

using Vec2d = Vec2<double>;
using Vec3d = Vec3<double>;
using Vec4d = Vec4<double>;

namespace Axis {

constexpr Vec3f X(1.f, 0.f, 0.f);
constexpr Vec3f Y(0.f, 1.f, 0.f);
constexpr Vec3f Z(0.f, 0.f, 1.f);

} // namespace Axis

} // namespace Raz

/// Specialization of std::hash for Vector.
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
template <typename T, std::size_t Size>
struct std::hash<Raz::Vector<T, Size>> {
  /// Computes the hash of the given vector.
  /// \param vec Vector to compute the hash of.
  /// \return Vector's hash value.
  constexpr std::size_t operator()(const Raz::Vector<T, Size>& vec) const noexcept { return vec.hash(); }
};

/// Specialization of std::equal_to for Vector. This performs a strict equality check.
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
template <typename T, std::size_t Size>
struct std::equal_to<Raz::Vector<T, Size>> {
  /// Checks that the two given vectors are strictly equal to each other.
  /// \param vec1 First vector to be compared.
  /// \param vec2 Second vector to be compared.
  /// \return True if vectors are strictly equal to each other, false otherwise.
  constexpr bool operator()(const Raz::Vector<T, Size>& vec1, const Raz::Vector<T, Size>& vec2) const noexcept {
    return vec1.strictlyEquals(vec2);
  }
};

#include "RaZ/Math/Vector.inl"

#endif // RAZ_VECTOR_HPP
