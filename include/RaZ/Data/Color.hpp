#pragma once

#ifndef RAZ_COLOR_HPP
#define RAZ_COLOR_HPP

#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Color {
public:
  constexpr Color() = default;
  constexpr explicit Color(const Vec3f& values) : m_values{ values } {
    assert("Error: Color values are out of bounds." && values[0] >= 0.f && values[0] <= 1.f
                                                    && values[1] >= 0.f && values[1] <= 1.f
                                                    && values[2] >= 0.f && values[2] <= 1.f);
  }
  constexpr Color(float red, float green, float blue) : Color(Vec3f(red, green, blue)) {}
  constexpr Color(int red, int green, int blue) : Color(static_cast<float>(red) / 255.f,
                                                        static_cast<float>(green) / 255.f,
                                                        static_cast<float>(blue) / 255.f) {}
  constexpr explicit Color(const Vec3b& values) : Color(values[0], values[1], values[2]) {}
  constexpr explicit Color(uint32_t value) : Color(static_cast<uint8_t>((value & 0xFF0000) >> 16u),
                                                   static_cast<uint8_t>((value & 0x00FF00) >> 8u),
                                                   static_cast<uint8_t>((value & 0x0000FF))) {}

  constexpr operator const Vec3f&() const noexcept { return m_values; }
  constexpr explicit operator Vec3b() const noexcept { return Vec3b(m_values[0] * 255, m_values[1] * 255, m_values[2] * 255); }
  constexpr bool operator==(const Color& color) const noexcept { return m_values == color.m_values; }
  constexpr bool operator!=(const Color& color) const noexcept { return !(*this == color); }

private:
  Vec3f m_values {};
};

namespace ColorPreset {

constexpr Color Black(0.f, 0.f, 0.f);
constexpr Color Gray(0.5f, 0.5f, 0.5f);
constexpr Color Red(1.f, 0.f, 0.f);
constexpr Color Green(0.f, 1.f, 0.f);
constexpr Color Blue(0.f, 0.f, 1.f);
constexpr Color MediumRed(1.f, 0.5f, 0.5f);
constexpr Color MediumGreen(0.5f, 1.f, 0.5f);
constexpr Color MediumBlue(0.5f, 0.5f, 1.f);
constexpr Color Cyan(0.f, 1.f, 1.f);
constexpr Color Magenta(1.f, 0.f, 1.f);
constexpr Color Yellow(1.f, 1.f, 0.f);
constexpr Color White(1.f, 1.f, 1.f);

} // namespace ColorPreset

} // namespace Raz

#endif // RAZ_COLOR_HPP

