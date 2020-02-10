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

/// Matrix class, representing a mathematical matrix, with generic type and size.
/// \tparam T Type of the matrix's data.
/// \tparam W Matrix's width.
/// \tparam H Matrix's height.
template <typename T, std::size_t W, std::size_t H>
class Matrix {
public:
  constexpr Matrix() noexcept = default;
  constexpr explicit Matrix(const Matrix<T, W + 1, H + 1>& mat) noexcept;
  constexpr explicit Matrix(const Matrix<T, W - 1, H - 1>& mat) noexcept;
  template <typename... Args,
      typename = std::enable_if_t<sizeof...(Args) == W * H>, // There can't be more or less values than width * height
      typename = std::enable_if_t<(std::is_same_v<T, std::decay_t<Args>> && ...)>> // Given values must be of the same type
  explicit constexpr Matrix(Args&&... args) noexcept : m_data{ std::forward<Args>(args)... } {}
  Matrix(std::initializer_list<std::initializer_list<T>> list) noexcept;
  constexpr Matrix(const Matrix&) noexcept = default;
  constexpr Matrix(Matrix&&) noexcept = default;

  constexpr std::size_t getWidth() const noexcept { return W; }
  constexpr std::size_t getHeight() const noexcept { return H; }
  constexpr const std::array<T, W * H>& getData() const noexcept { return m_data; }
  constexpr std::array<T, W * H>& getData() noexcept { return m_data; }
  constexpr const T* getDataPtr() const noexcept { return m_data.data(); }
  constexpr T* getDataPtr() noexcept { return m_data.data(); }

  /// Identity matrix static creation; needs to be called with a square matrix type.
  /// \return Identity matrix.
  static constexpr Matrix identity() noexcept;
  /// Transposed matrix computation.
  /// \return Transposed matrix.
  constexpr Matrix<T, H, W> transpose() const noexcept;
  /// Determinant computation.
  /// \return Matrix's determinant.
  constexpr T computeDeterminant() const noexcept;
  /// Inverse matrix computation.
  /// \return Matrix's inverse.
  constexpr Matrix inverse() const;
  /// Recovers the values in the row at the given index.
  /// \param rowIndex Index of the row to recover.
  /// \return Vector containing the row elements.
  constexpr Vector<T, W> recoverRow(std::size_t rowIndex) const noexcept;
  /// Recovers the values in the column at the given index.
  /// \param columnIndex Index of the column to recover.
  /// \return Vector containing the column elements.
  constexpr Vector<T, H> recoverColumn(std::size_t columnIndex) const noexcept;

  /// Default copy assignment operator.
  /// \return Reference to the copied matrix.
  constexpr Matrix& operator=(const Matrix&) noexcept = default;
  /// Default move assignment operator.
  /// \return Reference to the moved matrix.
  constexpr Matrix& operator=(Matrix&&) noexcept = default;
  /// Element-wise matrix-matrix addition operator.
  /// \param mat Matrix to be added.
  /// \return Result of the summed matrices.
  constexpr Matrix operator+(const Matrix& mat) const noexcept;
  /// Element-wise matrix-value addition operator.
  /// \param val Value to be added.
  /// \return Result of the matrix summed with the value.
  constexpr Matrix operator+(T val) const noexcept;
  /// Element-wise matrix-matrix substraction operator.
  /// \param mat Matrix to be substracted.
  /// \return Result of the substracted matrices.
  constexpr Matrix operator-(const Matrix& mat) const noexcept;
  /// Element-wise matrix-value substraction operator.
  /// \param val Value to be substracted.
  /// \return Result of the matrix substracted by the value.
  constexpr Matrix operator-(T val) const noexcept;
  /// Element-wise matrix-matrix multiplication operator.
  /// \param mat Matrix to be multiplied by.
  /// \return Result of the multiplied matrices.
  constexpr Matrix operator%(const Matrix& mat) const noexcept;
  /// Element-wise matrix-value multiplication operator.
  /// \param val Value to be multiplied by.
  /// \return Result of the matrix multiplied by the value.
  constexpr Matrix operator*(T val) const noexcept;
  /// Element-wise matrix-matrix division operator.
  /// \param mat Matrix to be divided by.
  /// \return Result of the divided matrices.
  constexpr Matrix operator/(const Matrix& mat) const;
  /// Element-wise matrix-value division operator.
  /// \param val Value to be divided by.
  /// \return Result of the matrix divided by the value.
  constexpr Matrix operator/(T val) const;
  /// Matrix-vector multiplication operator (assumes the vector to be vertical).
  /// \param vec Vector to be multiplied by.
  /// \return Result of the matrix-vector multiplication.
  constexpr Vector<T, H> operator*(const Vector<T, H>& vec) const noexcept;
  /// Matrix-matrix multiplication operator.
  /// \tparam WI Input matrix's width.
  /// \tparam HI Input matrix's height.
  /// \param mat Matrix to be multiplied by.
  /// \return Result of the multiplied matrices.
  template <std::size_t WI, std::size_t HI> constexpr Matrix<T, H, WI> operator*(const Matrix<T, WI, HI>& mat) const noexcept;
  /// Element-wise matrix-matrix addition assignment operator.
  /// \param mat Matrix to be added.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator+=(const Matrix& mat) noexcept;
  /// Element-wise matrix-value addition assignment operator.
  /// \param val Value to be added.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator+=(T val) noexcept;
  /// Element-wise matrix-matrix substraction assignment operator.
  /// \param mat Matrix to be substracted.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator-=(const Matrix& mat) noexcept;
  /// Element-wise matrix-value substraction assignment operator.
  /// \param val Value to be substracted.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator-=(T val) noexcept;
  /// Element-wise matrix-matrix multiplication assignment operator.
  /// \param mat Matrix to be multiplied by.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator%=(const Matrix& mat) noexcept;
  /// Element-wise matrix-value multiplication assignment operator.
  /// \param val Value to be multiplied by.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator*=(T val) noexcept;
  /// Element-wise matrix-matrix division assignment operator.
  /// \param mat Matrix to be divided by.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator/=(const Matrix& mat);
  /// Element-wise matrix-value divions assignment operator.
  /// \param val Value to be divided by.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator/=(T val);
  /// Element fetching operator given width & height indices.
  /// \param widthIndex Width index.
  /// \param heightIndex Height index.
  /// \return Reference to the fetched element.
  constexpr T& operator()(std::size_t widthIndex, std::size_t heightIndex) noexcept { return m_data[heightIndex * W + widthIndex]; }
  /// Element fetching operator with a single index.
  /// \param index Element's index.
  /// \return Constant reference to the fetched element.
  constexpr const T& operator[](std::size_t index) const noexcept { return m_data[index]; }
  /// Element fetching operator with a single index.
  /// \param index Element's index.
  /// \return Reference to the fetched element.
  constexpr T& operator[](std::size_t index) noexcept { return m_data[index]; }
  /// Matrix equality comparison operator.
  /// Uses a near-equality check on floating types to take floating-point errors into account.
  /// \param mat Matrix to be compared with.
  /// \return True if matrices are [nearly] equal, else otherwise.
  constexpr bool operator==(const Matrix& mat) const noexcept;
  /// Matrix unequality comparison operator.
  /// Uses a near-equality check on floating types to take floating-point errors into account.
  /// \param mat Matrix to be compared with.
  /// \return True if matrices are different, else otherwise.
  constexpr bool operator!=(const Matrix& mat) const noexcept { return !(*this == mat); }
  /// Output stream operator.
  /// \param stream Stream to output into.
  /// \param mat Matrix to be output.
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
