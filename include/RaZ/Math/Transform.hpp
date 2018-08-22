#pragma once

#ifndef RAZ_TRANSFORM_HPP
#define RAZ_TRANSFORM_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Transform {
public:
  explicit Transform(const Vec3f& position = Vec3f(0.f), const Mat4f& rotation = Mat4f::identity(), const Vec3f& scale = Vec3f(1.f))
    : m_position{ position }, m_rotation{ rotation }, m_scale{ scale } {}

  const Vec3f& getPosition() const { return m_position; }
  Vec3f& getPosition() { return m_position; }
  const Mat4f& getRotation() const { return m_rotation; }
  Mat4f& getRotation() { return m_rotation; }
  const Vec3f& getScale() const { return m_scale; }
  Vec3f& getScale() { return m_scale; }

  void setPosition(const Vec3f& position) { m_position = position; }
  void setPosition(float x, float y, float z) { setPosition(Vec3f({ x, y, z })); }
  void setRotation(const Mat4f& rotation) { m_rotation = rotation; }
  void setScale(const Vec3f& scale) { m_scale = scale; }
  void setScale(float val) { setScale(val, val, val); }
  void setScale(float x, float y, float z) { setScale(Vec3f({ x, y, z })); }

  virtual void move(float x, float y, float z) { move(Vec3f({ x, y, z })); }
  virtual void move(const Vec3f& displacement) { translate(displacement * Mat3f(m_rotation)); }
  virtual void translate(float x, float y, float z);
  virtual void translate(const Vec3f& values) { translate(values[0], values[1], values[2]); }
  virtual void rotate(float angle, const Vec3f& axis);
  virtual void scale(float x, float y, float z);
  virtual void scale(float val) { scale(val, val, val); }
  virtual void scale(const Vec3f& values) { scale(values[0], values[1], values[2]); }
  Mat4f computeTranslationMatrix(bool inverseTranslation = false) const;
  Mat4f computeTransformMatrix() const;

  virtual ~Transform() = default;

protected:
  Vec3f m_position;
  Mat4f m_rotation;
  Vec3f m_scale;
};

} // namespace Raz

#endif // RAZ_TRANSFORM_HPP
