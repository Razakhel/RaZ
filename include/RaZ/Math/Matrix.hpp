#pragma once

#ifndef RAZ_MATRIX_HPP
#define RAZ_MATRIX_HPP

#include <array>
#include <iostream>
#include <initializer_list>

namespace Raz {

// Forward declaration of Vector, to allow its usage into functions
template <typename T, std::size_t Size>
class Vector;

template <typename T, std::size_t W, std::size_t H>
class Matrix;

template <typename T, std::size_t W, std::size_t H>
std::ostream& operator<<(std::ostream& stream, const Matrix<T, W, H>& mat);

template <typename T, std::size_t W, std::size_t H>
class Matrix {
public:
  Matrix() = default;
  Matrix(const Matrix&) = default;
  Matrix(Matrix&&) noexcept = default;
  explicit Matrix(const Matrix<T, W + 1, H + 1>& mat);
  explicit Matrix(const Matrix<T, W - 1, H - 1>& mat);
  Matrix(std::initializer_list<std::initializer_list<T>> list);

  std::size_t getWidth() const { return W; }
  std::size_t getHeight() const { return H; }
  const std::array<T, W * H>& getData() const { return m_data; }
  std::array<T, W * H>& getData() { return m_data; }
  const T* getDataPtr() const { return m_data.data(); }
  T* getDataPtr() { return m_data.data(); }

  static Matrix identity();
  Matrix<T, H, W> transpose() const;
  float computeDeterminant() const;
  Matrix inverse() const;

  Matrix& operator=(const Matrix&) = default;
  Matrix& operator=(Matrix&&) noexcept = default;
  Matrix operator+(const Matrix& mat) const;
  Matrix operator+(float val) const;
  Matrix operator-(const Matrix& mat) const;
  Matrix operator-(float val) const;
  Matrix operator%(const Matrix& mat) const;
  Matrix operator*(float val) const;
  Matrix operator/(const Matrix& mat) const;
  Matrix operator/(float val) const;
  Vector<T, H> operator*(const Vector<T, H>& vec) const;
  template <std::size_t WI, std::size_t HI> Matrix<T, H, WI> operator*(const Matrix<T, WI, HI>& mat) const;
  Matrix& operator+=(const Matrix& mat);
  Matrix& operator+=(float val);
  Matrix& operator-=(const Matrix& mat);
  Matrix& operator-=(float val);
  Matrix& operator%=(const Matrix& mat);
  Matrix& operator*=(float val);
  Matrix& operator/=(const Matrix& mat);
  Matrix& operator/=(float val);
  T& operator()(std::size_t widthIndex, std::size_t heightIndex) { return m_data[heightIndex * W + widthIndex]; }
  const T& operator[](std::size_t index) const { return m_data[index]; }
  T& operator[](std::size_t index) { return m_data[index]; }
  bool operator==(const Matrix& mat) const { return std::equal(m_data.cbegin(), m_data.cend(), mat.getData().cbegin()); }
  friend std::ostream& operator<< <>(std::ostream& stream, const Matrix& mat);

private:
  std::array<T, W * H> m_data {};
};

template <typename T> using Mat2 = Matrix<T, 2, 2>;
template <typename T> using Mat3 = Matrix<T, 3, 3>;
template <typename T> using Mat4 = Matrix<T, 4, 4>;

using Mat2b = Mat2<uint8_t>;
using Mat3b = Mat3<uint8_t>;
using Mat4b = Mat4<uint8_t>;

using Mat2i = Mat2<int>;
using Mat3i = Mat3<int>;
using Mat4i = Mat4<int>;

using Mat2ul = Mat2<unsigned long>;
using Mat3ul = Mat3<unsigned long>;
using Mat4ul = Mat4<unsigned long>;

using Mat2f = Mat2<float>;
using Mat3f = Mat3<float>;
using Mat4f = Mat4<float>;

using Mat2d = Mat2<double>;
using Mat3d = Mat3<double>;
using Mat4d = Mat4<double>;

} // namespace Raz

#include "RaZ/Math/Matrix.inl"

#endif // RAZ_MATRIX_HPP
