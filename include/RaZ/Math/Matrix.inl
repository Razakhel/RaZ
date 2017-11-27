#include <algorithm>

namespace Raz {

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H>::Matrix(std::initializer_list<std::initializer_list<T>> list) {
  auto row = list.begin();

  for (std::size_t heightIndex = 0; heightIndex < list.size(); ++heightIndex, ++row) {
    auto element = row->begin();

    for (std::size_t widthIndex = 0; widthIndex < list.begin()->size(); ++widthIndex, ++element)
      m_data[heightIndex * list.begin()->size() + widthIndex] = *element;
  }
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::identity() {
  static_assert(("Error: Matrix must be a square one.", W == H));

  Matrix<T, W, H> res;

  for (std::size_t diagIndex = 0; diagIndex < W; ++diagIndex)
    res[diagIndex * W + diagIndex] = 1.f;

  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator+(Matrix mat) const {
  mat += *this;
  return mat;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator+(float val) const {
  Matrix<T, W, H> res = *this;
  res += val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator-(Matrix mat) const {
  mat -= *this;
  return mat;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator-(float val) const {
  Matrix<T, W, H> res = *this;
  res -= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator%(Matrix mat) const {
  mat %= *this;
  return mat;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator%(float val) const {
  Matrix<T, W, H> res = *this;
  res %= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator/(Matrix mat) const {
  mat /= *this;
  return mat;
}

template <typename T, std::size_t W, std::size_t H>
Matrix<T, W, H> Matrix<T, W, H>::operator/(float val) const {
  Matrix<T, W, H> res = *this;
  res /= val;
  return res;
}

template <typename T, std::size_t W, std::size_t H>
template <std::size_t WI, std::size_t HI>
Matrix<T, H, WI> Matrix<T, W, H>::operator*(const Matrix<T, WI, HI>& mat) const {
  static_assert(("Error: Input matrix's width must be equal to current matrix's height.", W == HI));

  Matrix<T, H, WI> res;

  for (std::size_t currHeightIndex = 0; currHeightIndex < H; ++currHeightIndex) {
    for (std::size_t inHeightIndex = 0; inHeightIndex < HI; ++inHeightIndex) {
      for (std::size_t currWidthIndex = 0; currWidthIndex < W; ++currWidthIndex)
        res.getData()[inHeightIndex * WI + currWidthIndex] += m_data[currHeightIndex * W + currWidthIndex]
                                                              * mat[inHeightIndex * WI + currHeightIndex];
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
Matrix<T, W, H>& Matrix<T, W, H>::operator%=(float val) {
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

} // namespace Raz
