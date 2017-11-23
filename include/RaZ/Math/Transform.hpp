#pragma once

#ifndef RAZ_TRANSFORM_HPP
#define RAZ_TRANSFORM_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Transform {
public:
  Transform() = default;

  void translate(float x, float y, float z);
  void rotate(float angle, float x, float y, float z);
  void scale(float x, float y, float z);
  Mat4f computeTransformMatrix() const;

private:
  Vec3f m_position {};
  Mat4f m_rotation = Mat4f::identity();
  Vec3f m_scale = Vec3f(1.f);
};

} // namespace Raz

#endif // RAZ_TRANSFORM_HPP
