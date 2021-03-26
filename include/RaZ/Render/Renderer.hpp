#pragma once

#ifndef RAZ_RENDERER_HPP
#define RAZ_RENDERER_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/RendererVk.hpp"
#else
#include "RaZ/Render/OpenGL/RendererGL.hpp"
#endif

#endif // RAZ_RENDERER_HPP
