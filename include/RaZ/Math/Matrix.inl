#include "RaZ/Utils/FloatUtils.hpp"

#include <algorithm>
#include <cassert>

namespace Raz {

namespace Details {

template <typename T>
constexpr T computeMatrixDeterminant(const Mat2<T>& mat) noexcept {
  return (mat[0] * mat[3]) - (mat[2] * mat[1]);
}

template <typename T>
constexpr T computeMatrixDeterminant(const Mat3<T>& mat) noexcept {
  const Mat2<T> leftMatrix(mat[4], mat[5],
                           mat[7], mat[8]);

  const Mat2<T> centerMatrix(mat[3], mat[5],
                             mat[6], mat[8]);

  const Mat2<T> rightMatrix(mat[3], mat[4],
                            mat[6], mat[7]);

  return computeMatrixDeterminant(leftMatrix) * mat[0]
       - computeMatrixDeterminant(centerMatrix) * mat[1]
       + computeMatrixDeterminant(rightMatrix) * mat[2];
}

template <typename T>
constexpr T computeMatrixDeterminant(const Mat4<T>& mat) noexcept {
  const Mat3<T> leftMatrix(mat[5],  mat[6],  mat[7],
                           mat[9],  mat[10], mat[11],
                           mat[13], mat[14], mat[15]);

  const Mat3<T> centerLeftMatrix(mat[4],  mat[6],  mat[7],
                                 mat[8],  mat[10], mat[11],
                                 mat[12], mat[14], mat[15]);

  const Mat3<T> centerRightMatrix(mat[4],  mat[5],  mat[7],
                                  mat[8],  mat[9],  mat[11],
                                  mat[12], mat[13], mat[15]);

  const Mat3<T> rightMatrix(mat[4],  mat[5],  mat[6],
                            mat[8],  mat[9],  mat[10],
                            mat[12], mat[13], mat[14]);

  return computeMatrixDeterminant(leftMatrix) * mat[0]
       - computeMatrixDeterminant(centerLeftMatrix) * mat[1]
       + computeMatrixDeterminant(centerRightMatrix) * mat[2]
       - computeMatrixDeterminant(rightMatrix) * mat[3];
}

template <typename T>
constexpr Mat2<T> computeMatrixInverse(const Mat2<T>& mat) noexcept {
  const T determinant = Details::computeMatrixDeterminant(mat);

  if (determinant == 0)
    return mat;

  const T invDeterminant = static_cast<T>(1) / determinant;

  return Mat2<T>( mat[3] * invDeterminant, -mat[1] * invDeterminant,
                 -mat[2] * invDeterminant,  mat[0] * invDeterminant);
}

template <typename T>
constexpr Mat3<T> computeMatrixInverse(const Mat3<T>& mat) noexcept {
  const Vector<T, 3> col1(mat[0], mat[1], mat[2]);
  const Vector<T, 3> col2(mat[3], mat[4], mat[5]);
  const Vector<T, 3> col3(mat[6], mat[7], mat[8]);

  const Vector<T, 3> resultRow1 = col2.cross(col3);
  const Vector<T, 3> resultRow2 = col3.cross(col1);
  const Vector<T, 3> resultRow3 = col1.cross(col2);

  const T determinant = resultRow3.dot(col3);

  if (determinant == 0)
    return mat;

  const T invDeterminant = static_cast<T>(1) / determinant;

  return Mat3<T>(resultRow1.x() * invDeterminant, resultRow1.y() * invDeterminant, resultRow1.z() * invDeterminant,
                 resultRow2.x() * invDeterminant, resultRow2.y() * invDeterminant, resultRow2.z() * invDeterminant,
                 resultRow3.x() * invDeterminant, resultRow3.y() * invDeterminant, resultRow3.z() * invDeterminant);
}

template <typename T>
constexpr Mat4<T> computeMatrixInverse(const Mat4<T>& mat) noexcept {
  const Vector<T, 3> upperCol1(mat[0], mat[1], mat[2]);
  const Vector<T, 3> upperCol2(mat[4], mat[5], mat[6]);
  const Vector<T, 3> upperCol3(mat[8], mat[9], mat[10]);
  const Vector<T, 3> upperCol4(mat[12], mat[13], mat[14]);

  const T lastRowX = mat[3];
  const T lastRowY = mat[7];
  const T lastRowZ = mat[11];
  const T lastRowW = mat[15];

  Vector<T, 3> minorCofactor1 = upperCol1.cross(upperCol2);
  Vector<T, 3> minorCofactor2 = upperCol3.cross(upperCol4);
  Vector<T, 3> adjointTerm1   = upperCol1 * lastRowY - upperCol2 * lastRowX;
  Vector<T, 3> adjointTerm2   = upperCol3 * lastRowW - upperCol4 * lastRowZ;

  const T determinant = minorCofactor1.dot(adjointTerm2) + minorCofactor2.dot(adjointTerm1);

  if (determinant == 0)
    return mat;

  const T invDeterminant = static_cast<T>(1) / determinant;

  minorCofactor1 *= invDeterminant;
  minorCofactor2 *= invDeterminant;
  adjointTerm1   *= invDeterminant;
  adjointTerm2   *= invDeterminant;

  const Vector<T, 3> resultRow1 = upperCol2.cross(adjointTerm2) + minorCofactor2 * lastRowY;
  const Vector<T, 3> resultRow2 = adjointTerm2.cross(upperCol1) - minorCofactor2 * lastRowX;
  const Vector<T, 3> resultRow3 = upperCol4.cross(adjointTerm1) + minorCofactor1 * lastRowW;
  const Vector<T, 3> resultRow4 = adjointTerm1.cross(upperCol3) - minorCofactor1 * lastRowZ;

  return Mat4<T>(resultRow1.x(), resultRow1.y(), resultRow1.z(), -upperCol2.dot(minorCofactor2),
                 resultRow2.x(), resultRow2.y(), resultRow2.z(),  upperCol1.dot(minorCofactor2),
                 resultRow3.x(), resultRow3.y(), resultRow3.z(), -upperCol4.dot(minorCofactor1),
                 resultRow4.x(), resultRow4.y(), resultRow4.z(),  upperCol3.dot(minorCofactor1));
}

} // namespace Details

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>::Matrix(const Matrix<T, W + 1, H + 1>& mat) noexcept {
  std::size_t widthStride = 0;

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    const std::size_t resIndex = heightIndex * W;

    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
      const std::size_t finalIndex = resIndex + widthIndex;

      m_data[finalIndex] = mat[finalIndex + widthStride];
    }

    ++widthStride;
  }
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>::Matrix(const Matrix<T, W - 1, H - 1>& mat) noexcept {
  std::size_t heightStride = 0;

  for (std::size_t widthIndex = 0; widthIndex < W - 1; ++widthIndex) {
    const std::size_t finalWidthIndex = widthIndex * (H - 1);

    for (std::size_t heightIndex = 0; heightIndex < H - 1; ++heightIndex) {
      const std::size_t finalIndex = finalWidthIndex + heightIndex;

      m_data[finalIndex + heightStride] = mat[finalIndex];
    }

    ++heightStride;
  }

  m_data.back() = 1;
}

template <typename T, std::size_t W, std::size_t H>
template <typename T2, typename... Args, typename>
constexpr Matrix<T, W, H>::Matrix(T2&& val, Args&&... args) noexcept {
  static_assert(sizeof...(Args) == W * H - 1, "Error: A Matrix can't be constructed with more or less values than it can hold.");
  setValues<0, 0>(std::forward<T2>(val), std::forward<Args>(args)...);
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::identity() noexcept {
  static_assert(W == H, "Error: Matrix must be a square one.");

  Matrix res;

  for (std::size_t diagIndex = 0; diagIndex < W; ++diagIndex)
    res[diagIndex * W + diagIndex] = 1.f;

  return res;
}

template <typename T, std::size_t W, std::size_t H>
template <typename... VecsTs>
constexpr Matrix<T, W, H> Matrix<T, W, H>::fromRows(VecsTs&&... vecs) noexcept {
  static_assert(sizeof...(VecsTs) == H, "Error: A Matrix can't be constructed with more or less vectors than it can hold.");

  Matrix res;
  res.setRows(std::forward<VecsTs>(vecs)...);
  return res;
}

template <typename T, std::size_t W, std::size_t H>
template <typename... VecsTs>
constexpr Matrix<T, W, H> Matrix<T, W, H>::fromColumns(VecsTs&&... vecs) noexcept {
  static_assert(sizeof...(VecsTs) == W, "Error: A Matrix can't be constructed with more or less vectors than it can hold.");

  Matrix res;
  res.setColumns(std::forward<VecsTs>(vecs)...);
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, H, W> Matrix<T, W, H>::transpose() const noexcept {
  Matrix<T, H, W> res;

  for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
    const std::size_t finalWidthIndex = widthIndex * H;

    for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex)
      res[heightIndex * W + widthIndex] = m_data[finalWidthIndex + heightIndex];
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr T Matrix<T, W, H>::computeDeterminant() const noexcept {
  static_assert(W == H, "Error: Matrix must be a square one.");
  return Details::computeMatrixDeterminant(*this);
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::inverse() const noexcept {
  static_assert(W == H, "Error: Matrix must be a square one.");
  return Details::computeMatrixInverse(*this);
}

template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, W> Matrix<T, W, H>::recoverRow(std::size_t rowIndex) const noexcept {
  assert("Error: Given row index is out of bounds." && rowIndex < H);

  Vector<T, W> res;

  for (std::size_t valIndex = 0; valIndex < W; ++valIndex)
    res[valIndex] = m_data[rowIndex + valIndex * H];

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, H> Matrix<T, W, H>::recoverColumn(std::size_t columnIndex) const noexcept {
  assert("Error: Given column index is out of bounds." && columnIndex < W);

  Vector<T, H> res;
  const std::size_t columnBeginIndex = columnIndex * H;

  for (std::size_t valIndex = 0; valIndex < H; ++valIndex)
    res[valIndex] = m_data[columnBeginIndex + valIndex];

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr bool Matrix<T, W, H>::strictlyEquals(const Matrix& mat) const noexcept {
  return std::equal(m_data.cbegin(), m_data.cend(), mat.getData().cbegin());
}

template <typename T, std::size_t W, std::size_t H>
constexpr std::size_t Matrix<T, W, H>::hash(std::size_t seed) const noexcept {
  std::hash<T> hasher {};

  for (const T& elt : m_data)
    seed ^= hasher(elt) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);

  return seed;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator+(const Matrix& mat) const noexcept {
  Matrix res = *this;
  res += mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator+(T val) const noexcept {
  Matrix res = *this;
  res += val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator-(const Matrix& mat) const noexcept {
  Matrix res = *this;
  res -= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator-(T val) const noexcept {
  Matrix res = *this;
  res -= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator%(const Matrix& mat) const noexcept {
  Matrix res = *this;
  res %= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator*(T val) const noexcept {
  Matrix res = *this;
  res *= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator/(const Matrix& mat) const noexcept {
  Matrix res = *this;
  res /= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator/(T val) const noexcept {
  Matrix res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator+=(const Matrix& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] += mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator+=(T val) noexcept {
  for (T& elt : m_data)
    elt += val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator-=(const Matrix& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] -= mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator-=(T val) noexcept {
  for (T& elt : m_data)
    elt -= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator%=(const Matrix& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] *= mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator*=(T val) noexcept {
  for (T& elt : m_data)
    elt *= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator/=(const Matrix& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] /= mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator/=(T val) noexcept {
  for (T& elt : m_data)
    elt /= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator*=(const Matrix& mat) noexcept {
  *this = *this * mat;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr bool Matrix<T, W, H>::operator==(const Matrix& mat) const noexcept {
  if constexpr (std::is_floating_point_v<T>)
    return FloatUtils::areNearlyEqual(*this, mat);
  else
    return strictlyEquals(mat);
}

template <typename T, std::size_t W, std::size_t H>
std::ostream& operator<<(std::ostream& stream, const Matrix<T, W, H>& mat) {
  stream << '[';

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    if (heightIndex > 0)
      stream << ' ';

    stream << "[ ";

    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
      stream << mat.getElement(widthIndex, heightIndex);

      if (widthIndex < W - 1)
        stream << ", ";
    }

    stream << " ]";

    if (heightIndex < H - 1)
      stream << '\n';
  }

  stream << ']';

  return stream;
}

template <typename T, std::size_t W, std::size_t H>
template <std::size_t WI, std::size_t HI, typename T2, typename... Args>
constexpr void Matrix<T, W, H>::setValues(T2&& val, Args&&... args) noexcept {
  static_assert(std::is_same_v<T, std::decay_t<T2>>, "Error: A Matrix can only be constructed from values of the same type as its inner one.");

  m_data[WI * H + HI] = std::forward<T2>(val);

  if constexpr (sizeof...(args) > 0) {
    constexpr std::size_t newWIndex = (WI + 1) % W;
    setValues<newWIndex, HI + (newWIndex == 0)>(std::forward<Args>(args)...);
  }
}

template <typename T, std::size_t W, std::size_t H>
template <typename VecT, typename... VecsTs>
constexpr void Matrix<T, W, H>::setRows(VecT&& vec, VecsTs&&... args) noexcept {
  static_assert(std::is_same_v<std::decay_t<VecT>, Vector<T, W>>, "Error: Rows must all be vectors of the same type & size.");

  constexpr std::size_t firstIndex = H - sizeof...(args) - 1;

  for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex)
    m_data[firstIndex + widthIndex * H] = std::forward<VecT>(vec)[widthIndex];

  if constexpr (sizeof...(args) > 0)
    setRows(std::forward<VecsTs>(args)...);
}

template <typename T, std::size_t W, std::size_t H>
template <typename VecT, typename... VecsTs>
constexpr void Matrix<T, W, H>::setColumns(VecT&& vec, VecsTs&&... args) noexcept {
  static_assert(std::is_same_v<std::decay_t<VecT>, Vector<T, H>>, "Error: Columns must all be vectors of the same type & size.");

  constexpr std::size_t firstIndex = H * (W - sizeof...(args) - 1);

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex)
    m_data[firstIndex + heightIndex] = std::forward<VecT>(vec)[heightIndex];

  if constexpr (sizeof...(args) > 0)
    setColumns(std::forward<VecsTs>(args)...);
}

template <typename T, std::size_t WL, std::size_t HL, std::size_t WR, std::size_t HR>
constexpr Matrix<T, HL, WR> operator*(const Matrix<T, WL, HL>& mat1, const Matrix<T, WR, HR>& mat2) noexcept {
  static_assert(WL == HR, "Error: The left-hand side matrix's width must be equal to the right-hand side matrix's height.");

  Matrix<T, HL, WR> res;

  for (std::size_t widthIndex = 0; widthIndex < WR; ++widthIndex) {
    const std::size_t finalWidthIndex = widthIndex * HL;

    for (std::size_t heightIndex = 0; heightIndex < HL; ++heightIndex) {
      T& val = res[finalWidthIndex + heightIndex];

      for (std::size_t stride = 0; stride < WL; ++stride)
        val += mat1.getElement(stride, heightIndex) * mat2.getElement(widthIndex, stride);
    }
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, H> operator*(const Matrix<T, W, H>& mat, const Vector<T, W>& vec) noexcept {
  // This multiplication is made assuming the vector to be vertical
  Vector<T, H> res;

  for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
    const std::size_t finalWidthIndex = widthIndex * H;

    for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex)
      res[heightIndex] += mat[finalWidthIndex + heightIndex] * vec[widthIndex];
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, W> operator*(const Vector<T, H>& vec, const Matrix<T, W, H>& mat) noexcept {
  // This multiplication is made assuming the vector to be horizontal
  Vector<T, W> res;

  for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
    const std::size_t finalWidthIndex = widthIndex * H;

    for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex)
      res[widthIndex] += vec[heightIndex] * mat[finalWidthIndex + heightIndex];
  }

  return res;
}

} // namespace Raz
