#pragma once

#ifndef RAZ_PERLINNOISE_HPP
#define RAZ_PERLINNOISE_HPP

#include <cstdint>

namespace Raz::PerlinNoise {

/// Computes the 1D Perlin noise at the given coordinate.
/// \param x X coordinate.
/// \param octaveCount Amount of octaves to apply for the [fractional Brownian motion](https://en.wikipedia.org/wiki/Fractional_Brownian_motion) computation.
/// \param normalize Remap the value between [0; 1]. If false, the original [-1; 1] range is preserved.
/// \return 1D Perlin noise value. May be slightly below or above the expected range.
float get1D(float x, uint8_t octaveCount = 1, bool normalize = false);
/// Computes the 2D Perlin noise at the given coordinates.
/// \param x X coordinate.
/// \param y Y coordinate.
/// \param octaveCount Amount of octaves to apply for the [fractional Brownian motion](https://en.wikipedia.org/wiki/Fractional_Brownian_motion) computation.
/// \param normalize Remap the value between [0; 1]. If false, the original [-1; 1] range is preserved.
/// \return 2D Perlin noise value. May be slightly below or above the expected range.
float get2D(float x, float y, uint8_t octaveCount = 1, bool normalize = false);

} // namespace Raz::PerlinNoise

#endif // RAZ_PERLINNOISE_HPP
