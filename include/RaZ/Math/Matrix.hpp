#pragma once

#ifndef RAZ_MATRIX_HPP
#define RAZ_MATRIX_HPP

#include <array>

namespace Raz {

template <typename T, std::size_t W, std::size_t H>
class Matrix {
public:
  Matrix() = default;
  Matrix(const Matrix&) = default;
  Matrix(Matrix&&) noexcept = default;
  explicit Matrix(const Matrix<T, W + 1, H + 1>& mat);
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

  Matrix& operator=(const Matrix& mat) = default;
  Matrix operator+(const Matrix& mat) const;
  Matrix operator+(float val) const;
  Matrix operator-(const Matrix& mat) const;
  Matrix operator-(float val) const;
  Matrix operator%(const Matrix& mat) const;
  Matrix operator*(float val) const;
  Matrix operator/(const Matrix& mat) const;
  Matrix operator/(float val) const;
  //template <std::size_t Size> Vector<T, Size> operator*(const Vector<T, Size>& vec) const;
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

private:
  std::array<T, W * H> m_data {};
};

template <typename T> using Mat2 = Matrix<T, 2, 2>;
using Mat2f = Mat2<float>;
template <typename T> using Mat3 = Matrix<T, 3, 3>;
using Mat3f = Mat3<float>;
template <typename T> using Mat4 = Matrix<T, 4, 4>;
using Mat4f = Mat4<float>;

} // namespace Raz

#include "RaZ/Math/Matrix.inl"

#endif // RAZ_MATRIX_HPP
