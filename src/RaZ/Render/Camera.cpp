#include "RaZ/Render/Camera.hpp"

namespace Raz {

Camera::Camera(unsigned int frameWidth, unsigned int frameHeight,
               float fieldOfViewDegrees,
               float nearPlane, float farPlane,
               const Vec3f& position) : m_frameRatio{ static_cast<float>(frameWidth) / static_cast<float>(frameHeight) },
                                        m_fieldOfView{ fieldOfViewDegrees * PI<float> / 180 },
                                        m_nearPlane{ nearPlane }, m_farPlane{ farPlane } {
  m_position = position;

  computeViewMatrix();
  computeInverseViewMatrix();
  computePerspectiveMatrix();
  computeInverseProjectionMatrix();
}

void Camera::setFieldOfView(float fieldOfViewDegrees) {
  m_fieldOfView = fieldOfViewDegrees  * PI<float> / 180;
  computePerspectiveMatrix();
}

void Camera::move(const Vec3f& displacement) {
  Transform::move(displacement);
  computeViewMatrix();
  computeInverseViewMatrix();
}

void Camera::translate(float x, float y, float z) {
  Transform::translate(x, y, z);
  computeViewMatrix();
  computeInverseViewMatrix();
}

void Camera::rotate(float angle, const Vec3f& axis) {
  Transform::rotate(angle, axis);
  computeViewMatrix();
  computeInverseViewMatrix();
}

const Mat4f& Camera::computeViewMatrix() {
  m_viewMat = m_rotation.inverse() * computeTranslationMatrix(true);
  return m_viewMat;
}

const Mat4f& Camera::computeInverseViewMatrix() {
  m_invViewMat = m_viewMat.inverse();
  return m_invViewMat;
}

const Mat4f& Camera::computeLookAt(const Vec3f& target, const Vec3f& orientation) {
  const Vec3f zAxis((m_position - target).normalize());
  const Vec3f xAxis(zAxis.cross(orientation).normalize());
  const Vec3f yAxis(xAxis.cross(zAxis));

  m_viewMat = Mat4f({{               xAxis[0],               yAxis[0],             -zAxis[0], 0.f },
                     {               xAxis[1],               yAxis[1],             -zAxis[1], 0.f },
                     {               xAxis[2],               yAxis[2],             -zAxis[2], 0.f },
                     { xAxis.dot(-m_position), yAxis.dot(-m_position), zAxis.dot(m_position), 1.f }});

  return m_viewMat;
}

const Mat4f& Camera::computeInverseProjectionMatrix() {
  m_invProjMat = m_projMat.inverse();
  return m_invProjMat;
}

const Mat4f& Camera::computePerspectiveMatrix() {
  const float halfFovTangent = std::tan(m_fieldOfView / 2.f);
  const float planeDist      = m_farPlane - m_nearPlane;
  const float planeMult      = m_farPlane * m_nearPlane;
  const float fovRatio       = m_frameRatio * halfFovTangent;

  m_projMat = Mat4f({{ 1 / fovRatio,                0.f,                    0.f, 0.f },
                     {          0.f, 1 / halfFovTangent,                    0.f, 0.f },
                     {          0.f,                0.f, m_farPlane / planeDist, 1.f },
                     {          0.f,                0.f, -planeMult / planeDist, 1.f }});

  return m_projMat;
}

} // namespace Raz
