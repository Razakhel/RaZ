#pragma once

#ifndef RAZ_TRANSFORM_HPP
#define RAZ_TRANSFORM_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

template <typename T, std::size_t W, std::size_t H>
class Matrix;
using Mat4f = Matrix<float, 4, 4>;

template <typename T>
struct Radians;
using Radiansf = Radians<float>;

/// Transform class which handles 3D transformations (translation/rotation/scale).
class Transform final : public Component {
public:
  explicit Transform(const Vec3f& position = Vec3f(0.f), const Quaternionf& rotation = Quaternionf::identity(), const Vec3f& scale = Vec3f(1.f)) noexcept
    : m_position{ position }, m_rotation{ rotation }, m_scale{ scale } {}

  const Vec3f& getPosition() const { return m_position; }
  const Quaternionf& getRotation() const { return m_rotation; }
  const Vec3f& getScale() const { return m_scale; }
  bool hasUpdated() const { return m_updated; }

  void setPosition(const Vec3f& position);
  void setPosition(float x, float y, float z) { setPosition(Vec3f(x, y, z)); }
  void setRotation(const Quaternionf& rotation);
  void setRotation(Radiansf angle, const Vec3f& axis);
  void setScale(const Vec3f& scale);
  void setScale(float x, float y, float z) { setScale(Vec3f(x, y, z)); }
  void setScale(float val) { setScale(val, val, val); }
  void setUpdated(bool updated) { m_updated = updated; }

  /// Moves by the given values in relative coordinates (takes rotation into account).
  /// \param displacement Displacement to be moved by.
  void move(const Vec3f& displacement) { translate(m_rotation * displacement); }
  /// Moves by the given values in relative coordinates (takes rotation into account).
  /// \param x Value of X to be moved by.
  /// \param y Value of Y to be moved by.
  /// \param z Value of Z to be moved by.
  void move(float x, float y, float z) { move(Vec3f(x, y, z)); }
  /// Translates by the given values in absolute coordinates (does not take rotation into account).
  /// \param displacement Displacement to be translated by.
  void translate(const Vec3f& displacement) { translate(displacement.x(), displacement.y(), displacement.z()); }
  /// Translates by the given values in absolute coordinates (does not take rotation into account).
  /// \param x Value of X to be translated by.
  /// \param y Value of Y to be translated by.
  /// \param z Value of Z to be translated by.
  void translate(float x, float y, float z);
  /// Rotates by the given quaternion.
  /// \param rotation Rotation quaternion to rotate by.
  void rotate(const Quaternionf& rotation);
  /// Rotates by the given angle around the given axis.
  /// The axis must be a normalized vector for this function to work properly.
  /// \param angle Angle to rotate by.
  /// \param axis Axis to rotate around.
  void rotate(Radiansf angle, const Vec3f& axis);
  /// Rotates around the given axes. This locks the up vector, preventing any rolling effect.
  /// \param xAngle Value of X to rotate around.
  /// \param yAngle Value of Y to rotate around.
  void rotate(Radiansf xAngle, Radiansf yAngle);
  /// Rotates around the given axes.
  /// \param xAngle Value of X to rotate around.
  /// \param yAngle Value of Y to rotate around.
  /// \param zAngle Value of Z to rotate around.
  void rotate(Radiansf xAngle, Radiansf yAngle, Radiansf zAngle);
  /// Scales by the given values.
  /// The scaling is a coefficient: scaling by a value of 2 doubles the size, while a value of 0.5 shrinks it by half.
  /// \param values Values to be scaled by.
  void scale(const Vec3f& values) { scale(values.x(), values.y(), values.z()); }
  /// Scales by the given values.
  /// The scaling is a coefficient: scaling by a value of 2 doubles the size, while a value of 0.5 shrinks it by half.
  /// \param x Value of X to be scaled by.
  /// \param y Value of X to be scaled by.
  /// \param z Value of X to be scaled by.
  void scale(float x, float y, float z);
  /// Scales by the given value in all 3 coordinates.
  /// The scaling is a coefficient: scaling by a value of 2 doubles the size, while a value of 0.5 shrinks it by half.
  /// \param val Value to be scaled by.
  void scale(float val) { scale(val, val, val); }
  /// Computes the translation matrix (identity matrix with the translation in the last row).
  /// \param reverseTranslation True if the translation should be reversed (negated), false otherwise.
  /// \return Translation matrix.
  Mat4f computeTranslationMatrix(bool reverseTranslation = false) const;
  /// Computes the transformation matrix.
  /// This matrix combines all three features: translation, rotation & scale.
  /// \return Transformation matrix.
  Mat4f computeTransformMatrix() const;

private:
  Vec3f m_position {};
  Quaternionf m_rotation = Quaternionf::identity();
  Vec3f m_scale = Vec3f(1.f);
  bool m_updated = true;
};

} // namespace Raz

#endif // RAZ_TRANSFORM_HPP
