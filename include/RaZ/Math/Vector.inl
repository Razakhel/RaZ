#include "RaZ/Utils/FloatUtils.hpp"

#include <algorithm>
#include <cassert>

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
  return const_cast<T&>(static_cast<const Vector*>(this)->x());
}

template <typename T, std::size_t Size>
constexpr const T& Vector<T, Size>::y() const noexcept {
  static_assert(Size >= 2, "Error: Getting the Y component requires the vector to be of size 2 or more.");
  return m_data[1];
}

template <typename T, std::size_t Size>
constexpr T& Vector<T, Size>::y() noexcept {
  return const_cast<T&>(static_cast<const Vector*>(this)->y());
}

template <typename T, std::size_t Size>
constexpr const T& Vector<T, Size>::z() const noexcept {
  static_assert(Size >= 3, "Error: Getting the Z component requires the vector to be of size 3 or more.");
  return m_data[2];
}

template <typename T, std::size_t Size>
constexpr T& Vector<T, Size>::z() noexcept {
  return const_cast<T&>(static_cast<const Vector*>(this)->z());
}

template <typename T, std::size_t Size>
constexpr const T& Vector<T, Size>::w() const noexcept {
  static_assert(Size >= 4, "Error: Getting the W component requires the vector to be of size 4 or more.");
  return m_data[3];
}

template <typename T, std::size_t Size>
constexpr T& Vector<T, Size>::w() noexcept {
  return const_cast<T&>(static_cast<const Vector*>(this)->w());
}

template <typename T, std::size_t Size>
template <typename DotT>
constexpr DotT Vector<T, Size>::dot(const Vector& vec) const noexcept {
  DotT res {};
  for (std::size_t i = 0; i < Size; ++i)
    res += static_cast<DotT>(m_data[i]) * static_cast<DotT>(vec[i]);
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::cross(const Vector& vec) const noexcept {
  static_assert(Size == 3, "Error: Both vectors must be 3 dimensional to compute a cross product.");
  static_assert(std::is_signed_v<T>, "Error: The cross product can only be computed with vectors of a signed type.");

  Vector<T, Size> res;

  res.m_data[0] =   m_data[1] * vec.m_data[2] - m_data[2] * vec.m_data[1];
  res.m_data[1] = -(m_data[0] * vec.m_data[2] - m_data[2] * vec.m_data[0]);
  res.m_data[2] =   m_data[0] * vec.m_data[1] - m_data[1] * vec.m_data[0];

  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::reflect(const Vector& normal) const noexcept {
  static_assert(std::is_signed_v<T>, "Error: The reflected vector can only be computed with vectors of a signed type.");

  return (*this - normal * static_cast<T>(dot(normal)) * 2);
}

template <typename T, std::size_t Size>
template <typename NormedT>
constexpr Vector<NormedT, Size> Vector<T, Size>::normalize() const noexcept {
  static_assert(std::is_floating_point_v<NormedT>, "Error: The normalized vector's type must be floating-point.");

  const NormedT sqLength = computeSquaredLength<NormedT>();
  Vector<NormedT, Size> res(*this);

  if (sqLength != static_cast<NormedT>(0))
    res /= std::sqrt(sqLength);

  return res;
}

template <typename T, std::size_t Size>
template <typename LerpT, typename CoeffT>
constexpr Vector<LerpT, Size> Vector<T, Size>::lerp(const Vector& vec, CoeffT coeff) const noexcept {
  static_assert(std::is_floating_point_v<CoeffT>, "Error: The linear interpolation's coefficient type must be floating-point.");
  assert("Error: The interpolation coefficient must be between 0 & 1." && (coeff >= 0 && coeff <= 1));

  const Vector<CoeffT, Size> convertedThis(*this);
  const Vector<CoeffT, Size> lerpVec = convertedThis + (Vector<CoeffT, Size>(vec) - convertedThis) * coeff;
  return Vector<LerpT, Size>(lerpVec);
}

template <typename T, std::size_t Size>
constexpr bool Vector<T, Size>::strictlyEquals(const Vector& vec) const noexcept {
  return std::equal(m_data.cbegin(), m_data.cend(), vec.m_data.cbegin());
}

template <typename T, std::size_t Size>
constexpr std::size_t Vector<T, Size>::hash(std::size_t seed) const noexcept {
  std::hash<T> hasher {};

  for (const T& elt : m_data)
    seed ^= hasher(elt) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);

  return seed;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator-() const noexcept {
  Vector res;
  for (std::size_t i = 0; i < Size; ++i)
    res.m_data[i] = static_cast<T>(-m_data[i]);
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator+(const Vector& vec) const noexcept {
  Vector res = *this;
  res += vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator+(T val) const noexcept {
  Vector res = *this;
  res += val;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator-(const Vector& vec) const noexcept {
  Vector res = *this;
  res -= vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator-(T val) const noexcept {
  Vector res = *this;
  res -= val;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator*(const Vector& vec) const noexcept {
  Vector res = *this;
  res *= vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator*(T val) const noexcept {
  Vector res = *this;
  res *= val;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator/(const Vector& vec) const noexcept {
  Vector res = *this;
  res /= vec;
  return res;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size> Vector<T, Size>::operator/(T val) const noexcept {
  Vector res = *this;
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
constexpr Vector<T, Size>& Vector<T, Size>::operator/=(const Vector& vec) noexcept {
  if constexpr (std::is_integral_v<T>)
    assert("Error: Integer vector division by 0 is undefined." && (std::find(vec.m_data.cbegin(), vec.m_data.cend(), 0) == vec.m_data.cend()));

  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] /= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
constexpr Vector<T, Size>& Vector<T, Size>::operator/=(T val) noexcept {
  if constexpr (std::is_integral_v<T>)
    assert("Error: Integer vector division by 0 is undefined." && (val != 0));

  for (T& elt : m_data)
    elt /= val;
  return *this;
}

template <typename T, std::size_t Size>
constexpr bool Vector<T, Size>::operator==(const Vector& vec) const noexcept {
  if constexpr (std::is_floating_point_v<T>)
    return FloatUtils::areNearlyEqual(*this, vec);
  else
    return strictlyEquals(vec);
}

template <typename T, std::size_t Size>
template <typename T2>
constexpr Vector<T, Size>::operator Vector<T2, Size>() const noexcept {
  Vector<T2, Size> res;
  for (std::size_t i = 0; i < Size; ++i)
    res[i] = static_cast<T2>(m_data[i]);
  return res;
}

template <typename T, std::size_t Size>
std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vec) {
  using PrintT = std::conditional_t<std::is_same_v<T, uint8_t>, int, T>;

  stream << "[ " << static_cast<PrintT>(vec[0]);

  for (std::size_t i = 1; i < Size; ++i)
    stream << ", " << static_cast<PrintT>(vec[i]);

  stream << " ]";

  return stream;
}

} // namespace Raz
