#include <limits>
#include <algorithm>

namespace Raz {

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>::Matrix() {
  if (W == H) {
    for (unsigned int heightIndex = 0; heightIndex < H; ++heightIndex) {
      for (unsigned int widthIndex = 0; widthIndex < W; ++widthIndex)
        m_data[heightIndex * W + widthIndex] = (heightIndex != widthIndex ? 0.f : 1.f);
    }
  }
};

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>::Matrix(std::initializer_list<std::initializer_list<T>> list) {
  auto row = list.begin();

  for (std::size_t heightIndex = 0; heightIndex < list.size(); ++heightIndex, ++row) {
    auto element = row->begin();

    for (std::size_t widthIndex = 0; widthIndex < list.begin()->size(); ++widthIndex, ++element)
      m_data[heightIndex * list.begin()->size() + widthIndex] = *element;
  }
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator+(Matrix mat) {
  mat += *this;
  return mat;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator+(float val) {
  Matrix<T, W, H> res = *this;
  res += val;
  return res;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator-(Matrix mat) {
  mat -= *this;
  return mat;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator-(float val) {
  Matrix<T, W, H> res = *this;
  res -= val;
  return res;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator*(Matrix mat) {
  mat *= *this;
  return mat;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator*(float val) {
  Matrix<T, W, H> res = *this;
  res *= val;
  return res;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator/(Matrix mat) {
  mat /= *this;
  return mat;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H> Matrix<T, W, H>::operator/(float val) {
  Matrix<T, W, H> res = *this;
  res /= val;
  return res;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator+=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] += mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator+=(float val) {
  for (T& it : m_data)
    it += val;
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator-=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] -= mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator-=(float val) {
  for (T& it : m_data)
    it -= val;
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator*=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] *= mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator*=(float val) {
  for (T& it : m_data)
    it *= val;
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator/=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < m_data.size(); ++i)
    m_data[i] /= mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator/=(float val) {
  for (T& it : m_data)
    it /= val;
  return *this;
}

} // namespace Raz
