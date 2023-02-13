#pragma once

#ifndef RAZ_MATRIX_HPP
#define RAZ_MATRIX_HPP

#include <array>
#include <ostream>

namespace Raz {

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
  /// Constructs a matrix from the given values.
  /// \note All values must be of the same type as the matrix's inner one, and there must be exactly as many values as the matrix can hold (width * height).
  /// \tparam T2 Type of the first value. Must be the same as T.
  /// \tparam Args Types of the other values. Must be the same as T.
  /// \param val First value.
  /// \param args Other values.
  template <typename T2, typename... Args, typename = std::enable_if_t<std::is_same_v<T, std::decay_t<T2>>>>
  constexpr explicit Matrix(T2&& val, Args&&... args) noexcept;
  constexpr Matrix(const Matrix&) noexcept = default;
  constexpr Matrix(Matrix&&) noexcept = default;

  constexpr std::size_t getWidth() const noexcept { return W; }
  constexpr std::size_t getHeight() const noexcept { return H; }
  constexpr const std::array<T, W * H>& getData() const noexcept { return m_data; }
  constexpr std::array<T, W * H>& getData() noexcept { return m_data; }
  constexpr const T* getDataPtr() const noexcept { return m_data.data(); }
  constexpr T* getDataPtr() noexcept { return m_data.data(); }
  /// Gets an element given its width & height indices.
  /// \param widthIndex Element's width index.
  /// \param heightIndex Element's height index.
  /// \return Constant reference to the fetched element.
  constexpr const T& getElement(std::size_t widthIndex, std::size_t heightIndex) const noexcept { return m_data[widthIndex * H + heightIndex]; }
  /// Gets an element given its width & height indices.
  /// \param widthIndex Element's width index.
  /// \param heightIndex Element's height index.
  /// \return Reference to the fetched element.
  constexpr T& getElement(std::size_t widthIndex, std::size_t heightIndex) noexcept { return m_data[widthIndex * H + heightIndex]; }

  /// Creates an identity matrix; needs to be called with a square matrix type.
  /// \return Identity matrix.
  static constexpr Matrix identity() noexcept;
  /// Constructs a matrix from the given row vectors.
  /// \note All vectors must be of the same inner type as the matrix's, and must have a size equal to the matrix's width.
  /// \tparam Vecs Types of the vectors to construct the matrix with.
  /// \param vecs Row vectors to construct the matrix with.
  template <typename... Vecs>
  static constexpr Matrix fromRows(Vecs&&... vecs) noexcept;
  /// Constructs a matrix from the given column vectors.
  /// \note All vectors must be of the same inner type as the matrix's, and must have a size equal to the matrix's height.
  /// \tparam Vecs Types of the vectors to construct the matrix with.
  /// \param vecs Column vectors to construct the matrix with.
  template <typename... Vecs>
  static constexpr Matrix fromColumns(Vecs&&... vecs) noexcept;

  /// Transposed matrix computation.
  /// \return Transposed matrix.
  constexpr Matrix<T, H, W> transpose() const noexcept;
  /// Determinant computation.
  /// \return Matrix's determinant.
  constexpr T computeDeterminant() const noexcept;
  /// Inverse matrix computation.
  /// \return Matrix's inverse.
  constexpr Matrix inverse() const noexcept;
  /// Recovers the values in the row at the given index.
  /// \param rowIndex Index of the row to recover.
  /// \return Vector containing the row elements.
  constexpr Vector<T, W> recoverRow(std::size_t rowIndex) const noexcept;
  /// Recovers the values in the column at the given index.
  /// \param columnIndex Index of the column to recover.
  /// \return Vector containing the column elements.
  constexpr Vector<T, H> recoverColumn(std::size_t columnIndex) const noexcept;
  /// Checks for strict equality between the current matrix & the given one.
  /// \param mat Matrix to be compared with.
  /// \return True if matrices are strictly equal to each other, false otherwise.
  constexpr bool strictlyEquals(const Matrix& mat) const noexcept;
  /// Computes the unique hash of the current matrix.
  /// \param seed Value to use as a hash seed.
  /// \return Matrix's hash.
  constexpr std::size_t hash(std::size_t seed = 0) const noexcept;

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
  constexpr Matrix operator/(const Matrix& mat) const noexcept;
  /// Element-wise matrix-value division operator.
  /// \param val Value to be divided by.
  /// \return Result of the matrix divided by the value.
  constexpr Matrix operator/(T val) const noexcept;
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
  constexpr Matrix& operator/=(const Matrix& mat) noexcept;
  /// Element-wise matrix-value division assignment operator.
  /// \param val Value to be divided by.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator/=(T val) noexcept;
  /// Matrix-matrix multiplication assignment operator.
  /// \param mat Matrix to be multiplied by.
  /// \return Reference to the modified original matrix.
  constexpr Matrix& operator*=(const Matrix& mat) noexcept;
  /// Element fetching operator with a single index.
  /// \warning Matrices being in column-major layout, this must be used with caution. For example, with a 3x3 matrix, the indices are laid out as follows:
  ///
  ///     0 3 6
  ///     1 4 7
  ///     2 5 8
  /// For readability and ease of use, you may prefer using getElement() instead.
  /// \param index Element's index.
  /// \return Constant reference to the fetched element.
  constexpr const T& operator[](std::size_t index) const noexcept { return m_data[index]; }
  /// Element fetching operator with a single index.
  /// \warning Matrices being in a column-major layout, this must be used with caution. For example, with a 3x3 matrix, the indices are laid out as follows:
  ///
  ///     0 3 6
  ///     1 4 7
  ///     2 5 8
  /// For readability and ease of use, you may prefer using getElement() instead.
  /// \param index Element's index.
  /// \return Reference to the fetched element.
  constexpr T& operator[](std::size_t index) noexcept { return m_data[index]; }
  /// Matrix equality comparison operator.
  /// Uses a near-equality check on floating types to take floating-point errors into account.
  /// \param mat Matrix to be compared with.
  /// \return True if matrices are [nearly] equal, else otherwise.
  constexpr bool operator==(const Matrix& mat) const noexcept;
  /// Matrix inequality comparison operator.
  /// Uses a near-equality check on floating types to take floating-point errors into account.
  /// \param mat Matrix to be compared with.
  /// \return True if matrices are different, else otherwise.
  constexpr bool operator!=(const Matrix& mat) const noexcept { return !(*this == mat); }
  /// Output stream operator.
  /// \param stream Stream to output into.
  /// \param mat Matrix to be output.
  friend std::ostream& operator<< <>(std::ostream& stream, const Matrix& mat);

private:
  template <std::size_t WI, std::size_t HI, typename T2, typename... Args>
  constexpr void setValues(T2&& val, Args&&... args) noexcept;

  template <typename Vec, typename... Vecs>
  constexpr void setRows(Vec&& vec, Vecs&&... args) noexcept;

  template <typename Vec, typename... Vecs>
  constexpr void setColumns(Vec&& vec, Vecs&&... args) noexcept;

  std::array<T, W * H> m_data {};
};

/// Matrix-matrix multiplication operator.
/// \tparam W Width of the left-hand matrix.
/// \tparam H Height of the left-hand matrix & width of the resulting one.
/// \tparam WI Width of the right-hand matrix & height of the resulting one.
/// \tparam HI Height of the right-hand matrix.
/// \param mat1 Left-hand matrix.
/// \param mat2 Right-hand matrix.
/// \return Result of the multiplied matrices.
template <typename T, std::size_t W, std::size_t H, std::size_t WI, std::size_t HI>
constexpr Matrix<T, H, WI> operator*(const Matrix<T, WI, H>& mat1, const Matrix<T, WI, HI>& mat2) noexcept;

/// Matrix-vector multiplication operator (assumes the vector to be vertical).
/// \tparam T Type of the matrix's & vector's data.
/// \tparam W Width of the matrix & size of the input vector.
/// \tparam H Height of the matrix & size of the resulting vector.
/// \param mat Left-hand matrix.
/// \param vec Right-hand vector.
/// \return Result of the matrix-vector multiplication.
template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, H> operator*(const Matrix<T, W, H>& mat, const Vector<T, W>& vec) noexcept;

// Aliases

template <typename T> using Mat2 = Matrix<T, 2, 2>;
template <typename T> using Mat3 = Matrix<T, 3, 3>;
template <typename T> using Mat4 = Matrix<T, 4, 4>;

using Mat2b = Mat2<uint8_t>;
using Mat3b = Mat3<uint8_t>;
using Mat4b = Mat4<uint8_t>;

using Mat2i = Mat2<int>;
using Mat3i = Mat3<int>;
using Mat4i = Mat4<int>;

using Mat2u = Mat2<uint32_t>;
using Mat3u = Mat3<uint32_t>;
using Mat4u = Mat4<uint32_t>;

using Mat2f = Mat2<float>;
using Mat3f = Mat3<float>;
using Mat4f = Mat4<float>;

using Mat2d = Mat2<double>;
using Mat3d = Mat3<double>;
using Mat4d = Mat4<double>;

} // namespace Raz

/// Specialization of std::hash for Matrix.
/// \tparam T Type of the matrix's data.
/// \tparam W Matrix's width.
/// \tparam H Matrix's height.
template <typename T, std::size_t W, std::size_t H>
struct std::hash<Raz::Matrix<T, W, H>> {
  /// Computes the hash of the given matrix.
  /// \param mat Matrix to compute the hash of.
  /// \return Matrix's hash value.
  constexpr std::size_t operator()(const Raz::Matrix<T, W, H>& mat) const noexcept { return mat.hash(); }
};

/// Specialization of std::equal_to for Matrix. This performs a strict equality check.
/// \tparam T Type of the matrices' data.
/// \tparam W Matrices' width.
/// \tparam H Matrices' height.
template <typename T, std::size_t W, std::size_t H>
struct std::equal_to<Raz::Matrix<T, W, H>> {
  /// Checks that the two given matrices are strictly equal to each other.
  /// \param mat1 First matrix to be compared.
  /// \param mat2 Second matrix to be compared.
  /// \return True if matrices are strictly equal to each other, false otherwise.
  constexpr bool operator()(const Raz::Matrix<T, W, H>& mat1, const Raz::Matrix<T, W, H>& mat2) const noexcept {
    return mat1.strictlyEquals(mat2);
  }
};

/// Specialization of std::less for Matrix.
/// \tparam T Type of the matrices' data.
/// \tparam W Matrices' width.
/// \tparam H Matrices' height.
template <typename T, std::size_t W, std::size_t H>
struct std::less<Raz::Matrix<T, W, H>> {
  /// Checks that the first given matrix is strictly less than the other.
  /// \param mat1 First matrix to be compared.
  /// \param mat2 Second matrix to be compared.
  /// \return True if the first matrix is strictly less than the other, false otherwise.
  constexpr bool operator()(const Raz::Matrix<T, W, H>& mat1, const Raz::Matrix<T, W, H>& mat2) const noexcept {
    for (std::size_t i = 0; i < W * H; ++i) {
      if (mat1[i] == mat2[i])
        continue;

      return (mat1[i] < mat2[i]);
    }

    return false;
  }
};

#include "RaZ/Math/Matrix.inl"

#endif // RAZ_MATRIX_HPP
