#include "RaZ/Math/Constants.hpp"

namespace Raz {

template <typename T>
constexpr Quaternion<T>::Quaternion(Radians<T> angle, const Vec3<T>& axis) noexcept {
  const T halfAngle = angle.value / 2;
  const T val       = std::sin(halfAngle);

  m_real      = std::cos(halfAngle);
  m_complexes = axis * val;
}

template <typename T>
constexpr Quaternion<T> Quaternion<T>::normalize() const noexcept {
  Quaternion<T> res = *this;
  const T sqNorm    = computeSquaredNorm();

  if (sqNorm > 0) {
    const T invSqNorm = 1 / std::sqrt(sqNorm);

    res.m_real      *= invSqNorm;
    res.m_complexes *= invSqNorm;
  }

  return res;
}

template <typename T>
constexpr Quaternion<T> Quaternion<T>::conjugate() const noexcept {
  Quaternion<T> res = *this;
  res.m_complexes   = -m_complexes;

  return res;
}

template <typename T>
constexpr Quaternion<T> Quaternion<T>::inverse() const noexcept {
  Quaternion<T> res = *this;
  const T sqNorm    = computeSquaredNorm();

  if (sqNorm > 0) {
    const T invSqNorm = 1 / sqNorm;

    res.m_real      *= invSqNorm;
    res.m_complexes *= -invSqNorm;
  }

  return res;
}

template <typename T>
constexpr Mat4<T> Quaternion<T>::computeMatrix() const {
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

  // Since RaZ's matrices are of a different majorness (row-major) than the usual (column-major),
  //  the following matrix is transposed compared to the common implementations & usages (like glm)
  return Mat4<T>(1 - yy - zz,       xy - zw,           xz + yw,           static_cast<T>(0),
                 xy + zw,           1 - xx - zz,       yz - xw,           static_cast<T>(0),
                 xz - yw,           yz + xw,           1 - xx - yy,       static_cast<T>(0),
                 static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
}

template <typename T>
constexpr Quaternion<T> Quaternion<T>::operator*(const Quaternion& quat) const noexcept {
  Quaternion<T> res = *this;
  res *= quat;
  return res;
}

template <typename T>
constexpr Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& quat) noexcept {
  const Quaternion<T> res = *this;

  m_real = res.m_real         * quat.m_real
         - res.m_complexes[0] * quat.m_complexes[0]
         - res.m_complexes[1] * quat.m_complexes[1]
         - res.m_complexes[2] * quat.m_complexes[2];

  m_complexes[0] = res.m_real         * quat.m_complexes[0]
                 + res.m_complexes[0] * quat.m_real
                 + res.m_complexes[1] * quat.m_complexes[2]
                 - res.m_complexes[2] * quat.m_complexes[1];

  m_complexes[1] = res.m_real         * quat.m_complexes[1]
                 - res.m_complexes[0] * quat.m_complexes[2]
                 + res.m_complexes[1] * quat.m_real
                 + res.m_complexes[2] * quat.m_complexes[0];

  m_complexes[2] = res.m_real         * quat.m_complexes[2]
                 + res.m_complexes[0] * quat.m_complexes[1]
                 - res.m_complexes[1] * quat.m_complexes[0]
                 + res.m_complexes[2] * quat.m_real;

  return *this;
}

template <typename T>
constexpr bool Quaternion<T>::operator==(const Quaternion& quat) const noexcept {
  return FloatUtils::areNearlyEqual(*this, quat);
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Quaternion<T>& quat) {
  stream << "[ " << quat.w() << "; " << quat.x() << "; " << quat.y() << "; " << quat.z() << " ]";
  return stream;
}

} // namespace Raz
