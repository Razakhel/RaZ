#include "RaZ/Math/Constants.hpp"

namespace Raz {

template <typename T>
Quaternion<T>::Quaternion(T angle, Vec3f axes) {
  const T halfAngle = (angle * pi<T> / 180) / 2;
  const T val = std::sin(halfAngle);

  m_real = std::cos(halfAngle);
  m_complexes = axes * val;
}

} // namespace Raz
