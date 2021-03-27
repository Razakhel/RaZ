#pragma once

#ifndef RAZ_UNIFORMBUFFER_HPP
#define RAZ_UNIFORMBUFFER_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/UniformBufferVk.hpp"
#else
#include "RaZ/Render/OpenGL/UniformBufferGL.hpp"
#endif

#endif // RAZ_UNIFORMBUFFER_HPP
