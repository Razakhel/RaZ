#include "RaZ/Render/Camera.hpp"

namespace Raz {

Mat4f Camera::computePerspectiveMatrix() const {
  const float halfFovTangent = std::tan(m_fieldOfView / 2.f);
  const float planeDist = m_farPlane - m_nearPlane;
  const float planeMult = m_farPlane * m_nearPlane;
  const float fovRatio = m_frameRatio * halfFovTangent;

  const Mat4f projMatrix({{ 1 / fovRatio,                0.f,                    0.f, 0.f },
                          {          0.f, 1 / halfFovTangent,                    0.f, 0.f },
                          {          0.f,                0.f, m_farPlane / planeDist, 1.f },
                          {          0.f,                0.f, -planeMult / planeDist, 1.f }});

  return projMatrix;
}

Mat4f Camera::lookAt(const Vec3f& target, const Vec3f& orientation) const {
  const Vec3f zAxis((m_position - target).normalize());
  const Vec3f xAxis(zAxis.cross(orientation).normalize());
  const Vec3f yAxis(xAxis.cross(zAxis));

  const Mat4f res({{               xAxis[0],               yAxis[0],             -zAxis[0], 0.f },
                   {               xAxis[1],               yAxis[1],             -zAxis[1], 0.f },
                   {               xAxis[2],               yAxis[2],             -zAxis[2], 0.f },
                   { xAxis.dot(-m_position), yAxis.dot(-m_position), zAxis.dot(m_position), 1.f }});

  return res;
}

} // namespace Raz
