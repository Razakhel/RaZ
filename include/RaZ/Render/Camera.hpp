#pragma once

#ifndef RAZ_CAMERA_HPP
#define RAZ_CAMERA_HPP

#include <memory>

#include "RaZ/Component.hpp"
#include "RaZ/Math/Constants.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Camera;
using CameraPtr = std::unique_ptr<Camera>;

class Camera : public Component {
public:
  Camera(unsigned int frameWidth, unsigned int frameHeight,
         float fieldOfViewDegrees = 45.f,
         float nearPlane = 0.1f, float farPlane = 100.f);

  float getFieldOfViewDegrees() const { return m_fieldOfView * 180.f / PI<float>; }
  const Mat4f& getViewMatrix() const { return m_viewMat; }
  const Mat4f& getInverseViewMatrix() const { return m_invViewMat; }
  const Mat4f& getProjectionMatrix() const { return m_projMat; }
  const Mat4f& getInverseProjectionMatrix() const { return m_invProjMat; }

  void setFieldOfView(float fieldOfViewDegrees);

  template <typename... Args> static CameraPtr create(Args&&... args) { return std::make_unique<Camera>(std::forward<Args>(args)...); }

  const Mat4f& computeViewMatrix(const Mat4f& inverseRotation, const Mat4f& translationMatrix);
  const Mat4f& computeInverseViewMatrix();
  const Mat4f& computeLookAt(const Vec3f& position, const Vec3f& target = Vec3f(0.f), const Vec3f& orientation = Axis::Y);
  const Mat4f& computeInverseProjectionMatrix();
  const Mat4f& computePerspectiveMatrix();

private:
  float m_frameRatio;
  float m_fieldOfView;
  float m_nearPlane;
  float m_farPlane;

  Mat4f m_viewMat;
  Mat4f m_invViewMat;
  Mat4f m_projMat;
  Mat4f m_invProjMat;
};

} // namespace Raz

#endif // RAZ_CAMERA_HPP
