#pragma once

#ifndef RAZ_CUBEMAP_HPP
#define RAZ_CUBEMAP_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/CubemapVk.hpp"
#else
#include "RaZ/Render/OpenGL/CubemapGL.hpp"
#endif

#endif // RAZ_CUBEMAP_HPP
