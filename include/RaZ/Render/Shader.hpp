#pragma once

#ifndef RAZ_SHADER_HPP
#define RAZ_SHADER_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/ShaderVk.hpp"
#else
#include "RaZ/Render/OpenGL/ShaderGL.hpp"
#endif

#endif // RAZ_SHADER_HPP
