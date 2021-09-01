#pragma once

#ifndef RAZ_RENDERPASS_HPP
#define RAZ_RENDERPASS_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/RenderPassVk.hpp"
#else
#include "RaZ/Render/OpenGL/RenderPassGL.hpp"
#endif

#endif // RAZ_RENDERPASS_HPP
