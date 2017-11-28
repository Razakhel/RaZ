#include "RaZ/Render/Camera.hpp"

namespace Raz {

Mat4f Camera::lookAt(const Vec3f& target, const Vec3f& orientation) const {
  const Vec3f axisZ((target - m_position).normalize());
  const Vec3f axisX(axisZ.cross(orientation).normalize());
  const Vec3f axisY(axisX.cross(axisZ));

  const Mat4f res({{               axisX[0],               axisY[0],             -axisZ[0], 0.f },
                   {               axisX[1],               axisY[1],             -axisZ[1], 0.f },
                   {               axisX[2],               axisY[2],             -axisZ[2], 0.f },
                   { axisX.dot(-m_position), axisY.dot(-m_position), axisZ.dot(m_position), 1.f }});

  return res;
}

Mat4f Camera::computePerspectiveMatrix() const {
  const float halfFovTangent = std::tan(m_fieldOfView / 2);
  const float planeDist = m_farPlane - m_nearPlane;
  const float planeMult = m_farPlane * m_nearPlane;
  const float fovRatio = m_frameRatio * halfFovTangent;

  const Mat4f projMatrix({{ 1 / fovRatio,                0.f,                    0.f, 0.f },
                          {          0.f, 1 / halfFovTangent,                    0.f, 0.f },
                          {          0.f,                0.f, m_farPlane / planeDist, 1.f },
                          {          0.f,                0.f, -planeMult / planeDist, 1.f }});

  return projMatrix;
}

} // namespace Raz
