#pragma once

#ifndef RAZ_LIGHT_HPP
#define RAZ_LIGHT_HPP

#include <memory>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Math/Transform.hpp"

namespace Raz {

class Light : public Transform {
public:
  virtual Vec4f getHomogeneousPosition() const = 0;
  const Vec3f& getDirection() const { return m_direction; }
  Vec3f& getDirection() { return m_direction; }
  const Vec3f& getColor() const { return m_color; }
  Vec3f& getColor() { return m_color; }
  float getAngle() const { return m_angle; }

protected:
  Light(const Vec3f& position, const Vec3f& color) : m_color{ color } { m_position = position; }
  Light(const Vec3f& position, const Vec3f& direction, const Vec3f& color)
    : m_direction{ direction }, m_color{ color } { m_position = position; }
  Light(const Vec3f& position, const Vec3f& direction, const Vec3f& color, float angle)
    : m_direction{ direction }, m_color{ color }, m_angle{ angle } { m_position = position; }

  Vec3f m_direction {};
  Vec3f m_color {};
  float m_angle = 0.f;
};

using LightPtr = std::unique_ptr<Light>;

class PointLight : public Light {
public:
  PointLight(const Vec3f& position, const Vec3f& color) : Light(position, color) {}

  Vec4f getHomogeneousPosition() const override { return Vec4f(m_position, 1.f); }
};

class DirectionalLight : public Light {
public:
  DirectionalLight(const Vec3f& position, const Vec3f& direction, const Vec3f& color) : Light(position, direction, color) {}

  Vec4f getHomogeneousPosition() const override { return Vec4f(m_position, 0.f); }
};

class SpotLight : public Light {
public:
  SpotLight(const Vec3f& position, const Vec3f& direction, const Vec3f& color, float angle)
    : Light(position, direction, color, angle) {}

  Vec4f getHomogeneousPosition() const override { return Vec4f(m_position, 1.f); }
};

} // namespace Raz

#endif // RAZ_LIGHT_HPP
