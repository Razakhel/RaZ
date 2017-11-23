#pragma once

#ifndef RAZ_VECTOR_HPP
#define RAZ_VECTOR_HPP

#include <array>
#include <initializer_list>

namespace Raz {

template <typename T, std::size_t Size>
class Vector {
public:
  Vector() = default;
  Vector(T val);
  Vector(std::initializer_list<T> list);

  std::size_t getSize() const { return Size; }
  const std::array<T, Size>& getData() const { return m_data; }
  std::array<T, Size>& getData() { return m_data; }
  const T* getDataPtr() const { return m_data.data(); }
  T* getDataPtr() { return m_data.data(); }

  T dot(const Vector& vec) const;
  Vector cross(const Vector& vec) const;
  Vector normalize() const;
  std::size_t hash(std::size_t seed) const;

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
  std::size_t operator()(const Vector& vec) const { return hash(0); }
  bool operator==(const Vector& vec) const { return (m_data[0] == vec.getData()[0])
                                                 && (m_data[1] == vec.getData()[1])
                                                 && (m_data[2] == vec.getData()[2]); }

private:
  std::array<T, Size> m_data;
};

using Vec2b = Vector<uint8_t, 2>;
using Vec3b = Vector<uint8_t, 3>;
using Vec4b = Vector<uint8_t, 4>;

using Vec2i = Vector<int, 2>;
using Vec3i = Vector<int, 3>;
using Vec4i = Vector<int, 4>;

using Vec2ul = Vector<std::size_t, 2>;
using Vec3ul = Vector<std::size_t, 3>;
using Vec4ul = Vector<std::size_t, 4>;

using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;
using Vec4f = Vector<float, 4>;

using Vec2d = Vector<double, 2>;
using Vec3d = Vector<double, 3>;
using Vec4d = Vector<double, 4>;

} // namespace Raz

#include "RaZ/Math/Vector.inl"

#endif // RAZ_VECTOR_HPP
