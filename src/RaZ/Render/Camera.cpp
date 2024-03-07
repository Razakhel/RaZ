#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

Camera::Camera(unsigned int frameWidth, unsigned int frameHeight,
               Radiansf fieldOfView,
               float nearPlane, float farPlane,
               ProjectionType projType) : m_frameRatio{ static_cast<float>(frameWidth) / static_cast<float>(frameHeight) },
                                          m_fieldOfView{ fieldOfView },
                                          m_nearPlane{ nearPlane }, m_farPlane{ farPlane },
                                          m_projType{ projType } {
  ZoneScopedN("Camera::Camera");

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

void Camera::setFieldOfView(Radiansf fieldOfView) {
  ZoneScopedN("Camera::setFieldOfView");

  m_fieldOfView = fieldOfView;

  if (m_projType == ProjectionType::PERSPECTIVE) {
    computeProjectionMatrix();
    computeInverseProjectionMatrix();
  }
}

void Camera::setOrthographicBound(float bound) {
  ZoneScopedN("Camera::setOrthographicBound");

  m_orthoBound = bound;

  if (m_projType == ProjectionType::ORTHOGRAPHIC) {
    computeProjectionMatrix();
    computeInverseProjectionMatrix();
  }
}

void Camera::setProjectionType(ProjectionType projType) {
  ZoneScopedN("Camera::setProjectionType");

  if (projType == m_projType)
    return; // No need to recompute the projection matrix

  m_projType = projType;

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

const Mat4f& Camera::computeViewMatrix(const Transform& cameraTransform) {
  ZoneScopedN("Camera::computeViewMatrix");

  // TODO: the rotation quaternion being supposedly a unit one, the inversion could be replaced by a conjugation
  m_viewMat = cameraTransform.getRotation().inverse().computeMatrix() * cameraTransform.computeTranslationMatrix(true);
  return m_viewMat;
}

const Mat4f& Camera::computeLookAt(const Vec3f& position) {
  ZoneScopedN("Camera::computeLookAt");

  const Vec3f zAxis = (position - m_target).normalize();
  const Vec3f xAxis = m_upAxis.cross(zAxis).normalize();
  const Vec3f yAxis = zAxis.cross(xAxis);

  m_viewMat = Mat4f(xAxis.x(), xAxis.y(), xAxis.z(), -xAxis.dot(position),
                    yAxis.x(), yAxis.y(), yAxis.z(), -yAxis.dot(position),
                    zAxis.x(), zAxis.y(), zAxis.z(), -zAxis.dot(position),
                    0.f,       0.f,       0.f,        1.f);

  return m_viewMat;
}

const Mat4f& Camera::computeInverseViewMatrix() {
  ZoneScopedN("Camera::computeInverseViewMatrix");

  m_invViewMat = m_viewMat.inverse();
  return m_invViewMat;
}

const Mat4f& Camera::computePerspectiveMatrix() {
  ZoneScopedN("Camera::computePerspectiveMatrix");

  const float halfFovTangent = std::tan(m_fieldOfView.value * 0.5f);
  const float fovRatio       = m_frameRatio * halfFovTangent;
  const float planeMult      = m_farPlane * m_nearPlane;
  const float invDist        = 1.f / (m_farPlane - m_nearPlane);

  m_projMat = Mat4f(1.f / fovRatio, 0.f,                   0.f,                                   0.f,
                    0.f,            1.f / halfFovTangent,  0.f,                                   0.f,
                    0.f,            0.f,                  -(m_farPlane + m_nearPlane) * invDist, -2.f * planeMult * invDist,
                    0.f,            0.f,                  -1.f,                                   0.f);

  return m_projMat;
}

const Mat4f& Camera::computeOrthographicMatrix(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
  ZoneScopedN("Camera::computeOrthographicMatrix");

  const float invDistX = 1.f / (maxX - minX);
  const float invDistY = 1.f / (maxY - minY);
  const float invDistZ = 1.f / (maxZ - minZ);

  m_projMat = Mat4f(2.f * invDistX, 0.f,             0.f,            -(maxX + minX) * invDistX,
                    0.f,            2.f * invDistY,  0.f,            -(maxY + minY) * invDistY,
                    0.f,            0.f,            -2.f * invDistZ, -(maxZ + minZ) * invDistZ,
                    0.f,            0.f,             0.f,             1.f);

  return m_projMat;
}

const Mat4f& Camera::computeOrthographicMatrix() {
  const float orthoRatio = m_orthoBound * m_frameRatio;
  return computeOrthographicMatrix(-orthoRatio, orthoRatio, -m_orthoBound, m_orthoBound, -m_farPlane, m_farPlane);
}

const Mat4f& Camera::computeProjectionMatrix() {
  ZoneScopedN("Camera::computeProjectionMatrix");

  if (m_projType == ProjectionType::ORTHOGRAPHIC)
    return computeOrthographicMatrix();

  return computePerspectiveMatrix();
}

const Mat4f& Camera::computeInverseProjectionMatrix() {
  ZoneScopedN("Camera::computeInverseProjectionMatrix");

  m_invProjMat = m_projMat.inverse();
  return m_invProjMat;
}

void Camera::resizeViewport(unsigned int frameWidth, unsigned int frameHeight) {
  ZoneScopedN("Camera::resizeViewport");

  const float newRatio = static_cast<float>(frameWidth) / static_cast<float>(frameHeight);

  if (newRatio == m_frameRatio)
    return; // No need to recompute the projection matrix

  m_frameRatio = newRatio;

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

} // namespace Raz
