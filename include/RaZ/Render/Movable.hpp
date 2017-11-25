#pragma once

#ifndef RAZ_MOVABLE_HPP
#define RAZ_MOVABLE_HPP

namespace Raz {

class Movable {
public:
  const Vec3f& getPosition() const { return m_transform.getPosition(); }
  const Mat4f& getRotation() const { return m_transform.getRotation(); }
  const Vec3f& getScale() const { return m_transform.getScale(); }
  const Transform& getTransform() const { return m_transform; }
  Transform& getTransform() { return m_transform; }

  virtual void translate(float x, float y, float z) { m_transform.translate(x, y, z); }
  virtual void rotate(float angle, float x, float y, float z) { m_transform.rotate(angle, x, y, z); }
  virtual void scale(float x, float y, float z) { m_transform.scale(x, y, z); }
  virtual void scale(float val) { m_transform.scale(val, val, val); }

protected:
  Movable() = default;

  Transform m_transform {};
};

} // namespace Raz

#endif // RAZ_MOVABLE_HPP
