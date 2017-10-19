namespace Raz {

template <typename T>
Quaternion<T>::Quaternion(T angle, Vec3f axes) : Quaternion() {
  const T halfAngle = angle / 2;
  const T val = std::sin(halfAngle);

  m_real = std::cos(halfAngle);
  m_complexes = axes * val;
}

} // namespace Raz
