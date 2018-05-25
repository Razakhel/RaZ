#pragma once

#ifndef RAZ_CAMERA_HPP
#define RAZ_CAMERA_HPP

#include <memory>

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Math/Constants.hpp"
#include "RaZ/Math/Transform.hpp"

namespace Raz {

class Camera : public Transform {
public:
  Camera(unsigned int frameWidth, unsigned int frameHeight,
         float fieldOfViewDegrees,
         float nearPlane, float farPlane,
         const Vec3f& position = Vec3f(0.f))
    : m_frameRatio{ static_cast<float>(frameWidth) / static_cast<float>(frameHeight) },
      m_fieldOfView{ fieldOfViewDegrees * PI<float> / 180 },
      m_nearPlane{ nearPlane },
      m_farPlane{ farPlane } { m_position = position; }

  Mat4f computeViewMatrix() const { return m_rotation.inverse() * computeTranslationMatrix(true); }
  Mat4f computeLookAt(const Vec3f& target = Vec3f(0.f), const Vec3f& orientation = Vec3f({ 0.f, 1.f, 0.f })) const;
  Mat4f computePerspectiveMatrix() const;

private:
  float m_frameRatio;
  float m_fieldOfView;
  float m_nearPlane;
  float m_farPlane;
};

using CameraPtr = std::unique_ptr<Camera>;

} // namespace Raz

#endif // RAZ_CAMERA_HPP
