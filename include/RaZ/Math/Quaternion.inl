#include "RaZ/Math/Constants.hpp"

namespace Raz {

template <typename T>
Quaternion<T>::Quaternion(T angle, const Vec3<T>& axes) {
  const T halfAngle = (angle * PI<T> / 180) / 2;
  const T val       = std::sin(halfAngle);

  m_real      = std::cos(halfAngle);
  m_complexes = axes * val;
}

template <typename T>
Mat4<T> Quaternion<T>::computeMatrix() const {
  const float xx = 2 * (m_complexes[0] * m_complexes[0]);
  const float yy = 2 * (m_complexes[1] * m_complexes[1]);
  const float zz = 2 * (m_complexes[2] * m_complexes[2]);

  const float xy = 2 * m_complexes[0] * m_complexes[1];
  const float xz = 2 * m_complexes[0] * m_complexes[2];
  const float yz = 2 * m_complexes[1] * m_complexes[2];

  const float xw = 2 * m_complexes[0] * m_real;
  const float yw = 2 * m_complexes[1] * m_real;
  const float zw = 2 * m_complexes[2] * m_real;

  return Mat4<T>({{ 1 - yy - zz,     xy + zw,     xz - yw, 0.f },
                  {     xy - zw, 1 - xx - zz,     yz + xw, 0.f },
                  {     xz + yw,     yz - xw, 1 - xx - yy, 0.f },
                  {         0.f,         0.f,         0.f, 1.f }});
}

} // namespace Raz
