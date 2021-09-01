#pragma once

#ifndef RAZ_PIPELINE_HPP
#define RAZ_PIPELINE_HPP

#if defined(RAZ_USE_VULKAN)
#include "RaZ/Render/Vulkan/PipelineVk.hpp"
#else
#include "RaZ/Render/OpenGL/PipelineGL.hpp"
#endif

#endif // RAZ_PIPELINE_HPP
