#pragma once

#ifndef RAZ_CAMERA_HPP
#define RAZ_CAMERA_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Constants.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

#include <memory>

namespace Raz {

class Transform;

enum class CameraType : uint8_t {
  FREE_FLY = 0,
  LOOK_AT
};

enum class ProjectionType : uint8_t {
  PERSPECTIVE = 0,
  ORTHOGRAPHIC
};

/// Camera simulating a point of view for a scene to be rendered from.
class Camera final : public Component {
public:
  Camera() = default;
  Camera(unsigned int frameWidth, unsigned int frameHeight,
         Radiansf fieldOfView = Degreesf(45.f),
         float nearPlane = 0.1f, float farPlane = 1000.f,
         ProjectionType projType = ProjectionType::PERSPECTIVE);

  Radiansf getFieldOfView() const { return m_fieldOfView; }
  float getOrthographicBound() const { return m_orthoBound; }
  CameraType getCameraType() const { return m_cameraType; }
  const Mat4f& getViewMatrix() const { return m_viewMat; }
  const Mat4f& getInverseViewMatrix() const { return m_invViewMat; }
  const Mat4f& getProjectionMatrix() const { return m_projMat; }
  const Mat4f& getInverseProjectionMatrix() const { return m_invProjMat; }

  void setFieldOfView(Radiansf fieldOfView);
  void setOrthographicBound(float bound);
  void setCameraType(CameraType camType) { m_cameraType = camType; }
  void setProjectionType(ProjectionType projType);
  void setTarget(const Vec3f& target) { m_target = target; }

  /// Computes the standard "free fly" view matrix.
  /// \param cameraTransform Transform component of the camera.
  /// \return Reference to the computed view matrix.
  const Mat4f& computeViewMatrix(const Transform& cameraTransform);
  /// Computes the "look at" view matrix.
  /// \param position Position of the camera.
  /// \return Reference to the computed view matrix.
  const Mat4f& computeLookAt(const Vec3f& position);
  /// Computes the inverse view matrix.
  /// \return Reference to the computed inverse view matrix.
  const Mat4f& computeInverseViewMatrix();
  /// Computes the perspective projection matrix.
  /// \return Reference to the computed perspective projection matrix.
  const Mat4f& computePerspectiveMatrix();
  /// Computes the orthographic projection matrix.
  /// \param minX Minimum X limit of the projection frustum.
  /// \param maxX Maximum X limit of the projection frustum.
  /// \param minY Minimum Y limit of the projection frustum.
  /// \param maxY Maximum Y limit of the projection frustum.
  /// \param minZ Minimum Z limit of the projection frustum.
  /// \param maxZ Maximum Z limit of the projection frustum.
  /// \return Reference to the computed orthographic projection matrix.
  const Mat4f& computeOrthographicMatrix(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
  /// Computes the orthographic projection matrix using the camera's orthographic bounds.
  /// \see setOrthographicBound()
  /// \return Reference to the computed orthographic projection matrix.
  const Mat4f& computeOrthographicMatrix();
  /// Computes the projection matrix.
  /// Depending on the projection type, either perspective or orthographic will be computed.
  /// \return Reference to the computed projection matrix.
  const Mat4f& computeProjectionMatrix();
  /// Computes the inverse projection matrix.
  /// Depending on the projection type, either perspective or orthographic will be computed.
  /// \return Reference to the computed inverse projection matrix.
  const Mat4f& computeInverseProjectionMatrix();
  /// Resizes the viewport.
  /// Resizing the viewport recomputes the projection matrix.
  /// \param frameWidth Viewport width.
  /// \param frameHeight Viewport height.
  void resizeViewport(unsigned int frameWidth, unsigned int frameHeight);
  /// Unprojects to world space the given 3D point in homogeneous coordinates.
  /// \param point Point to unproject.
  /// \return Given point in world space.
  constexpr Vec3f unproject(const Vec4f& point) const noexcept(std::numeric_limits<float>::is_iec559) {
    Vec4f viewSpacePoint = m_invProjMat * point;
    viewSpacePoint      /= viewSpacePoint.w();

    return Vec3f(m_invViewMat * viewSpacePoint);
  }
  /// Unprojects to world space the given 3D point.
  /// \param point Point to unproject.
  /// \return Given point in world space.
  constexpr Vec3f unproject(const Vec3f& point) const noexcept(std::numeric_limits<float>::is_iec559) { return unproject(Vec4f(point, 1.f)); }
  /// Unprojects to world space the given 2D coordinates.
  /// \param point Point to unproject. Its values are supposed to be between -1 & 1.
  /// \return Given point in world space.
  constexpr Vec3f unproject(const Vec2f& point) const noexcept(std::numeric_limits<float>::is_iec559) { return unproject(Vec3f(point, 0.f)); }

private:
  float m_frameRatio     = 1.f;
  Radiansf m_fieldOfView = Degreesf(45.f);
  float m_nearPlane      = 0.1f;
  float m_farPlane       = 1000.f;
  float m_orthoBound     = 1.f;

  CameraType m_cameraType   = CameraType::FREE_FLY;
  ProjectionType m_projType = ProjectionType::PERSPECTIVE;

  Vec3f m_target = Vec3f(0.f);
  Vec3f m_upAxis = Axis::Y;

  Mat4f m_viewMat    = Mat4f::identity();
  Mat4f m_invViewMat = Mat4f::identity();
  Mat4f m_projMat    = Mat4f::identity();
  Mat4f m_invProjMat = Mat4f::identity();
};

} // namespace Raz

#endif // RAZ_CAMERA_HPP
