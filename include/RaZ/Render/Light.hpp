#pragma once

#ifndef RAZ_LIGHT_HPP
#define RAZ_LIGHT_HPP

#include "RaZ/Data/Color.hpp"
#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

enum class LightType {
  POINT = 0,
  DIRECTIONAL,
  SPOT
};

class Light final : public Component {
public:
  constexpr Light(LightType type, float energy, const Color& color = ColorPreset::White) noexcept
    : m_type{ type }, m_energy{ energy }, m_color{ color } {}
  constexpr Light(LightType type, const Vec3f& direction, float energy, const Color& color = ColorPreset::White) noexcept
    : m_type{ type }, m_direction{ direction }, m_energy{ energy }, m_color{ color } {}
  constexpr Light(LightType type, const Vec3f& direction, float energy, Radiansf angle, const Color& color = ColorPreset::White) noexcept
    : m_type{ type }, m_direction{ direction }, m_energy{ energy }, m_color{ color }, m_angle{ angle } {}

  constexpr LightType getType() const noexcept { return m_type; }
  constexpr const Vec3f& getDirection() const noexcept { return m_direction; }
  constexpr float getEnergy() const noexcept { return m_energy; }
  constexpr const Color& getColor() const noexcept { return m_color; }
  constexpr Radiansf getAngle() const noexcept { return m_angle; }

  constexpr void setType(LightType type) noexcept { m_type = type; }
  constexpr void setDirection(const Vec3f& direction) noexcept { m_direction = direction; }
  constexpr void setEnergy(float energy) noexcept { m_energy = energy; }
  constexpr void setColor(const Color& color) noexcept { m_color = color; }
  constexpr void setAngle(Radiansf angle) noexcept { m_angle = angle; }

private:
  LightType m_type {};
  Vec3f m_direction {};
  float m_energy = 1.f;
  Color m_color {};
  Radiansf m_angle = Radiansf(0.f);
};

} // namespace Raz

#endif // RAZ_LIGHT_HPP
