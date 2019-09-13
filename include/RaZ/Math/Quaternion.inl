#include "RaZ/Math/Constants.hpp"

namespace Raz {

template <typename T>
Quaternion<T>::Quaternion(Radians<T> angle, const Vec3<T>& axis) {
  const T halfAngle = angle.value / 2;
  const T val       = std::sin(halfAngle);

  m_real      = std::cos(halfAngle);
  m_complexes = axis * val;
}

template <typename T>
Quaternion<T> Quaternion<T>::normalize() const {
  Quaternion<T> res = *this;
  const T sqNorm = computeSquaredNorm();

  if (sqNorm > 0) {
    const T invNorm = 1 / std::sqrt(sqNorm);

    res.m_real      *= invNorm;
    res.m_complexes *= invNorm;
  }

  return res;
}

template <typename T>
Quaternion<T> Quaternion<T>::conjugate() const {
  Quaternion<T> res = *this;
  res.m_complexes   = -m_complexes;

  return res;
}

template <typename T>
Quaternion<T> Quaternion<T>::inverse() const {
  Quaternion<T> res = *this;
  const T sqNorm = computeSquaredNorm();

  if (sqNorm > 0) {
    const T invSqNorm = 1 / sqNorm;

    res.m_real      *= invSqNorm;
    res.m_complexes *= -invSqNorm;
  }

  return res;
}

template <typename T>
Mat4<T> Quaternion<T>::computeMatrix() const {
  const T invSqNorm = 1 / computeSquaredNorm();

  const T xx = (2 * m_complexes[0] * m_complexes[0]) * invSqNorm;
  const T yy = (2 * m_complexes[1] * m_complexes[1]) * invSqNorm;
  const T zz = (2 * m_complexes[2] * m_complexes[2]) * invSqNorm;

  const T xy = (2 * m_complexes[0] * m_complexes[1]) * invSqNorm;
  const T xz = (2 * m_complexes[0] * m_complexes[2]) * invSqNorm;
  const T yz = (2 * m_complexes[1] * m_complexes[2]) * invSqNorm;

  const T xw = (2 * m_complexes[0] * m_real) * invSqNorm;
  const T yw = (2 * m_complexes[1] * m_real) * invSqNorm;
  const T zw = (2 * m_complexes[2] * m_real) * invSqNorm;

  return Mat4<T>({{ 1 - yy - zz,     xy + zw,     xz - yw, 0 },
                  {     xy - zw, 1 - xx - zz,     yz + xw, 0 },
                  {     xz + yw,     yz - xw, 1 - xx - yy, 0 },
                  {           0,           0,           0, 1 }});
}

} // namespace Raz
