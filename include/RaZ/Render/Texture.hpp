#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include "RaZ/Utils/EnumUtils.hpp"

#include <memory>

namespace Raz {

enum class ColorPreset : uint32_t {
  BLACK        = 0x000000,
  GRAY         = 0x808080,
  RED          = 0xFF0000,
  GREEN        = 0x00FF00,
  BLUE         = 0x0000FF,
  MEDIUM_RED   = GRAY | RED,
  MEDIUM_GREEN = GRAY | GREEN,
  MEDIUM_BLUE  = GRAY | BLUE,
  YELLOW       = RED | GREEN,
  CYAN         = GREEN | BLUE,
  MAGENTA      = RED | BLUE,
  WHITE        = RED | GREEN | BLUE
};
MAKE_ENUM_FLAG(ColorPreset)

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

} // namespace Raz

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/TextureVk.hpp"
#else
#include "RaZ/Render/OpenGL/TextureGL.hpp"
#endif

#endif // RAZ_TEXTURE_HPP
