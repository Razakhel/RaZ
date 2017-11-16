#pragma once

#ifndef RAZ_LIGHT_HPP
#define RAZ_LIGHT_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Light {
public:
  const Vec4f& getPosition() const { return m_position; }
  Vec4f& getPosition() { return m_position; }
  const Vec3f& getDirection() const { return m_direction; }
  Vec3f& getDirection() { return m_direction; }
  const Vec3f& getColor() const { return m_color; }
  Vec3f& getColor() { return m_color; }
  float getAngle() const { return m_angle; }

protected:
  Light(const Vec4f& position, const Vec3f& color) : m_position(position), m_color(color) {}
  Light(const Vec4f& position, const Vec3f& direction, const Vec3f& color)
    : m_position(position), m_direction(direction), m_color(color) {}
  Light(const Vec4f& position, const Vec3f& direction, const Vec3f& color, float angle)
    : m_position(position), m_direction(direction), m_color(color), m_angle{ angle } {}

  Vec4f m_position {};
  Vec3f m_direction {};
  Vec3f m_color {};
  float m_angle = 0.f;
};

class PointLight : public Light {
public:
  PointLight(const Vec4f& position, const Vec3f& color) : Light(position, color) {}
};

class DirectionalLight : public Light {
public:
  DirectionalLight(const Vec4f& position, const Vec3f& direction, const Vec3f& color)
    : Light(position, direction, color) {}
};

class SpotLight : public Light {
public:
  SpotLight(const Vec4f& position, const Vec3f& direction, const Vec3f& color, float angle)
    : Light(position, direction, color, angle) {}
};

} // namespace Raz

#endif // RAZ_LIGHT_HPP
