#include "RaZ/Utils/FloatUtils.hpp"

#include <algorithm>
#include <cassert>

namespace Raz {

namespace {

template <typename T>
constexpr T computeMatrixDeterminant(const Mat2<T>& mat) noexcept {
  return (mat.getData()[0] * mat.getData()[3]) - (mat.getData()[2] * mat.getData()[1]);
}

template <typename T>
constexpr T computeMatrixDeterminant(const Mat3<T>& mat) noexcept {
  const auto leftMatrix = Mat2<T>(mat.getData()[4], mat.getData()[5],
                                  mat.getData()[7], mat.getData()[8]);

  const auto centerMatrix = Mat2<T>(mat.getData()[3], mat.getData()[5],
                                    mat.getData()[6], mat.getData()[8]);

  const auto rightMatrix = Mat2<T>(mat.getData()[3], mat.getData()[4],
                                   mat.getData()[6], mat.getData()[7]);

  return computeMatrixDeterminant(leftMatrix) * mat.getData()[0]
       - computeMatrixDeterminant(centerMatrix) * mat.getData()[1]
       + computeMatrixDeterminant(rightMatrix) * mat.getData()[2];
}

template <typename T>
constexpr T computeMatrixDeterminant(const Mat4<T>& mat) noexcept {
  const auto leftMatrix = Mat3<T>(mat.getData()[5],  mat.getData()[6],  mat.getData()[7],
                                  mat.getData()[9],  mat.getData()[10], mat.getData()[11],
                                  mat.getData()[13], mat.getData()[14], mat.getData()[15]);

  const auto centerLeftMatrix = Mat3<T>(mat.getData()[4],  mat.getData()[6],  mat.getData()[7],
                                        mat.getData()[8],  mat.getData()[10], mat.getData()[11],
                                        mat.getData()[12], mat.getData()[14], mat.getData()[15]);

  const auto centerRightMatrix = Mat3<T>(mat.getData()[4],  mat.getData()[5],  mat.getData()[7],
                                         mat.getData()[8],  mat.getData()[9],  mat.getData()[11],
                                         mat.getData()[12], mat.getData()[13], mat.getData()[15]);

  const auto rightMatrix = Mat3<T>(mat.getData()[4],  mat.getData()[5],  mat.getData()[6],
                                   mat.getData()[8],  mat.getData()[9],  mat.getData()[10],
                                   mat.getData()[12], mat.getData()[13], mat.getData()[14]);

  return computeMatrixDeterminant(leftMatrix) * mat.getData()[0]
       - computeMatrixDeterminant(centerLeftMatrix) * mat.getData()[1]
       + computeMatrixDeterminant(centerRightMatrix) * mat.getData()[2]
       - computeMatrixDeterminant(rightMatrix) * mat.getData()[3];
}

template <typename T>
constexpr Mat2<T> computeMatrixInverse(const Mat2<T>& mat, T determinant) {
  const Mat2<T> res( mat.getData()[3], -mat.getData()[1],
                    -mat.getData()[2],  mat.getData()[0]);

  return res / determinant;
}

template <typename T>
constexpr Mat3<T> computeMatrixInverse(const Mat3<T>& mat, T determinant) {
  const Mat2<T> topLeft(mat.getData()[4], mat.getData()[5],
                        mat.getData()[7], mat.getData()[8]);
  const Mat2<T> topCenter(mat.getData()[3], mat.getData()[5],
                          mat.getData()[6], mat.getData()[8]);
  const Mat2<T> topRight(mat.getData()[3], mat.getData()[4],
                         mat.getData()[6], mat.getData()[7]);

  const Mat2<T> midLeft(mat.getData()[1], mat.getData()[2],
                        mat.getData()[7], mat.getData()[8]);
  const Mat2<T> midCenter(mat.getData()[0], mat.getData()[2],
                          mat.getData()[6], mat.getData()[8]);
  const Mat2<T> midRight(mat.getData()[0], mat.getData()[1],
                         mat.getData()[6], mat.getData()[7]);

  const Mat2<T> botLeft(mat.getData()[1], mat.getData()[2],
                        mat.getData()[4], mat.getData()[5]);
  const Mat2<T> botCenter(mat.getData()[0], mat.getData()[2],
                          mat.getData()[3], mat.getData()[5]);
  const Mat2<T> botRight(mat.getData()[0], mat.getData()[1],
                         mat.getData()[3], mat.getData()[4]);

  const Mat3<T> cofactors( topLeft.computeDeterminant(), -topCenter.computeDeterminant(),  topRight.computeDeterminant(),
                          -midLeft.computeDeterminant(),  midCenter.computeDeterminant(), -midRight.computeDeterminant(),
                           botLeft.computeDeterminant(), -botCenter.computeDeterminant(),  botRight.computeDeterminant());

  return cofactors.transpose() / determinant;
}

template <typename T>
constexpr Mat4<T> computeMatrixInverse(const Mat4<T>& mat, T determinant) {
  const Mat3<T> topLeft(mat.getData()[5],  mat.getData()[6],  mat.getData()[7],
                        mat.getData()[9],  mat.getData()[10], mat.getData()[11],
                        mat.getData()[13], mat.getData()[14], mat.getData()[15]);
  const Mat3<T> topCenterLeft(mat.getData()[4],  mat.getData()[6],  mat.getData()[7],
                              mat.getData()[8],  mat.getData()[10], mat.getData()[11],
                              mat.getData()[12], mat.getData()[14], mat.getData()[15]);
  const Mat3<T> topCenterRight(mat.getData()[4],  mat.getData()[5],  mat.getData()[7],
                               mat.getData()[8],  mat.getData()[9],  mat.getData()[11],
                               mat.getData()[12], mat.getData()[13], mat.getData()[15]);
  const Mat3<T> topRight(mat.getData()[4],  mat.getData()[5],  mat.getData()[6],
                         mat.getData()[8],  mat.getData()[9],  mat.getData()[10],
                         mat.getData()[12], mat.getData()[13], mat.getData()[14]);

  const Mat3<T> midTopLeft(mat.getData()[1],  mat.getData()[2],  mat.getData()[3],
                           mat.getData()[9],  mat.getData()[10], mat.getData()[11],
                           mat.getData()[13], mat.getData()[14], mat.getData()[15]);
  const Mat3<T> midTopCenterLeft(mat.getData()[0],  mat.getData()[2],  mat.getData()[3],
                                 mat.getData()[8],  mat.getData()[10], mat.getData()[11],
                                 mat.getData()[12], mat.getData()[14], mat.getData()[15]);
  const Mat3<T> midTopCenterRight(mat.getData()[0],  mat.getData()[1],  mat.getData()[3],
                                  mat.getData()[8],  mat.getData()[9],  mat.getData()[11],
                                  mat.getData()[12], mat.getData()[13], mat.getData()[15]);
  const Mat3<T> midTopRight(mat.getData()[0],  mat.getData()[1],  mat.getData()[2],
                            mat.getData()[8],  mat.getData()[9],  mat.getData()[10],
                            mat.getData()[12], mat.getData()[13], mat.getData()[14]);

  const Mat3<T> midBotLeft(mat.getData()[1],  mat.getData()[2],  mat.getData()[3],
                           mat.getData()[5],  mat.getData()[6],  mat.getData()[7],
                           mat.getData()[13], mat.getData()[14], mat.getData()[15]);
  const Mat3<T> midBotCenterLeft(mat.getData()[0],  mat.getData()[2],  mat.getData()[3],
                                 mat.getData()[4],  mat.getData()[6],  mat.getData()[7],
                                 mat.getData()[12], mat.getData()[14], mat.getData()[15]);
  const Mat3<T> midBotCenterRight(mat.getData()[0],  mat.getData()[1],  mat.getData()[3],
                                  mat.getData()[4],  mat.getData()[5],  mat.getData()[7],
                                  mat.getData()[12], mat.getData()[13], mat.getData()[15]);
  const Mat3<T> midBotRight(mat.getData()[0],  mat.getData()[1],  mat.getData()[2],
                            mat.getData()[4],  mat.getData()[5],  mat.getData()[6],
                            mat.getData()[12], mat.getData()[13], mat.getData()[14]);

  const Mat3<T> botLeft(mat.getData()[1], mat.getData()[2], mat.getData()[3],
                        mat.getData()[5], mat.getData()[6], mat.getData()[7],
                        mat.getData()[9], mat.getData()[10], mat.getData()[11]);
  const Mat3<T> botCenterLeft(mat.getData()[0], mat.getData()[2],  mat.getData()[3],
                              mat.getData()[4], mat.getData()[6],  mat.getData()[7],
                              mat.getData()[8], mat.getData()[10], mat.getData()[11]);
  const Mat3<T> botCenterRight(mat.getData()[0], mat.getData()[1], mat.getData()[3],
                               mat.getData()[4], mat.getData()[5], mat.getData()[7],
                               mat.getData()[8], mat.getData()[9], mat.getData()[11]);
  const Mat3<T> botRight(mat.getData()[0], mat.getData()[1], mat.getData()[2],
                         mat.getData()[4], mat.getData()[5], mat.getData()[6],
                         mat.getData()[8], mat.getData()[9], mat.getData()[10]);

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

  return cofactors.transpose() / determinant;
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
  std::size_t widthStride = 0;

  for (std::size_t heightIndex = 0; heightIndex < H - 1; ++heightIndex) {
    std::size_t resIndex = heightIndex * (W - 1);

    for (std::size_t widthIndex = 0; widthIndex < W - 1; ++widthIndex) {
      const std::size_t finalIndex = resIndex + widthIndex;

      m_data[finalIndex + widthStride] = mat[finalIndex];
    }

    ++widthStride;
  }

  m_data.back() = 1;
}

template <typename T, std::size_t W, std::size_t H>
template <typename... Args>
constexpr Matrix<T, W, H>::Matrix(T val, Args&&... args) noexcept : m_data{ std::move(val), std::forward<Args>(args)... } {
  static_assert(sizeof...(Args) == W * H - 1, "Error: A Matrix can't be constructed with more or less values than it can hold.");
  static_assert((std::is_same_v<T, std::decay_t<Args>> && ...), "Error: A Matrix can only be constructed from values of the same type as its inner one.");
}

template <typename T, std::size_t W, std::size_t H>
template <typename... Vecs>
constexpr Matrix<T, W, H>::Matrix(const Vector<T, W>& vec, Vecs&&... vecs) noexcept {
  static_assert(sizeof...(Vecs) == H - 1, "Error: A Matrix can't be constructed with more or less vectors than it can hold.");
  static_assert((std::is_same_v<Vector<T, W>, std::decay_t<Vecs>> && ...),
                "Error: A Matrix can only be constructed from vectors of the same inner type as its own.");

  setRows(vec, std::forward<Vecs>(vecs)...);
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
constexpr Matrix<T, H, W> Matrix<T, W, H>::transpose() const noexcept {
  Matrix<T, H, W> res;

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex)
      res[widthIndex * H + heightIndex] = m_data[heightIndex * W + widthIndex];
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr T Matrix<T, W, H>::computeDeterminant() const noexcept {
  static_assert(W == H, "Error: Matrix must be a square one.");

  return computeMatrixDeterminant(*this);
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::inverse() const {
  static_assert(W == H, "Error: Matrix must be a square one.");

  return computeMatrixInverse(*this, computeMatrixDeterminant(*this));
}

template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, W> Matrix<T, W, H>::recoverRow(std::size_t rowIndex) const noexcept {
  assert("Error: Given row index is out of bounds." && rowIndex < H);

  Vector<T, W> res {};
  const std::size_t rowBeginIndex = rowIndex * W;

  for (std::size_t valIndex = 0; valIndex < W; ++valIndex)
    res[valIndex] = m_data[rowBeginIndex + valIndex];

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, H> Matrix<T, W, H>::recoverColumn(std::size_t columnIndex) const noexcept {
  assert("Error: Given column index is out of bounds." && columnIndex < W);

  Vector<T, H> res {};

  for (std::size_t valIndex = 0; valIndex < H; ++valIndex)
    res[valIndex] = m_data[columnIndex + valIndex * W];

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
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator/(const Matrix& mat) const {
  Matrix<T, W, H> res = *this;
  res /= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H> Matrix<T, W, H>::operator/(T val) const {
  Matrix<T, W, H> res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Vector<T, H> Matrix<T, W, H>::operator*(const Vector<T, H>& vec) const noexcept {
  // This multiplication is made assuming the vector to be vertical
  Vector<T, H> res {};

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex)
      res[heightIndex] += m_data[heightIndex * W + widthIndex] * vec[widthIndex];
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
template <std::size_t WI, std::size_t HI>
constexpr Matrix<T, H, WI> Matrix<T, W, H>::operator*(const Matrix<T, WI, HI>& mat) const noexcept {
  static_assert(W == HI, "Error: Input matrix's width must be equal to current matrix's height.");

  Matrix<T, H, WI> res {};

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
      T& val = res.getElement(widthIndex, heightIndex);

      for (std::size_t stride = 0; stride < W; ++stride)
        val += getElement(stride, heightIndex) * mat.getElement(widthIndex, stride);
    }
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator+=(const Matrix<T, W, H>& mat) noexcept {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] += mat.getData()[i];
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
    m_data[i] -= mat.getData()[i];
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
    m_data[i] *= mat.getData()[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator*=(T val) noexcept {
  for (T& it : m_data)
    it *= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator/=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] /= mat.getData()[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
constexpr Matrix<T, W, H>& Matrix<T, W, H>::operator/=(T val) {
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
  stream << "[[ " << mat.getData()[0];

  for (std::size_t widthIndex = 1; widthIndex < W; ++widthIndex)
    stream << "; " << mat.getData()[widthIndex];

  stream << " ]\n";

  for (std::size_t heightIndex = 1; heightIndex < H - 1; ++heightIndex) {
    stream << " [ " << mat.getData()[heightIndex * W];

    for (std::size_t widthIndex = 1; widthIndex < W; ++widthIndex)
      stream << "; " << mat.getData()[heightIndex * W + widthIndex];

    stream << " ]\n";
  }

  // To avoid breaking a line at the end
  stream << " [ " << mat.getData()[(H - 1) * W];

  for (std::size_t widthIndex = 1; widthIndex < W; ++widthIndex)
    stream << "; " << mat.getData()[(H - 1) * W + widthIndex];

  stream << " ]]";

  return stream;
}

template <typename T, std::size_t W, std::size_t H>
template <typename Vec, typename... Vecs>
constexpr void Matrix<T, W, H>::setRows(Vec&& vec, Vecs&&... args) noexcept {
  static_assert(std::is_same_v<std::decay_t<Vec>, Vector<T, W>>, "Error: Rows must all be vectors of the same type & size.");

  constexpr std::size_t firstIndex = W * (H - sizeof...(args) - 1);

  for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex)
    m_data[firstIndex + widthIndex] = vec[widthIndex];

  if constexpr (sizeof...(args) > 0)
    setRows(std::forward<Vecs>(args)...);
}

} // namespace Raz
