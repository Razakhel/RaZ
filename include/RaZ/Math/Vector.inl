#include <cmath>
#include <cassert>

namespace Raz {

template <typename T, std::size_t Size>
Vector<T, Size>::Vector(const Vector<T, Size - 1>& vec, T val) {
  for (std::size_t i = 0; i < vec.getSize(); ++i)
    m_data[i] = vec[i];
  m_data.back() = val;
}

template <typename T, std::size_t Size>
Vector<T, Size>::Vector(T val) {
  for (T& elt : m_data)
    elt = val;
}

template <typename T, std::size_t Size>
Vector<T, Size>::Vector(std::initializer_list<T> list) {
  assert(("Error: Vector must not be created with less/more values than specified.", Size == list.size()));

  auto element = list.begin();

  for (std::size_t i = 0; i < list.size(); ++i, ++element)
    m_data[i] = *element;
}

template <typename T, std::size_t Size>
T Vector<T, Size>::dot(const Vector<T, Size>& vec) const {
  float res = 0.f;
  for (std::size_t i = 0; i < Size; ++i)
    res += m_data[i] * vec[i];
  return res;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::cross(const Vector<T, Size>& vec) const {
  static_assert(Size == 3, "Error: Both vectors must be 3 dimensional.");

  Vector<T, Size> res;

  res[0] = m_data[1] * vec.getData()[2] - m_data[2] * vec.getData()[1];
  res[1] = -(m_data[0] * vec.getData()[2] - m_data[2] * vec.getData()[0]);
  res[2] = m_data[0] * vec.getData()[1] - m_data[1] * vec.getData()[0];

  return res;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::normalize() const {
  Vector<T, Size> res = *this;
  res /= std::sqrt(dot(*this));
  return res;
}

template <typename T, std::size_t Size>
float Vector<T, Size>::computeLength() const {
  float length = 0;

  for (const auto& val : m_data)
    length += val * val;

  return std::sqrt(length);
}

template <typename T, std::size_t Size>
std::size_t Vector<T, Size>::hash(std::size_t seed) const {
  for (const auto& elt : m_data)
    seed ^= std::hash<T>()(elt) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

  return seed;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator+(Vector<T, Size> vec) const {
  vec += *this;
  return vec;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator+(float val) const {
  Vector<T, Size> res = *this;
  res += val;
  return res;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator-(Vector<T, Size> vec) const {
  vec -= *this;
  return vec;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator-(float val) const {
  Vector<T, Size> res = *this;
  res -= val;
  return res;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator*(Vector<T, Size> vec) const {
  vec *= *this;
  return vec;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator*(float val) const {
  Vector<T, Size> res = *this;
  res *= val;
  return res;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator/(Vector<T, Size> vec) const {
  vec /= *this;
  return vec;
}

template <typename T, std::size_t Size>
Vector<T, Size> Vector<T, Size>::operator/(float val) const {
  Vector<T, Size> res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t Size>
template <std::size_t H>
Vector<T, Size> Vector<T, Size>::operator*(const Matrix<T, Size, H>& mat) const {
  Vector<T, Size> res;

  for (std::size_t j = 0; j < Size; ++j) {
    for (std::size_t i = 0; i < H; ++i)
      res[j] = m_data[j] + mat[i * Size + j];
  }

  return res;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator+=(const Vector<T, Size>& vec) {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] += vec[i];
  return *this;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator+=(float val) {
  for (T& elt : m_data)
    elt += val;
  return *this;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator-=(const Vector<T, Size>& vec) {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] -= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator-=(float val) {
  for (T& elt : m_data)
    elt -= val;
  return *this;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator*=(const Vector<T, Size>& vec) {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] *= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator*=(float val) {
  for (T& elt : m_data)
    elt *= val;
  return *this;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator/=(const Vector<T, Size>& vec) {
  for (std::size_t i = 0; i < Size; ++i)
    m_data[i] /= vec[i];
  return *this;
}

template <typename T, std::size_t Size>
Vector<T, Size>& Vector<T, Size>::operator/=(float val) {
  for (T& elt : m_data)
    elt /= val;
  return *this;
}

} // namespace Raz
