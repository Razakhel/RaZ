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
Vector<T, Size>::Vector(std::initializer_list<T> list) noexcept {
  assert("Error: A Vector cannot be created with less/more values than specified." && Size == list.size());

  auto element = list.begin();

  for (std::size_t i = 0; i < list.size(); ++i, ++element)
    m_data[i] = *element;
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

  res[0] =   m_data[1] * vec[2] - m_data[2] * vec[1];
  res[1] = -(m_data[0] * vec[2] - m_data[2] * vec[0]);
  res[2] =   m_data[0] * vec[1] - m_data[1] * vec[0];

  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::normalize() const {
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
constexpr Vector<T, Size> Vector<T, Size>::operator/(const Vector& vec) const {
  Vector<T, Size> res = *this;
  res /= vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator/(T val) const {
  Vector<T, Size> res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t Size>
template <std::size_t H>
constexpr Vector<T, Size> Vector<T, Size>::operator*(const Matrix<T, Size, H>& mat) const noexcept {
  // This multiplication is made assuming the vector to be horizontal
  Vector<T, Size> res {};

  for (std::size_t widthIndex = 0; widthIndex < Size; ++widthIndex) {
    for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex)
      res[widthIndex] += m_data[heightIndex] * mat[heightIndex * Size + widthIndex];
  }

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
constexpr Vector<T, Size>& Vector<T, Size>::operator*=(T val) noexcept {
  for (T& elt : m_data)
    elt *= val;
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator/=(const Vector& vec) {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] /= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator/=(T val) {
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
    stream << "; " << vec[i];

  stream << " ]";

  return stream;
}

} // namespace Raz
