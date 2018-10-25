#include "RaZ/Math/Constants.hpp"

namespace Raz {

template <typename T>
Quaternion<T>::Quaternion(T angleDegrees, const Vec3<T>& axes) {
  const T halfAngle = (angleDegrees * PI<T> / 180) / 2;
  const T val       = std::sin(halfAngle);

  m_real      = std::cos(halfAngle);
  m_complexes = axes * val;
}

template <typename T>
Mat4<T> Quaternion<T>::computeMatrix() const {
  const T xx = 2 * (m_complexes[0] * m_complexes[0]);
  const T yy = 2 * (m_complexes[1] * m_complexes[1]);
  const T zz = 2 * (m_complexes[2] * m_complexes[2]);

  const T xy = 2 * m_complexes[0] * m_complexes[1];
  const T xz = 2 * m_complexes[0] * m_complexes[2];
  const T yz = 2 * m_complexes[1] * m_complexes[2];

  const T xw = 2 * m_complexes[0] * m_real;
  const T yw = 2 * m_complexes[1] * m_real;
  const T zw = 2 * m_complexes[2] * m_real;

  return Mat4<T>({{ 1 - yy - zz,     xy + zw,     xz - yw, 0.f },
                  {     xy - zw, 1 - xx - zz,     yz + xw, 0.f },
                  {     xz + yw,     yz - xw, 1 - xx - yy, 0.f },
                  {         0.f,         0.f,         0.f, 1.f }});
}

} // namespace Raz
