#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"

namespace Raz {

Camera::Camera(unsigned int frameWidth, unsigned int frameHeight,
               Radiansf fieldOfView,
               float nearPlane, float farPlane,
               ProjectionType projType) : m_frameRatio{ static_cast<float>(frameWidth) / static_cast<float>(frameHeight) },
                                          m_fieldOfView{ fieldOfView },
                                          m_nearPlane{ nearPlane }, m_farPlane{ farPlane },
                                          m_projType{ projType } {
  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

void Camera::setFieldOfView(Radiansf fieldOfView) {
  m_fieldOfView = fieldOfView;

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

void Camera::setOrthoBoundX(float boundX) {
  m_orthoBoundX = boundX;

  if (m_projType == ProjectionType::ORTHOGRAPHIC) {
    computeProjectionMatrix();
    computeInverseProjectionMatrix();
  }
}

void Camera::setOrthoBoundY(float boundY) {
  m_orthoBoundY = boundY;

  if (m_projType == ProjectionType::ORTHOGRAPHIC) {
    computeProjectionMatrix();
    computeInverseProjectionMatrix();
  }
}

void Camera::setProjectionType(ProjectionType projType) {
  m_projType = projType;

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

const Mat4f& Camera::computeViewMatrix(const Transform& cameraTransform) {
  m_viewMat = cameraTransform.getRotation().computeMatrix() * cameraTransform.computeTranslationMatrix(true);
  return m_viewMat;
}

const Mat4f& Camera::computeLookAt(const Vec3f& position) {
  const Vec3f zAxis = (position - m_target).normalize();
  const Vec3f xAxis = zAxis.cross(m_upAxis).normalize();
  const Vec3f yAxis = xAxis.cross(zAxis);

  m_viewMat = Mat4f( xAxis.x(),  xAxis.y(),  xAxis.z(), -xAxis.dot(position),
                     yAxis.x(),  yAxis.y(),  yAxis.z(), -yAxis.dot(position),
                    -zAxis.x(), -zAxis.y(), -zAxis.z(),  zAxis.dot(position),
                     0.f,        0.f,        0.f,        1.f);

  return m_viewMat;
}

const Mat4f& Camera::computeInverseViewMatrix() {
  m_invViewMat = m_viewMat.inverse();
  return m_invViewMat;
}

const Mat4f& Camera::computePerspectiveMatrix() {
  const float halfFovTangent = std::tan(m_fieldOfView.value * 0.5f);
  const float fovRatio       = m_frameRatio * halfFovTangent;
  const float planeMult      = m_farPlane * m_nearPlane;
  const float invDist        = 1.f / (m_farPlane - m_nearPlane);

  m_projMat = Mat4f(1.f / fovRatio, 0.f,                   0.f,                   0.f,
                    0.f,            1.f / halfFovTangent,  0.f,                   0.f,
                    0.f,            0.f,                   m_farPlane * invDist, -planeMult * invDist,
                    0.f,            0.f,                   1.f,                   0.f);

  return m_projMat;
}

const Mat4f& Camera::computeOrthographicMatrix(float right, float left, float top, float bottom, float near, float far) {
  const float xDist = right - left;
  const float yDist = top - bottom;
  const float zDist = far - near;

  m_projMat = Mat4f( 2.f / xDist,             0.f,                     0.f,                  0.f,
                     0.f,                     2.f / yDist,             0.f,                  0.f,
                     0.f,                     0.f,                    -2.f / zDist,          0.f,
                    -(right + left) / xDist, -(top + bottom) / yDist, -(far + near) / zDist, 1.f);

  return m_projMat;
}

const Mat4f& Camera::computeProjectionMatrix() {
  if (m_projType == ProjectionType::ORTHOGRAPHIC) {
    const float orthoX = m_orthoBoundX * m_frameRatio;
    return computeOrthographicMatrix(orthoX, -orthoX, m_orthoBoundY, -m_orthoBoundY, m_nearPlane, m_farPlane);
  }

  return computePerspectiveMatrix();
}

const Mat4f& Camera::computeInverseProjectionMatrix() {
  m_invProjMat = m_projMat.inverse();
  return m_invProjMat;
}

void Camera::resizeViewport(unsigned int frameWidth, unsigned int frameHeight) {
  m_frameRatio = static_cast<float>(frameWidth) / static_cast<float>(frameHeight);

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

} // namespace Raz
