#pragma once

#ifndef RAZ_VECTOR_HPP
#define RAZ_VECTOR_HPP

#include <array>
#include <initializer_list>

namespace Raz {

template <typename T, unsigned int Size>
class Vector {
public:
  Vector() = default;
  Vector(std::initializer_list<T> list);

  const std::array<T, Size>& getData() const { return m_data; }
  std::array<T, Size>& getData() { return m_data; }

  T dot(const Vector& vec) const;
  Vector cross(const Vector& vec) const;
  Vector normalize() const;

  Vector operator+(Vector vec);
  Vector operator+(float val);
  Vector operator-(Vector vec);
  Vector operator-(float val);
  Vector operator*(Vector vec);
  Vector operator*(float val);
  Vector operator/(Vector vec);
  Vector operator/(float val);
  Vector& operator+=(const Vector& vec);
  Vector& operator+=(float val);
  Vector& operator-=(const Vector& vec);
  Vector& operator-=(float val);
  Vector& operator*=(const Vector& vec);
  Vector& operator*=(float val);
  Vector& operator/=(const Vector& vec);
  Vector& operator/=(float val);
  Vector& operator=(const Vector& vec);
  Vector& operator=(float val);
  const T& operator[](std::size_t index) const { return m_data[index]; }
  T& operator[](std::size_t index) { return m_data[index]; }

private:
  std::array<T, Size> m_data;
};

using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;
using Vec4f = Vector<float, 4>;

} // namespace Raz

#include "RaZ/Math/Vector.inl"

#endif // RAZ_VECTOR_HPP
