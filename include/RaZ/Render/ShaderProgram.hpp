#pragma once

#ifndef RAZ_SHADERPROGRAM_HPP
#define RAZ_SHADERPROGRAM_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/ShaderProgramVk.hpp"
#else
#include "RaZ/Render/OpenGL/ShaderProgramGL.hpp"
#endif

#endif // RAZ_SHADERPROGRAM_HPP
