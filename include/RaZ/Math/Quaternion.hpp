#pragma once

#ifndef RAZ_QUATERNION_HPP
#define RAZ_QUATERNION_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

template <typename T = float>
class Quaternion {
  static_assert(("Error: Quaternion's type must be floating point.", std::is_floating_point<T>::value));

public:
  Quaternion(T angle, Vec3f axes);
  Quaternion(T angle, float axisX, float axisY, float axisZ) : Quaternion(angle, Vec3f({ axisX, axisY, axisZ })) {}

  Mat4f computeMatrix() const;

private:
  T m_real {};
  Vector<T, 3> m_complexes {};
};

} // namespace Raz

#include "RaZ/Math/Quaternion.inl"

#endif // RAZ_QUATERNION_HPP
