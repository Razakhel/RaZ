#pragma once

#ifndef RAZ_IMAGEUTILS_HPP
#define RAZ_IMAGEUTILS_HPP

#include "RaZ/Data/Image.hpp"

#include <array>

namespace Raz::ImageUtils {

std::array<Image, 6> convertEquirectangularToCubemap(const Image& equirectangularImg);

} // namespace Raz::ImageUtils

#endif // RAZ_IMAGEUTILS_HPP
