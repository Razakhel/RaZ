#pragma once

#ifndef RAZ_CAMERA_HPP
#define RAZ_CAMERA_HPP

#include <memory>

#include "RaZ/Component.hpp"
#include "RaZ/Math/Constants.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

enum class ProjectionType : uint8_t {
  PERSPECTIVE = 0,
  ORTHOGRAPHIC
};

/// Camera class, simulating a point of view for a scene to be rendered from.
class Camera : public Component {
public:
  Camera(unsigned int frameWidth, unsigned int frameHeight,
         float fieldOfViewDegrees = 45.f,
         float nearPlane = 0.1f, float farPlane = 100.f,
         ProjectionType projType = ProjectionType::PERSPECTIVE);

  float getFieldOfViewDegrees() const { return m_fieldOfView * 180.f / PI<float>; }
  const Mat4f& getViewMatrix() const { return m_viewMat; }
  const Mat4f& getInverseViewMatrix() const { return m_invViewMat; }
  const Mat4f& getProjectionMatrix() const { return m_projMat; }
  const Mat4f& getInverseProjectionMatrix() const { return m_invProjMat; }

  void setFieldOfView(float fieldOfViewDegrees);
  void setProjectionType(ProjectionType projType);

  /// Standard view matrix computation.
  /// \param translationMatrix Translation matrix.
  /// \param inverseRotation Inverse rotation matrix.
  /// \return Reference to the computed view matrix.
  const Mat4f& computeViewMatrix(const Mat4f& translationMatrix, const Mat4f& inverseRotation);
  /// 'Look at' view matrix computation.
  /// \param position Position of the camera.
  /// \param target Position of where the camera's looking at.
  /// \param upDirection Vector pointing to the up direction relative to the current orientation.
  /// \return Reference to the computed view matrix.
  const Mat4f& computeLookAt(const Vec3f& position, const Vec3f& target = Vec3f(0.f), const Vec3f& upDirection = Axis::Y);
  /// Inverse view matrix computation.
  /// \return Reference to the computed inverse view matrix.
  const Mat4f& computeInverseViewMatrix();
  /// Projection matrix computation.
  /// According to projection's type, either perspective or orthographic will be computed.
  /// \return Reference to the computed perspective/orthographic matrix.
  const Mat4f& computeProjectionMatrix();
  /// Perspective projection matrix computation.
  /// \return Reference to the computed perspective projection matrix.
  const Mat4f& computePerspectiveMatrix();
  /// Orthographic projection matrix computation.
  /// \param right Right limit of the projection frustum.
  /// \param left Left limit of the projection frustum.
  /// \param top Top limit of the projection frustum.
  /// \param bottom Bottom limit of the projection frustum.
  /// \param near Near limit of the projection frustum.
  /// \param far Far limit of the projection frustum.
  /// \return Reference to the computed orthographic projection matrix.
  const Mat4f& computeOrthographicMatrix(float right, float left, float top, float bottom, float near, float far);
  /// Inverse projection matrix computation.
  /// According to projection's type, either perspective or orthographic will be computed.
  /// \return Reference to the computed inverse perspective/orthographic projection matrix.
  const Mat4f& computeInverseProjectionMatrix();

private:
  float m_frameRatio;
  float m_fieldOfView;
  float m_nearPlane;
  float m_farPlane;
  ProjectionType m_projType;

  Mat4f m_viewMat;
  Mat4f m_invViewMat;
  Mat4f m_projMat;
  Mat4f m_invProjMat;
};

} // namespace Raz

#endif // RAZ_CAMERA_HPP
