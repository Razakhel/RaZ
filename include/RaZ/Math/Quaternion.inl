#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

namespace Raz {

template <typename T>
Quaternion<T>::Quaternion(T angle, Vec3f axes) : Quaternion() {
  const T halfAngle = (angle * static_cast<T>(M_PI / 180)) / 2;
  const T val = std::sin(halfAngle);

  m_real = std::cos(halfAngle);
  m_complexes = axes * val;
}

} // namespace Raz
