#pragma once

#ifndef RAZ_QUATERNION_HPP
#define RAZ_QUATERNION_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

template <typename T = float>
class Quaternion {
  static_assert(std::is_floating_point<T>::value, "Error: Quaternion's type must be floating point.");

public:
  Quaternion(T angle, const Vec3<T>& axes);
  Quaternion(T angle, float axisX, float axisY, float axisZ) : Quaternion(angle, Vec3<T>({ axisX, axisY, axisZ })) {}

  Mat4<T> computeMatrix() const;

private:
  T m_real {};
  Vec3<T> m_complexes {};
};

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

} // namespace Raz

#include "RaZ/Math/Quaternion.inl"

#endif // RAZ_QUATERNION_HPP
