#include <algorithm>
#include <cassert>

#include "RaZ/Utils/FloatUtils.hpp"

namespace Raz {

namespace {

template <typename T>
float computeMatrixDeterminant(const Mat2<T>& mat) {
  return (mat.getData()[0] * mat.getData()[3]) - (mat.getData()[2] * mat.getData()[1]);
}

template <typename T>
float computeMatrixDeterminant(const Mat3<T>& mat) {
  const auto leftMatrix = Mat2<T>({{ mat.getData()[4], mat.getData()[5] },
                                   { mat.getData()[7], mat.getData()[8] }});

  const auto centerMatrix = Mat2<T>({{ mat.getData()[3], mat.getData()[5] },
                                     { mat.getData()[6], mat.getData()[8] }});

  const auto rightMatrix = Mat2<T>({{ mat.getData()[3], mat.getData()[4] },
                                    { mat.getData()[6], mat.getData()[7] }});

  return computeMatrixDeterminant(leftMatrix) * mat.getData()[0]
       - computeMatrixDeterminant(centerMatrix) * mat.getData()[1]
       + computeMatrixDeterminant(rightMatrix) * mat.getData()[2];
}

template <typename T>
float computeMatrixDeterminant(const Mat4<T>& mat) {
  const auto leftMatrix = Mat3<T>({{  mat.getData()[5],  mat.getData()[6],  mat.getData()[7] },
                                   {  mat.getData()[9], mat.getData()[10], mat.getData()[11] },
                                   { mat.getData()[13], mat.getData()[14], mat.getData()[15] }});

  const auto centerLeftMatrix = Mat3<T>({{  mat.getData()[4],  mat.getData()[6],  mat.getData()[7] },
                                         {  mat.getData()[8], mat.getData()[10], mat.getData()[11] },
                                         { mat.getData()[12], mat.getData()[14], mat.getData()[15] }});

  const auto centerRightMatrix = Mat3<T>({{  mat.getData()[4],  mat.getData()[5],  mat.getData()[7] },
                                          {  mat.getData()[8],  mat.getData()[9], mat.getData()[11] },
                                          { mat.getData()[12], mat.getData()[13], mat.getData()[15] }});

  const auto rightMatrix = Mat3<T>({{  mat.getData()[4],  mat.getData()[5],  mat.getData()[6] },
                                    {  mat.getData()[8],  mat.getData()[9], mat.getData()[10] },
                                    { mat.getData()[12], mat.getData()[13], mat.getData()[14] }});

  return computeMatrixDeterminant(leftMatrix) * mat.getData()[0]
       - computeMatrixDeterminant(centerLeftMatrix) * mat.getData()[1]
       + computeMatrixDeterminant(centerRightMatrix) * mat.getData()[2]
       - computeMatrixDeterminant(rightMatrix) * mat.getData()[3];
}

template <typename T>
Mat2<T> computeMatrixInverse(const Mat2<T>& mat, float determinant) {
  const Mat2<T> res({{  mat.getData()[3], -mat.getData()[1] },
                     { -mat.getData()[2],  mat.getData()[0] }});

  return res / determinant;
}

template <typename T>
Mat3<T> computeMatrixInverse(const Mat3<T>& mat, float determinant) {
  const Mat2<T> topLeft({{ mat.getData()[4], mat.getData()[5] },
                         { mat.getData()[7], mat.getData()[8] }});
  const Mat2<T> topCenter({{ mat.getData()[3], mat.getData()[5] },
                           { mat.getData()[6], mat.getData()[8] }});
  const Mat2<T> topRight({{ mat.getData()[3], mat.getData()[4] },
                          { mat.getData()[6], mat.getData()[7] }});

  const Mat2<T> midLeft({{ mat.getData()[1], mat.getData()[2] },
                         { mat.getData()[7], mat.getData()[8] }});
  const Mat2<T> midCenter({{ mat.getData()[0], mat.getData()[2] },
                           { mat.getData()[6], mat.getData()[8] }});
  const Mat2<T> midRight({{ mat.getData()[0], mat.getData()[1] },
                          { mat.getData()[6], mat.getData()[7] }});

  const Mat2<T> botLeft({{ mat.getData()[1], mat.getData()[2] },
                         { mat.getData()[4], mat.getData()[5] }});
  const Mat2<T> botCenter({{ mat.getData()[0], mat.getData()[2] },
                           { mat.getData()[3], mat.getData()[5] }});
  const Mat2<T> botRight({{ mat.getData()[0], mat.getData()[1] },
                          { mat.getData()[3], mat.getData()[4] }});

  const Mat3<T> cofactors({{  topLeft.computeDeterminant(), -topCenter.computeDeterminant(),  topRight.computeDeterminant() },
                           { -midLeft.computeDeterminant(),  midCenter.computeDeterminant(), -midRight.computeDeterminant() },
                           {  botLeft.computeDeterminant(), -botCenter.computeDeterminant(),  botRight.computeDeterminant() }});

  return cofactors.transpose() / determinant;
}

template <typename T>
Mat4<T> computeMatrixInverse(const Mat4<T>& mat, float determinant) {
  const Mat3<T> topLeft({{  mat.getData()[5],  mat.getData()[6],  mat.getData()[7] },
                         {  mat.getData()[9], mat.getData()[10], mat.getData()[11] },
                         { mat.getData()[13], mat.getData()[14], mat.getData()[15] }});
  const Mat3<T> topCenterLeft({{  mat.getData()[4],  mat.getData()[6],  mat.getData()[7] },
                               {  mat.getData()[8], mat.getData()[10], mat.getData()[11] },
                               { mat.getData()[12], mat.getData()[14], mat.getData()[15] }});
  const Mat3<T> topCenterRight({{  mat.getData()[4],  mat.getData()[5],  mat.getData()[7] },
                                {  mat.getData()[8],  mat.getData()[9], mat.getData()[11] },
                                { mat.getData()[12], mat.getData()[13], mat.getData()[15] }});
  const Mat3<T> topRight({{  mat.getData()[4],  mat.getData()[5],  mat.getData()[6] },
                          {  mat.getData()[8],  mat.getData()[9], mat.getData()[10] },
                          { mat.getData()[12], mat.getData()[13], mat.getData()[14] }});

  const Mat3<T> midTopLeft({{  mat.getData()[1],  mat.getData()[2],  mat.getData()[3] },
                            {  mat.getData()[9], mat.getData()[10], mat.getData()[11] },
                            { mat.getData()[13], mat.getData()[14], mat.getData()[15] }});
  const Mat3<T> midTopCenterLeft({{  mat.getData()[0],  mat.getData()[2],  mat.getData()[3] },
                                  {  mat.getData()[8], mat.getData()[10], mat.getData()[11] },
                                  { mat.getData()[12], mat.getData()[14], mat.getData()[15] }});
  const Mat3<T> midTopCenterRight({{  mat.getData()[0],  mat.getData()[1],  mat.getData()[3] },
                                   {  mat.getData()[8],  mat.getData()[9], mat.getData()[11] },
                                   { mat.getData()[12], mat.getData()[13], mat.getData()[15] }});
  const Mat3<T> midTopRight({{  mat.getData()[0],  mat.getData()[1],  mat.getData()[2] },
                             {  mat.getData()[8],  mat.getData()[9], mat.getData()[10] },
                             { mat.getData()[12], mat.getData()[13], mat.getData()[14] }});

  const Mat3<T> midBotLeft({{  mat.getData()[1],  mat.getData()[2],  mat.getData()[3] },
                            {  mat.getData()[5],  mat.getData()[6],  mat.getData()[7] },
                            { mat.getData()[13], mat.getData()[14], mat.getData()[15] }});
  const Mat3<T> midBotCenterLeft({{  mat.getData()[0],  mat.getData()[2],  mat.getData()[3] },
                                  {  mat.getData()[4],  mat.getData()[6],  mat.getData()[7] },
                                  { mat.getData()[12], mat.getData()[14], mat.getData()[15] }});
  const Mat3<T> midBotCenterRight({{  mat.getData()[0],  mat.getData()[1],  mat.getData()[3] },
                                   {  mat.getData()[4],  mat.getData()[5],  mat.getData()[7] },
                                   { mat.getData()[12], mat.getData()[13], mat.getData()[15] }});
  const Mat3<T> midBotRight({{  mat.getData()[0],  mat.getData()[1],  mat.getData()[2] },
                             {  mat.getData()[4],  mat.getData()[5],  mat.getData()[6] },
                             { mat.getData()[12], mat.getData()[13], mat.getData()[14] }});

  const Mat3<T> botLeft({{ mat.getData()[1],  mat.getData()[2],  mat.getData()[3] },
                         { mat.getData()[5],  mat.getData()[6],  mat.getData()[7] },
                         { mat.getData()[9], mat.getData()[10], mat.getData()[11] }});
  const Mat3<T> botCenterLeft({{ mat.getData()[0],  mat.getData()[2],  mat.getData()[3] },
                               { mat.getData()[4],  mat.getData()[6],  mat.getData()[7] },
                               { mat.getData()[8], mat.getData()[10], mat.getData()[11] }});
  const Mat3<T> botCenterRight({{ mat.getData()[0], mat.getData()[1],  mat.getData()[3] },
                                { mat.getData()[4], mat.getData()[5],  mat.getData()[7] },
                                { mat.getData()[8], mat.getData()[9], mat.getData()[11] }});
  const Mat3<T> botRight({{ mat.getData()[0], mat.getData()[1],  mat.getData()[2] },
                          { mat.getData()[4], mat.getData()[5],  mat.getData()[6] },
                          { mat.getData()[8], mat.getData()[9], mat.getData()[10] }});

  const float topLeftDeterm        = topLeft.computeDeterminant();
  const float topCenterLeftDeterm  = topCenterLeft.computeDeterminant();
  const float topCenterRightDeterm = topCenterRight.computeDeterminant();
  const float topRightDeterm       = topRight.computeDeterminant();

  const float midTopLeftDeterm        = midTopLeft.computeDeterminant();
  const float midTopCenterLeftDeterm  = midTopCenterLeft.computeDeterminant();
  const float midTopCenterRightDeterm = midTopCenterRight.computeDeterminant();
  const float midTopRightDeterm       = midTopRight.computeDeterminant();

  const float midBotLeftDeterm        = midBotLeft.computeDeterminant();
  const float midBotCenterLeftDeterm  = midBotCenterLeft.computeDeterminant();
  const float midBotCenterRightDeterm = midBotCenterRight.computeDeterminant();
  const float midBotRightDeterm       = midBotRight.computeDeterminant();

  const float botLeftDeterm        = botLeft.computeDeterminant();
  const float botCenterLeftDeterm  = botCenterLeft.computeDeterminant();
  const float botCenterRightDeterm = botCenterRight.computeDeterminant();
  const float botRightDeterm       = botRight.computeDeterminant();

  const Mat4<T> cofactors({{     topLeftDeterm,    -topCenterLeftDeterm,     topCenterRightDeterm,    -topRightDeterm },
                           { -midTopLeftDeterm,  midTopCenterLeftDeterm, -midTopCenterRightDeterm,  midTopRightDeterm },
                           {  midBotLeftDeterm, -midBotCenterLeftDeterm,  midBotCenterRightDeterm, -midBotRightDeterm },
                           {    -botLeftDeterm,     botCenterLeftDeterm,    -botCenterRightDeterm,     botRightDeterm }});

  return cofactors.transpose() / determinant;
}

} // namespace

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>::Matrix(const Matrix<T, W + 1, H + 1>& mat) {
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
Matrix<T, W, H>::Matrix(const Matrix<T, W - 1, H - 1>& mat) {
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
Matrix<T, W, H>::Matrix(std::initializer_list<std::initializer_list<T>> list) {
  assert("Error: A Matrix cannot be created with less/more values than specified." && H == list.size());

  auto row = list.begin();

  for (std::size_t heightIndex = 0; heightIndex < list.size(); ++heightIndex, ++row) {
    assert("Error: A Matrix cannot be created with less/more values than specified." && W == (list.begin() + heightIndex)->size());

    auto element = row->begin();

    for (std::size_t widthIndex = 0; widthIndex < list.begin()->size(); ++widthIndex, ++element)
      m_data[heightIndex * list.begin()->size() + widthIndex] = *element;
  }
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::identity() {
  static_assert(W == H, "Error: Matrix must be a square one.");

  Matrix<T, W, H> res;

  for (std::size_t diagIndex = 0; diagIndex < W; ++diagIndex)
    res[diagIndex * W + diagIndex] = 1.f;

  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, H, W> Matrix<T, W, H>::transpose() const {
  Matrix<T, H, W> res;

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex)
      res[widthIndex * H + heightIndex] = m_data[heightIndex * W + widthIndex];
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
float Matrix<T, W, H>::computeDeterminant() const {
  static_assert(W == H, "Error: Matrix must be a square one.");

  return computeMatrixDeterminant(*this);
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::inverse() const {
  static_assert(W == H, "Error: Matrix must be a square one.");

  return computeMatrixInverse(*this, computeMatrixDeterminant(*this));
}

template <typename T, std::size_t W, std::size_t H>
Vector<T, W> Matrix<T, W, H>::recoverRow(std::size_t rowIndex) const {
  assert("Error: Given row index is out of bounds." && rowIndex < H);

  Vector<T, W> res {};
  const std::size_t rowBeginIndex = rowIndex * W;

  for (std::size_t valIndex = 0; valIndex < W; ++valIndex)
    res[valIndex] = m_data[rowBeginIndex + valIndex];

  return res;
}

template <typename T, std::size_t W, std::size_t H>
Vector<T, H> Matrix<T, W, H>::recoverColumn(std::size_t columnIndex) const {
  assert("Error: Given column index is out of bounds." && columnIndex < W);

  Vector<T, H> res {};

  for (std::size_t valIndex = 0; valIndex < H; ++valIndex)
    res[valIndex] = m_data[columnIndex + valIndex * W];

  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator+(const Matrix& mat) const {
  Matrix<T, W, H> res = *this;
  res += mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator+(float val) const {
  Matrix<T, W, H> res = *this;
  res += val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator-(const Matrix& mat) const {
  Matrix<T, W, H> res = *this;
  res -= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator-(float val) const {
  Matrix<T, W, H> res = *this;
  res -= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator%(const Matrix& mat) const {
  Matrix<T, W, H> res = *this;
  res %= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator*(float val) const {
  Matrix<T, W, H> res = *this;
  res *= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator/(const Matrix& mat) const {
  Matrix<T, W, H> res = *this;
  res /= mat;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator/(float val) const {
  Matrix<T, W, H> res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Vector<T, H> Matrix<T, W, H>::operator*(const Vector<T, H>& vec) const {
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
Matrix<T, H, WI> Matrix<T, W, H>::operator*(const Matrix<T, WI, HI>& mat) const {
  static_assert(W == HI, "Error: Input matrix's width must be equal to current matrix's height.");

  Matrix<T, H, WI> res {};

  for (std::size_t heightIndex = 0; heightIndex < H; ++heightIndex) {
    for (std::size_t widthIndex = 0; widthIndex < W; ++widthIndex) {
      T& val = res.getData()[heightIndex * W + widthIndex];

      for (std::size_t stride = 0; stride < W; ++stride)
        val += m_data[heightIndex * W + stride] * mat.getData()[stride * WI + widthIndex];
    }
  }

  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator+=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] += mat.getData()[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator+=(float val) {
  for (T& it : m_data)
    it += val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator-=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] -= mat.getData()[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator-=(float val) {
  for (T& it : m_data)
    it -= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator%=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] *= mat.getData()[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator*=(float val) {
  for (T& it : m_data)
    it *= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator/=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] /= mat.getData()[i];
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>& Matrix<T, W, H>::operator/=(float val) {
  for (T& it : m_data)
    it /= val;
  return *this;
}

template <typename T, std::size_t W, std::size_t H>
bool Matrix<T, W, H>::operator==(const Matrix<T, W, H>& mat) const {
  if constexpr (std::is_floating_point_v<T>) {
    for (std::size_t i = 0; i < W * H; ++i) {
      if (!FloatUtils::areNearlyEqual(m_data[i], mat.getData()[i]))
        return false;
    }

    return true;
  } else {
    return std::equal(m_data.cbegin(), m_data.cend(), mat.getData().cbegin());
  }
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

} // namespace Raz
