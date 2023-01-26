#include "RaZ/Utils/FloatUtils.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace Raz {

template <typename T, std::size_t Size>
constexpr Vector<T, Size>::Vector(const Vector<T, Size + 1>& vec) noexcept {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] = vec[i];
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>::Vector(const Vector<T, Size - 1>& vec, T val) noexcept {
  for (std::size_t i = 0; i < Size - 1; ++i)
    m_data[i] = vec[i];
  m_data.back() = val;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>::Vector(T val) noexcept {
  for (T& elt : m_data)
    elt = val;
}

template <typename T, std::size_t Size>
constexpr const T& Vector<T, Size>::x() const noexcept {
  static_assert(Size >= 1, "Error: Getting the X component requires the vector to be of size 1 or more.");

  return m_data[0];
}

template <typename T, std::size_t Size>
constexpr T& Vector<T, Size>::x() noexcept {
  return const_cast<T&>(static_cast<const Vector<T, Size>*>(this)->x());
}

template <typename T, std::size_t Size>
constexpr const T& Vector<T, Size>::y() const noexcept {
  static_assert(Size >= 2, "Error: Getting the Y component requires the vector to be of size 2 or more.");

  return m_data[1];
}

template <typename T, std::size_t Size>
constexpr T& Vector<T, Size>::y() noexcept {
  return const_cast<T&>(static_cast<const Vector<T, Size>*>(this)->y());
}

template <typename T, std::size_t Size>
constexpr const T& Vector<T, Size>::z() const noexcept {
  static_assert(Size >= 3, "Error: Getting the Z component requires the vector to be of size 3 or more.");

  return m_data[2];
}

template <typename T, std::size_t Size>
constexpr T& Vector<T, Size>::z() noexcept {
  return const_cast<T&>(static_cast<const Vector<T, Size>*>(this)->z());
}

template <typename T, std::size_t Size>
constexpr const T& Vector<T, Size>::w() const noexcept {
  static_assert(Size >= 4, "Error: Getting the W component requires the vector to be of size 4 or more.");

  return m_data[3];
}

template <typename T, std::size_t Size>
constexpr T& Vector<T, Size>::w() noexcept {
  return const_cast<T&>(static_cast<const Vector<T, Size>*>(this)->w());
}

template <typename T, std::size_t Size>
constexpr T Vector<T, Size>::dot(const Vector& vec) const noexcept {
  T res {};
  for (std::size_t i = 0; i < Size; ++i)
    res += m_data[i] * vec[i];
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::cross(const Vector& vec) const noexcept {
  static_assert(Size == 3, "Error: Both vectors must be 3 dimensional to compute a cross product.");

  Vector<T, Size> res;

  res.m_data[0] =   m_data[1] * vec.m_data[2] - m_data[2] * vec.m_data[1];
  res.m_data[1] = -(m_data[0] * vec.m_data[2] - m_data[2] * vec.m_data[0]);
  res.m_data[2] =   m_data[0] * vec.m_data[1] - m_data[1] * vec.m_data[0];

  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::normalize() const noexcept {
  Vector<T, Size> res = *this;
  res /= computeLength();
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::lerp(const Vector& vec, float coeff) const noexcept {
  assert("Error: The interpolation coefficient must be between 0 & 1." && (coeff >= 0 && coeff <= 1));

  return *this + (vec - *this) * coeff;
}

template <typename T, std::size_t Size>
constexpr bool Vector<T, Size>::strictlyEquals(const Vector& vec) const noexcept {
  return std::equal(m_data.cbegin(), m_data.cend(), vec.getData().cbegin());
}

template <typename T, std::size_t Size>
constexpr std::size_t Vector<T, Size>::hash(std::size_t seed) const noexcept {
  std::hash<T> hasher {};

  for (const T& elt : m_data)
    seed ^= hasher(elt) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);

  return seed;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator+(const Vector& vec) const noexcept {
  Vector<T, Size> res = *this;
  res += vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator+(T val) const noexcept {
  Vector<T, Size> res = *this;
  res += val;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator-(const Vector& vec) const noexcept {
  Vector<T, Size> res = *this;
  res -= vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator-(T val) const noexcept {
  Vector<T, Size> res = *this;
  res -= val;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator*(const Vector& vec) const noexcept {
  Vector<T, Size> res = *this;
  res *= vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator*(T val) const noexcept {
  Vector<T, Size> res = *this;
  res *= val;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator/(const Vector& vec) const noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  Vector<T, Size> res = *this;
  res /= vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator/(T val) const noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  Vector<T, Size> res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator+=(const Vector& vec) noexcept {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] += vec[i];
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator+=(T val) noexcept {
  for (T& elt : m_data)
    elt += val;
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator-=(const Vector& vec) noexcept {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] -= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator-=(T val) noexcept {
  for (T& elt : m_data)
    elt -= val;
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator*=(const Vector& vec) noexcept {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] *= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
template <typename ValT>
constexpr Vector<T, Size>& Vector<T, Size>::operator*=(ValT val) noexcept {
  for (T& elt : m_data)
    elt *= static_cast<T>(val);
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator/=(const Vector& vec) noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] /= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator/=(T val) noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  for (T& elt : m_data)
    elt /= val;
  return *this;
}

template <typename T, std::size_t Size>
constexpr bool Vector<T, Size>::operator==(const Vector<T, Size>& vec) const noexcept {
  if constexpr (std::is_floating_point_v<T>)
    return FloatUtils::areNearlyEqual(*this, vec);
  else
    return strictlyEquals(vec);
}

template <typename T, std::size_t Size>
std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vec) {
  stream << "[ " << vec[0];

  for (std::size_t i = 1; i < Size; ++i)
    stream << ", " << vec[i];

  stream << " ]";

  return stream;
}

template <typename T, std::size_t Size, std::size_t W>
constexpr Vector<T, W> operator*(const Vector<T, Size>& vec, const Matrix<T, W, Size>& mat) noexcept {
  // This multiplication is made assuming the vector to be horizontal
  Vector<T, W> res;

  for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
    const std::size_t finalWidthIndex = widthIndex * Size;

    for (std::size_t heightIndex = 0; heightIndex < Size; ++heightIndex)
      res[widthIndex] += vec[heightIndex] * mat[finalWidthIndex + heightIndex];
  }

  return res;
}

} // namespace Raz
