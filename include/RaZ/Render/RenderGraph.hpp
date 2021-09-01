#pragma once

#ifndef RAZ_RENDERGRAPH_HPP
#define RAZ_RENDERGRAPH_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/RenderGraphVk.hpp"
#else
#include "RaZ/Render/OpenGL/RenderGraphGL.hpp"
#endif

#endif // RAZ_RENDERGRAPH_HPP
