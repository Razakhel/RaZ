#pragma once

#ifndef RAZ_LIGHT_HPP
#define RAZ_LIGHT_HPP

#include <memory>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Math/Transform.hpp"

namespace Raz {

enum class LightType {
  POINT = 0,
  DIRECTIONAL,
  SPOT
};

class Light : public Component {
public:
  Light(LightType type, float energy, const Vec3f& color = Vec3f(1.f))
    : m_type{ type }, m_energy{ energy }, m_color{ color } {}
  Light(LightType type, const Vec3f& direction, float energy, const Vec3f& color = Vec3f(1.f))
    : m_type{ type }, m_direction{ direction }, m_energy{ energy }, m_color{ color } {}
  Light(LightType type, const Vec3f& direction, float energy, float angle, const Vec3f& color = Vec3f(1.f))
    : m_type{ type }, m_direction{ direction }, m_energy{ energy }, m_angle{ angle }, m_color{ color } {}

  LightType getType() const { return m_type; }
  const Vec3f& getDirection() const { return m_direction; }
  Vec3f& getDirection() { return m_direction; }
  const Vec3f& getColor() const { return m_color; }
  Vec3f& getColor() { return m_color; }
  float getEnergy() const { return m_energy; }
  float getAngle() const { return m_angle; }

  void setType(LightType type) { m_type = type; }
  void setDirection(const Vec3f& direction) { m_direction = direction; }
  void setEnergy(float energy) { m_energy = energy; }
  void setAngle(float angle) { m_angle = angle; }

private:
  LightType m_type {};
  Vec3f m_direction {};
  float m_energy = 1.f;
  float m_angle  = 0.f;
  Vec3f m_color {};
};

} // namespace Raz

#endif // RAZ_LIGHT_HPP
