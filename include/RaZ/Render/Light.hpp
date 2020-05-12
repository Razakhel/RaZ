#pragma once

#ifndef RAZ_LIGHT_HPP
#define RAZ_LIGHT_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Math/Transform.hpp"

namespace Raz {

enum class LightType {
  POINT = 0,
  DIRECTIONAL,
  SPOT
};

class Light final : public Component {
public:
  Light(LightType type, float energy, const Vec3f& color = Vec3f(1.f))
    : m_type{ type }, m_energy{ energy }, m_color{ color } {}
  Light(LightType type, const Vec3f& direction, float energy, const Vec3f& color = Vec3f(1.f))
    : m_type{ type }, m_direction{ direction }, m_energy{ energy }, m_color{ color } {}
  Light(LightType type, const Vec3f& direction, float energy, float angle, const Vec3f& color = Vec3f(1.f))
    : m_type{ type }, m_direction{ direction }, m_energy{ energy }, m_color{ color }, m_angle{ angle } {}

  LightType getType() const { return m_type; }
  const Vec3f& getDirection() const { return m_direction; }
  float getEnergy() const { return m_energy; }
  const Vec3f& getColor() const { return m_color; }
  float getAngle() const { return m_angle; }

  void setType(LightType type) { m_type = type; }
  void setDirection(const Vec3f& direction) { m_direction = direction; }
  void setEnergy(float energy) { m_energy = energy; }
  void setColor(const Vec3f& color) { m_color = color; }
  void setAngle(float angle) { m_angle = angle; }

private:
  LightType m_type {};
  Vec3f m_direction {};
  float m_energy = 1.f;
  Vec3f m_color {};
  float m_angle  = 0.f;
};

} // namespace Raz

#endif // RAZ_LIGHT_HPP
