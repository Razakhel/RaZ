#include "RaZ/Render/Camera.hpp"

namespace Raz {

Camera::Camera(unsigned int frameWidth, unsigned int frameHeight,
               float fieldOfViewDegrees,
               float nearPlane, float farPlane,
               ProjectionType projType) : m_frameRatio{ static_cast<float>(frameWidth) / frameHeight },
                                          m_fieldOfView{ fieldOfViewDegrees * PI<float> / 180.f },
                                          m_nearPlane{ nearPlane }, m_farPlane{ farPlane },
                                          m_projType{ projType } {
  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

void Camera::setFieldOfView(float fieldOfViewDegrees) {
  m_fieldOfView = fieldOfViewDegrees * PI<float> / 180.f;

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

void Camera::setProjectionType(ProjectionType projType) {
  m_projType = projType;

  computeProjectionMatrix();
  computeInverseProjectionMatrix();
}

const Mat4f& Camera::computeViewMatrix(const Mat4f& translationMatrix, const Mat4f& inverseRotation) {
  m_viewMat = translationMatrix * inverseRotation;
  return m_viewMat;
}

const Mat4f& Camera::computeLookAt(const Vec3f& position, const Vec3f& target, const Vec3f& upDirection) {
  const Vec3f zAxis((position - target).normalize());
  const Vec3f xAxis(zAxis.cross(upDirection).normalize());
  const Vec3f yAxis(xAxis.cross(zAxis));

  m_viewMat = Mat4f({{             xAxis[0],             yAxis[0],           -zAxis[0], 0.f },
                     {             xAxis[1],             yAxis[1],           -zAxis[1], 0.f },
                     {             xAxis[2],             yAxis[2],           -zAxis[2], 0.f },
                     { xAxis.dot(-position), yAxis.dot(-position), zAxis.dot(position), 1.f }});

  return m_viewMat;
}

const Mat4f& Camera::computeInverseViewMatrix() {
  m_invViewMat = m_viewMat.inverse();
  return m_invViewMat;
}

const Mat4f& Camera::computePerspectiveMatrix() {
  const float halfFovTangent = std::tan(m_fieldOfView / 2.f);
  const float planeDist      = m_farPlane - m_nearPlane;
  const float planeMult      = m_farPlane * m_nearPlane;
  const float fovRatio       = m_frameRatio * halfFovTangent;

  m_projMat = Mat4f({{ 1 / fovRatio,                0.f,                    0.f, 0.f },
                     {          0.f, 1 / halfFovTangent,                    0.f, 0.f },
                     {          0.f,                0.f, m_farPlane / planeDist, 1.f },
                     {          0.f,                0.f, -planeMult / planeDist, 0.f }});

  return m_projMat;
}

const Mat4f& Camera::computeOrthographicMatrix(float right, float left, float top, float bottom, float near, float far) {
  const float xDist = right - left;
  const float yDist = top - bottom;
  const float zDist = far - near;

  m_projMat = Mat4f({{ 2.f / xDist,         0.f,          0.f, -(right + left) / xDist },
                     {         0.f, 2.f / yDist,          0.f, -(top + bottom) / yDist },
                     {         0.f,         0.f, -2.f / zDist,   -(far + near) / zDist },
                     {         0.f,         0.f,          0.f,                     1.f }});

  return m_projMat;
}

const Mat4f& Camera::computeProjectionMatrix() {
  if (m_projType == ProjectionType::ORTHOGRAPHIC)
    return computeOrthographicMatrix(1.f, -1.f, -1.f, 1.f, m_nearPlane, m_farPlane);

  return computePerspectiveMatrix();
}

const Mat4f& Camera::computeInverseProjectionMatrix() {
  m_invProjMat = m_projMat.inverse();
  return m_invProjMat;
}

} // namespace Raz
