#pragma once

#ifndef RAZ_MATRIX_HPP
#define RAZ_MATRIX_HPP

#include <array>

namespace Raz {

template <typename T, unsigned int W, unsigned int H>
class Matrix {
public:
  Matrix();
  Matrix(std::initializer_list<std::initializer_list<T>> list);

  std::size_t getWidth() const { return W; }
  std::size_t getHeight() const { return H; }
  const std::array<T, W * H>& getData() const { return m_data; }
  std::array<T, W * H>& getData() { return m_data; }

  Matrix operator+(Matrix mat);
  Matrix operator+(float val);
  Matrix operator-(Matrix mat);
  Matrix operator-(float val);
  Matrix operator*(Matrix mat);
  Matrix operator*(float val);
  Matrix operator/(Matrix mat);
  Matrix operator/(float val);
  Matrix& operator+=(const Matrix& mat);
  Matrix& operator+=(float val);
  Matrix& operator-=(const Matrix& mat);
  Matrix& operator-=(float val);
  Matrix& operator*=(const Matrix& mat);
  Matrix& operator*=(float val);
  Matrix& operator/=(const Matrix& mat);
  Matrix& operator/=(float val);
  T& operator()(unsigned int widthIndex, unsigned int heightIndex) { return m_data[heightIndex * W + widthIndex]; }
  const T& operator[](unsigned int index) const { return m_data[index]; }
  T& operator[](unsigned int index) { return m_data[index]; }

private:
  std::array<T, W * H> m_data;
};

using Mat3f = Matrix<float, 3, 3>;
using Mat4f = Matrix<float, 4, 4>;

} // namespace Raz

#include "RaZ/Math/Matrix.inl"

#endif // RAZ_MATRIX_HPP
