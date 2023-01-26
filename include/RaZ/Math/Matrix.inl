#include "RaZ/Utils/FloatUtils.hpp"

#include <algorithm>
#include <cassert>

namespace Raz {

namespace {

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
constexpr Mat2<T> computeMatrixInverse(const Mat2<T>& mat, T invDeterminant) noexcept {
  return Mat2<T>( mat[3] * invDeterminant, -mat[1] * invDeterminant,
                 -mat[2] * invDeterminant,  mat[0] * invDeterminant);
}

template <typename T>
constexpr Mat3<T> computeMatrixInverse(const Mat3<T>& mat, T invDeterminant) noexcept {
  const Mat2<T> topLeft(mat[4], mat[5],
                        mat[7], mat[8]);
  const Mat2<T> topCenter(mat[3], mat[5],
                          mat[6], mat[8]);
  const Mat2<T> topRight(mat[3], mat[4],
                         mat[6], mat[7]);

  const Mat2<T> midLeft(mat[1], mat[2],
                        mat[7], mat[8]);
  const Mat2<T> midCenter(mat[0], mat[2],
                          mat[6], mat[8]);
  const Mat2<T> midRight(mat[0], mat[1],
                         mat[6], mat[7]);

  const Mat2<T> botLeft(mat[1], mat[2],
                        mat[4], mat[5]);
  const Mat2<T> botCenter(mat[0], mat[2],
                          mat[3], mat[5]);
  const Mat2<T> botRight(mat[0], mat[1],
                         mat[3], mat[4]);

  const Mat3<T> cofactors( topLeft.computeDeterminant(), -topCenter.computeDeterminant(),  topRight.computeDeterminant(),
                          -midLeft.computeDeterminant(),  midCenter.computeDeterminant(), -midRight.computeDeterminant(),
                           botLeft.computeDeterminant(), -botCenter.computeDeterminant(),  botRight.computeDeterminant());

  return cofactors * invDeterminant;
}

template <typename T>
constexpr Mat4<T> computeMatrixInverse(const Mat4<T>& mat, T invDeterminant) noexcept {
  const Mat3<T> topLeft(mat[5],  mat[6],  mat[7],
                        mat[9],  mat[10], mat[11],
                        mat[13], mat[14], mat[15]);
  const Mat3<T> topCenterLeft(mat[4],  mat[6],  mat[7],
                              mat[8],  mat[10], mat[11],
                              mat[12], mat[14], mat[15]);
  const Mat3<T> topCenterRight(mat[4],  mat[5],  mat[7],
                               mat[8],  mat[9],  mat[11],
                               mat[12], mat[13], mat[15]);
  const Mat3<T> topRight(mat[4],  mat[5],  mat[6],
                         mat[8],  mat[9],  mat[10],
                         mat[12], mat[13], mat[14]);

  const Mat3<T> midTopLeft(mat[1],  mat[2],  mat[3],
                           mat[9],  mat[10], mat[11],
                           mat[13], mat[14], mat[15]);
  const Mat3<T> midTopCenterLeft(mat[0],  mat[2],  mat[3],
                                 mat[8],  mat[10], mat[11],
                                 mat[12], mat[14], mat[15]);
  const Mat3<T> midTopCenterRight(mat[0],  mat[1],  mat[3],
                                  mat[8],  mat[9],  mat[11],
                                  mat[12], mat[13], mat[15]);
  const Mat3<T> midTopRight(mat[0],  mat[1],  mat[2],
                            mat[8],  mat[9],  mat[10],
                            mat[12], mat[13], mat[14]);

  const Mat3<T> midBotLeft(mat[1],  mat[2],  mat[3],
                           mat[5],  mat[6],  mat[7],
                           mat[13], mat[14], mat[15]);
  const Mat3<T> midBotCenterLeft(mat[0],  mat[2],  mat[3],
                                 mat[4],  mat[6],  mat[7],
                                 mat[12], mat[14], mat[15]);
  const Mat3<T> midBotCenterRight(mat[0],  mat[1],  mat[3],
                                  mat[4],  mat[5],  mat[7],
                                  mat[12], mat[13], mat[15]);
  const Mat3<T> midBotRight(mat[0],  mat[1],  mat[2],
                            mat[4],  mat[5],  mat[6],
                            mat[12], mat[13], mat[14]);

  const Mat3<T> botLeft(mat[1], mat[2], mat[3],
                        mat[5], mat[6], mat[7],
                        mat[9], mat[10], mat[11]);
  const Mat3<T> botCenterLeft(mat[0], mat[2],  mat[3],
                              mat[4], mat[6],  mat[7],
                              mat[8], mat[10], mat[11]);
  const Mat3<T> botCenterRight(mat[0], mat[1], mat[3],
                               mat[4], mat[5], mat[7],
                               mat[8], mat[9], mat[11]);
  const Mat3<T> botRight(mat[0], mat[1], mat[2],
                         mat[4], mat[5], mat[6],
                         mat[8], mat[9], mat[10]);

  const T topLeftDeterm        = topLeft.computeDeterminant();
  const T topCenterLeftDeterm  = topCenterLeft.computeDeterminant();
  const T topCenterRightDeterm = topCenterRight.computeDeterminant();
  const T topRightDeterm       = topRight.computeDeterminant();

  const T midTopLeftDeterm        = midTopLeft.computeDeterminant();
  const T midTopCenterLeftDeterm  = midTopCenterLeft.computeDeterminant();
  const T midTopCenterRightDeterm = midTopCenterRight.computeDeterminant();
  const T midTopRightDeterm       = midTopRight.computeDeterminant();

  const T midBotLeftDeterm        = midBotLeft.computeDeterminant();
  const T midBotCenterLeftDeterm  = midBotCenterLeft.computeDeterminant();
  const T midBotCenterRightDeterm = midBotCenterRight.computeDeterminant();
  const T midBotRightDeterm       = midBotRight.computeDeterminant();

  const T botLeftDeterm        = botLeft.computeDeterminant();
  const T botCenterLeftDeterm  = botCenterLeft.computeDeterminant();
  const T botCenterRightDeterm = botCenterRight.computeDeterminant();
  const T botRightDeterm       = botRight.computeDeterminant();

  const Mat4<T> cofactors( topLeftDeterm,    -topCenterLeftDeterm,     topCenterRightDeterm,    -topRightDeterm,
                          -midTopLeftDeterm,  midTopCenterLeftDeterm, -midTopCenterRightDeterm,  midTopRightDeterm,
                           midBotLeftDeterm, -midBotCenterLeftDeterm,  midBotCenterRightDeterm, -midBotRightDeterm,
                          -botLeftDeterm,     botCenterLeftDeterm,    -botCenterRightDeterm,     botRightDeterm);

  return cofactors * invDeterminant;
}

} // namespace

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>::Matrix(const Matrix<T, W + 1, H + 1>& mat) noexcept {
  std::size_t widthStride = 0;

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    std::size_t resIndex = heightIndex * W;

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

  Matrix<T, W, H> res;

  for (std::size_t diagIndex = 0; diagIndex < W; ++diagIndex)
    res[diagIndex * W + diagIndex] = 1.f;

  return res;
}

template <typename T, std::size_t W, std::size_t H>
template <typename... Vecs>
constexpr Matrix<T, W, H> Matrix<T, W, H>::fromRows(Vecs&&... vecs) noexcept {
  static_assert(sizeof...(Vecs) == H, "Error: A Matrix can't be constructed with more or less vectors than it can hold.");

  Matrix res;
  res.setRows(std::forward<Vecs>(vecs)...);
  return res;
}

template <typename T, std::size_t W, std::size_t H>
template <typename... Vecs>
constexpr Matrix<T, W, H> Matrix<T, W, H>::fromColumns(Vecs&&... vecs) noexcept {
  static_assert(sizeof...(Vecs) == W, "Error: A Matrix can't be constructed with more or less vectors than it can hold.");

  Matrix res;
  res.setColumns(std::forward<Vecs>(vecs)...);
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

  return computeMatrixDeterminant(*this);
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::inverse() const noexcept {
  static_assert(W == H, "Error: Matrix must be a square one.");

  const T determ = computeMatrixDeterminant(*this);

  if (determ == 0)
    return *this;

  return computeMatrixInverse(*this, static_cast<T>(1) / determ);
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
constexpr bool Matrix<T, W, H>::strictlyEquals(const Matrix<T, W, H>& mat) const noexcept {
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
  Matrix<T, W, H> res = *this;
  res += mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator+(T val) const noexcept {
  Matrix<T, W, H> res = *this;
  res += val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator-(const Matrix& mat) const noexcept {
  Matrix<T, W, H> res = *this;
  res -= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator-(T val) const noexcept {
  Matrix<T, W, H> res = *this;
  res -= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator%(const Matrix& mat) const noexcept {
  Matrix<T, W, H> res = *this;
  res %= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator*(T val) const noexcept {
  Matrix<T, W, H> res = *this;
  res *= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator/(const Matrix& mat) const noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  Matrix<T, W, H> res = *this;
  res /= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator/(T val) const noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  Matrix<T, W, H> res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator+=(const Matrix<T, W, H>& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] += mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator+=(T val) noexcept {
  for (T& it : m_data)
    it += val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator-=(const Matrix<T, W, H>& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] -= mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator-=(T val) noexcept {
  for (T& it : m_data)
    it -= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator%=(const Matrix<T, W, H>& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] *= mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator*=(T val) noexcept {
  for (T& it : m_data)
    it *= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator/=(const Matrix<T, W, H>& mat) noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] /= mat[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator/=(T val) noexcept(std::is_integral_v<T> || std::numeric_limits<T>::is_iec559) {
  for (T& it : m_data)
    it /= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator*=(const Matrix<T, W, H>& mat) noexcept {
  *this = *this * mat;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr bool Matrix<T, W, H>::operator==(const Matrix<T, W, H>& mat) const noexcept {
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
template <typename Vec, typename... Vecs>
constexpr void Matrix<T, W, H>::setRows(Vec&& vec, Vecs&&... args) noexcept {
  static_assert(std::is_same_v<std::decay_t<Vec>, Vector<T, W>>, "Error: Rows must all be vectors of the same type & size.");

  constexpr std::size_t firstIndex = H - sizeof...(args) - 1;

  for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex)
    m_data[firstIndex + widthIndex * H] = vec[widthIndex];

  if constexpr (sizeof...(args) > 0)
    setRows(std::forward<Vecs>(args)...);
}

template <typename T, std::size_t W, std::size_t H>
template <typename Vec, typename... Vecs>
constexpr void Matrix<T, W, H>::setColumns(Vec&& vec, Vecs&&... args) noexcept {
  static_assert(std::is_same_v<std::decay_t<Vec>, Vector<T, H>>, "Error: Columns must all be vectors of the same type & size.");

  constexpr std::size_t firstIndex = H * (W - sizeof...(args) - 1);

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex)
    m_data[firstIndex + heightIndex] = vec[heightIndex];

  if constexpr (sizeof...(args) > 0)
    setColumns(std::forward<Vecs>(args)...);
}

template <typename T, std::size_t W, std::size_t H, std::size_t WI, std::size_t HI>
constexpr Matrix<T, H, WI> operator*(const Matrix<T, W, H>& mat1, const Matrix<T, WI, HI>& mat2) noexcept {
  static_assert(W == HI, "Error: The left-hand matrix's width must be equal to the right-hand matrix's height.");

  Matrix<T, H, WI> res;

  for (std::size_t widthIndex = 0; widthIndex < WI; ++widthIndex) {
    const std::size_t finalWidthIndex = widthIndex * H;

    for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
      T& val = res[finalWidthIndex + heightIndex];

      for (std::size_t stride = 0; stride < W; ++stride)
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

} // namespace Raz
