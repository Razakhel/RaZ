#include <limits>
#include <algorithm>

namespace Raz {

template <typename T, unsigned int W, unsigned int H>
template <typename TI, unsigned int WI, unsigned int HI>
Matrix<T, W, H>::Matrix(const Matrix<TI, WI, HI>& mat) : Matrix<T, W, H>() {
  if (std::numeric_limits<TI>::max() > std::numeric_limits<T>::max()) {
    for (std::size_t eltIndex = 0; eltIndex < mat.getData().size(); ++eltIndex) {
      data[eltIndex] = std::max(static_cast<TI>(std::numeric_limits<T>::min()),
                                std::min(static_cast<TI>(std::numeric_limits<T>::max()),
                                         static_cast<TI>(mat.getData()[eltIndex])));
    }
  } else {
    std::copy(mat.getData().begin(), mat.getData().end(), data.begin());
  }
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>::Matrix(std::initializer_list<std::initializer_list<T>> list) {
  auto row = list.begin();

  for (std::size_t heightIndex = 0; heightIndex < list.size(); ++heightIndex, ++row) {
    auto element = row->begin();

    for (std::size_t widthIndex = 0; widthIndex < list.begin()->size(); ++widthIndex, ++element)
      data[heightIndex * list.begin()->size() + widthIndex] = *element;
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
  for (std::size_t i = 0; i < data.size(); ++i)
    data[i] += mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator+=(float val) {
  for (T& it : data)
    it += val;
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator-=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < data.size(); ++i)
    data[i] -= mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator-=(float val) {
  for (T& it : data)
    it -= val;
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator*=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < data.size(); ++i)
    data[i] *= mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator*=(float val) {
  for (T& it : data)
    it *= val;
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator/=(const Matrix<T, W, H>& mat) {
  for (std::size_t i = 0; i < data.size(); ++i)
    data[i] /= mat.getData()[i];
  return *this;
}

template <typename T, unsigned int W, unsigned int H>
Matrix<T, W, H>& Matrix<T, W, H>::operator/=(float val) {
  for (T& it : data)
    it /= val;
  return *this;
}

} // namespace Raz
