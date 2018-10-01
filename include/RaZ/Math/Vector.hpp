#pragma once

#ifndef RAZ_VECTOR_HPP
#define RAZ_VECTOR_HPP

#include <array>
#include <cmath>
#include <iostream>
#include <initializer_list>

namespace Raz {

// Forward declaration of Matrix, to allow its usage into functions
template <typename T, std::size_t W, std::size_t H>
class Matrix;

template <typename T, std::size_t Size>
class Vector;

template <typename T, std::size_t Size>
std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vec);

template <typename T, std::size_t Size>
class Vector {
public:
  Vector() = default;
  Vector(const Vector&) = default;
  Vector(Vector&&) noexcept = default;
  Vector(const Vector<T, Size - 1>& vec, T val);
  explicit Vector(T val) noexcept;
  Vector(std::initializer_list<T> list);

  std::size_t getSize() const { return Size; }
  const std::array<T, Size>& getData() const { return m_data; }
  std::array<T, Size>& getData() { return m_data; }
  const T* getDataPtr() const { return m_data.data(); }
  T* getDataPtr() { return m_data.data(); }

  T dot(const Vector& vec) const;
  Vector cross(const Vector& vec) const;
  Vector reflect(const Vector& normal) const { return (*this - normal * dot(normal) * 2); }
  Vector normalize() const;
  float computeLength() const { return std::sqrt(computeSquaredLength()); }
  float computeSquaredLength() const { return dot(*this); }
  std::size_t hash(std::size_t seed) const;

  Vector& operator=(const Vector&) = default;
  Vector& operator=(Vector&&) noexcept = default;
  Vector operator-() const { return (*this * -1); }
  Vector operator+(const Vector& vec) const;
  Vector operator+(float val) const;
  Vector operator-(const Vector& vec) const;
  Vector operator-(float val) const;
  Vector operator*(const Vector& vec) const;
  Vector operator*(float val) const;
  Vector operator/(const Vector& vec) const;
  Vector operator/(float val) const;
  template <std::size_t H> Vector operator*(const Matrix<T, Size, H>& mat) const;
  Vector& operator+=(const Vector& vec);
  Vector& operator+=(float val);
  Vector& operator-=(const Vector& vec);
  Vector& operator-=(float val);
  Vector& operator*=(const Vector& vec);
  Vector& operator*=(float val);
  Vector& operator/=(const Vector& vec);
  Vector& operator/=(float val);
  const T& operator[](std::size_t index) const { return m_data[index]; }
  T& operator[](std::size_t index) { return m_data[index]; }
  std::size_t operator()(const Vector&) const { return hash(0); }
  bool operator==(const Vector& vec) const;
  friend std::ostream& operator<< <>(std::ostream& stream, const Vector& vec);

private:
  std::array<T, Size> m_data {};
};

template <typename T> using Vec2 = Vector<T, 2>;
template <typename T> using Vec3 = Vector<T, 3>;
template <typename T> using Vec4 = Vector<T, 4>;

using Vec2b = Vec2<uint8_t>;
using Vec3b = Vec3<uint8_t>;
using Vec4b = Vec4<uint8_t>;

using Vec2i = Vec2<int>;
using Vec3i = Vec3<int>;
using Vec4i = Vec4<int>;

using Vec2ul = Vec2<uint32_t>;
using Vec3ul = Vec3<uint32_t>;
using Vec4ul = Vec4<uint32_t>;

using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;

using Vec2d = Vec2<double>;
using Vec3d = Vec3<double>;
using Vec4d = Vec4<double>;

namespace Axis {

const Vec3f X({ 1.f, 0.f, 0.f });
const Vec3f Y({ 0.f, 1.f, 0.f });
const Vec3f Z({ 0.f, 0.f, 1.f });

}

} // namespace Raz

#include "RaZ/Math/Vector.inl"

#endif // RAZ_VECTOR_HPP
