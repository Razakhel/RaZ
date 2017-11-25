#include "RaZ/Render/Camera.hpp"

namespace Raz {

Mat4f Camera::lookAt(const Vec3f& target, const Vec3f& orientation) const {
  const Vec3f f((target - getPosition()).normalize());
  const Vec3f s(f.cross(orientation).normalize());
  const Vec3f u(s.cross(f));
  const Vec3f& pos = getPosition();

  const Mat4f res({{        s[0],        u[0],      -f[0], 0.f },
                   {        s[1],        u[1],      -f[1], 0.f },
                   {        s[2],        u[2],      -f[2], 0.f },
                   { -s.dot(pos), -u.dot(pos), f.dot(pos), 1.f }});

  return res;
}

Mat4f Camera::computeProjectionMatrix() const {
  const float halfFovTangent = std::tan(m_fieldOfView / 2);
  const float planeDist = m_farPlane - m_nearPlane;
  const float fovRatio = m_frameRatio * halfFovTangent;

  const Mat4f projMatrix({{ 1 / fovRatio,                  0.f,                                     0.f, 0.f },
                          {          0.f,   1 / halfFovTangent,                                     0.f, 0.f },
                          {          0.f,                  0.f,                  m_farPlane / planeDist, 1.f },
                          {          0.f,                  0.f, -(m_farPlane * m_nearPlane) / planeDist, 1.f }});

  return projMatrix;
}

} // namespace Raz
