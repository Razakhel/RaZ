#pragma once

#ifndef RAZ_RENDERERVK_HPP
#define RAZ_RENDERERVK_HPP

#include "RaZ/Utils/EnumUtils.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace Raz {

enum class CullingMode : uint32_t {
  NONE       = 0 /* VK_CULL_MODE_NONE           */, ///< No triangle culling (shows everything).
  FRONT      = 1 /* VK_CULL_MODE_FRONT_BIT      */, ///< Cull front-facing triangles.
  BACK       = 2 /* VK_CULL_MODE_BACK_BIT       */, ///< Cull back-facing triangles.
  FRONT_BACK = 3 /* VK_CULL_MODE_FRONT_AND_BACK */  ///< Cull both front- & back-facing triangles (discards everything).
};

enum class ShaderStage : uint32_t {
  VERTEX                  = 1          /* VK_SHADER_STAGE_VERTEX_BIT                  */, ///< Vertex shader stage.
  TESSELLATION_CONTROL    = 2          /* VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT    */, ///< Tesselation control shader stage.
  TESSELLATION_EVALUATION = 4          /* VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT */, ///< Tesselation evualuation shader stage.
  GEOMETRY                = 8          /* VK_SHADER_STAGE_GEOMETRY_BIT                */, ///< Geometry shader stage.
  FRAGMENT                = 16         /* VK_SHADER_STAGE_FRAGMENT_BIT                */, ///< Fragment shader stage.
  COMPUTE                 = 32         /* VK_SHADER_STAGE_COMPUTE_BIT                 */, ///< Compute shader stage.
  ALL_GRAPHICS            = 31         /* VK_SHADER_STAGE_ALL_GRAPHICS                */, ///< All graphic shader stages.
  ALL                     = 2147483647 /* VK_SHADER_STAGE_ALL                         */, ///< All shader stages.
  RAYGEN                  = 256        /* VK_SHADER_STAGE_RAYGEN_BIT_KHR              */, ///< Raygen shader stage.
  ANY_HIT                 = 512        /* VK_SHADER_STAGE_ANY_HIT_BIT_KHR             */, ///< Any hit shader stage.
  CLOSEST_HIT             = 1024       /* VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR         */, ///< Closest hit shader stage.
  MISS                    = 2048       /* VK_SHADER_STAGE_MISS_BIT_KHR                */, ///< Miss shader stage.
  INTERSECTION            = 4096       /* VK_SHADER_STAGE_INTERSECTION_BIT_KHR        */, ///< Intersection shader stage.
  CALLABLE                = 8192       /* VK_SHADER_STAGE_CALLABLE_BIT_KHR            */, ///< Callable shader stage.
  TASK_NV                 = 64         /* VK_SHADER_STAGE_TASK_BIT_NV                 */, ///< Task shader stage (NVidia specific).
  MESH_NV                 = 128        /* VK_SHADER_STAGE_MESH_BIT_NV                 */  ///< Mesh shader stage (NVidia specific).
};
MAKE_ENUM_FLAG(ShaderStage)

enum class ImageViewType : uint32_t {
  IMAGE_1D   = 0 /* VK_IMAGE_VIEW_TYPE_1D         */, ///< One-dimensional image view.
  IMAGE_2D   = 1 /* VK_IMAGE_VIEW_TYPE_2D         */, ///< Two-dimensional image view.
  IMAGE_3D   = 2 /* VK_IMAGE_VIEW_TYPE_3D         */, ///< Three-dimensional image view.
  CUBE       = 3 /* VK_IMAGE_VIEW_TYPE_CUBE       */, ///< Cubic image view.
  ARRAY_1D   = 4 /* VK_IMAGE_VIEW_TYPE_1D_ARRAY   */, ///< One-dimensional array image view.
  ARRAY_2D   = 5 /* VK_IMAGE_VIEW_TYPE_2D_ARRAY   */, ///< Two-dimensional array image view.
  CUBE_ARRAY = 6 /* VK_IMAGE_VIEW_TYPE_CUBE_ARRAY */  ///< Cubic array image view.
};

enum class ComponentSwizzle : uint32_t {
  IDENTITY = 0 /* VK_COMPONENT_SWIZZLE_IDENTITY */, ///<
  ZERO     = 1 /* VK_COMPONENT_SWIZZLE_ZERO     */, ///<
  ONE      = 2 /* VK_COMPONENT_SWIZZLE_ONE      */, ///<
  RED      = 3 /* VK_COMPONENT_SWIZZLE_R        */, ///<
  GREEN    = 4 /* VK_COMPONENT_SWIZZLE_G        */, ///<
  BLUE     = 5 /* VK_COMPONENT_SWIZZLE_B        */, ///<
  ALPHA    = 6 /* VK_COMPONENT_SWIZZLE_A        */  ///<
};

enum class ImageAspect : uint32_t {
  COLOR          = 1    /* VK_IMAGE_ASPECT_COLOR_BIT              */, ///<
  DEPTH          = 2    /* VK_IMAGE_ASPECT_DEPTH_BIT              */, ///<
  STENCIL        = 4    /* VK_IMAGE_ASPECT_STENCIL_BIT            */, ///<
  METADATA       = 8    /* VK_IMAGE_ASPECT_METADATA_BIT           */, ///<
  PLANE_0        = 16   /* VK_IMAGE_ASPECT_PLANE_0_BIT            */, ///<
  PLANE_1        = 32   /* VK_IMAGE_ASPECT_PLANE_1_BIT            */, ///<
  PLANE_2        = 64   /* VK_IMAGE_ASPECT_PLANE_2_BIT            */, ///<
  MEMORY_PLANE_0 = 128  /* VK_IMAGE_ASPECT_PLANE_0_BIT_KHR        */, ///<
  MEMORY_PLANE_1 = 256  /* VK_IMAGE_ASPECT_PLANE_1_BIT_KHR        */, ///<
  MEMORY_PLANE_2 = 512  /* VK_IMAGE_ASPECT_PLANE_2_BIT_KHR        */, ///<
  MEMORY_PLANE_3 = 1024 /* VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT */  ///<
};
MAKE_ENUM_FLAG(ImageAspect)

enum class CommandPoolOption : uint32_t {
  TRANSIENT            = 1 /* VK_COMMAND_POOL_CREATE_TRANSIENT_BIT            */, ///<
  RESET_COMMAND_BUFFER = 2 /* VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT */, ///<
  PROTECTED            = 4 /* VK_COMMAND_POOL_CREATE_PROTECTED_BIT            */  ///<
};
MAKE_ENUM_FLAG(CommandPoolOption)

enum class ImageType : uint32_t {
  IMAGE_1D = 0 /* VK_IMAGE_TYPE_1D */, ///< One-dimensional image.
  IMAGE_2D = 1 /* VK_IMAGE_TYPE_2D */, ///< Two-dimensional image.
  IMAGE_3D = 2 /* VK_IMAGE_TYPE_3D */  ///< Three-dimensional image.
};

enum class SampleCount : uint32_t {
  ONE        = 1  /* VK_SAMPLE_COUNT_1_BIT  */, ///< One sample per pixel.
  TWO        = 2  /* VK_SAMPLE_COUNT_2_BIT  */, ///< Two samples per pixel.
  FOUR       = 4  /* VK_SAMPLE_COUNT_4_BIT  */, ///< Four samples per pixel.
  EIGHT      = 8  /* VK_SAMPLE_COUNT_8_BIT  */, ///< Eight samples per pixel.
  SIXTEEN    = 16 /* VK_SAMPLE_COUNT_16_BIT */, ///< Sixteen samples per pixel.
  THIRTY_TWO = 32 /* VK_SAMPLE_COUNT_32_BIT */, ///< Thirty-two samples per pixel.
  SIXTY_FOUR = 64 /* VK_SAMPLE_COUNT_64_BIT */  ///< Sixty-four samples per pixel.
};

enum class ImageTiling : uint32_t {
  OPTIMAL             = 0          /* VK_IMAGE_TILING_OPTIMAL                 */, ///<
  LINEAR              = 1          /* VK_IMAGE_TILING_LINEAR                  */, ///<
  DRM_FORMAT_MODIFIER = 1000158000 /* VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT */  ///<
};

enum class ImageUsage : uint32_t {
  TRANSFER_SRC             = 1   /* VK_IMAGE_USAGE_TRANSFER_SRC_BIT             */, ///< Transfer command source.
  TRANSFER_DST             = 2   /* VK_IMAGE_USAGE_TRANSFER_DST_BIT             */, ///< Transfer command destination.
  SAMPLED                  = 4   /* VK_IMAGE_USAGE_SAMPLED_BIT                  */, ///< Sampled (in shaders).
  STORAGE                  = 8   /* VK_IMAGE_USAGE_STORAGE_BIT                  */, ///< Storage.
  COLOR_ATTACHMENT         = 16  /* VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT         */, ///< Color attachment.
  DEPTH_STENCIL_ATTACHMENT = 32  /* VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT */, ///< Depth & stencil attachment.
  TRANSIENT_ATTACHMENT     = 64  /* VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT     */, ///< Transient attachment.
  INPUT_ATTACHMENT         = 128 /* VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT         */, ///< Input attachment.
  SHADING_RATE_IMAGE_NV    = 256 /* VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV    */, ///< Shading rate (NVidia specific).
  FRAGMENT_DENSITY_MAP     = 512 /* VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT */  ///< Fragment density map.
};
MAKE_ENUM_FLAG(ImageUsage)

enum class SharingMode : uint32_t {
  EXCLUSIVE  = 0 /* VK_SHARING_MODE_EXCLUSIVE  */, ///< Exclusive sharing mode.
  CONCURRENT = 1 /* VK_SHARING_MODE_CONCURRENT */  ///< Concurrent sharing mode.
};

enum class ImageLayout : uint32_t {
  UNDEFINED                          = 0          /* VK_IMAGE_LAYOUT_UNDEFINED                                  */, ///< Undefined.
  GENERAL                            = 1          /* VK_IMAGE_LAYOUT_GENERAL                                    */, ///< General.
  COLOR_ATTACHMENT                   = 2          /* VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL                   */, ///< Color attachment.
  DEPTH_STENCIL_ATTACHMENT           = 3          /* VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL           */, ///< Depth & stencil attachment.
  DEPTH_STENCIL_READ_ONLY            = 4          /* VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL            */, ///< Read-only depth & read-only stencil.
  SHADER_READ_ONLY                   = 5          /* VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL                   */, ///< Read-only shader.
  TRANSFER_SRC                       = 6          /* VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL                       */, ///< Transfer command source.
  TRANSFER_DST                       = 7          /* VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL                       */, ///< Transfer command destination.
  PREINITIALIZED                     = 8          /* VK_IMAGE_LAYOUT_PREINITIALIZED                             */, ///< Preinitialized.
  DEPTH_READ_ONLY_STENCIL_ATTACHMENT = 1000117000 /* VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL */, ///< Read-only depth & stencil.
  DEPTH_ATTACHMENT_STENCIL_READ_ONLY = 1000117001 /* VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL */, ///< Depth & read-only stencil.
  DEPTH_ATTACHMENT                   = 1000241000 /* VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL                   */, ///< Depth attachment.
  DEPTH_READ_ONLY                    = 1000241001 /* VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL                    */, ///< Read-only depth attachment.
  STENCIL_ATTACHMENT                 = 1000241002 /* VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL                 */, ///< Stencil attachment.
  STENCIL_READ_ONLY                  = 1000241003 /* VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL                  */, ///< Read-only stencil attachment.
  PRESENT_SRC                        = 1000001002 /* VK_IMAGE_LAYOUT_PRESENT_SRC_KHR                            */, ///< Source presentation.
  SHARED_PRESENT                     = 1000111000 /* VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR                         */, ///< Shared presentation.
  SHADING_RATE                       = 1000164003 /* VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV                    */, ///< Shading rate.
  FRAGMENT_DENSITY_MAP               = 1000218000 /* VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT           */  ///< Fragment density map.
};

enum class TextureFilter : uint32_t {
  NEAREST = 0          /* VK_FILTER_NEAREST   */, ///<
  LINEAR  = 1          /* VK_FILTER_LINEAR    */, ///<
  CUBIC   = 1000015000 /* VK_FILTER_CUBIC_IMG */  ///<
};

enum class SamplerMipmapMode : uint32_t {
  NEAREST = 0 /* VK_SAMPLER_MIPMAP_MODE_NEAREST */, ///<
  LINEAR  = 1 /* VK_SAMPLER_MIPMAP_MODE_LINEAR  */  ///<
};

enum class SamplerAddressMode : uint32_t {
  REPEAT               = 0 /* VK_SAMPLER_ADDRESS_MODE_REPEAT               */, ///<
  MIRRORED_REPEAT      = 1 /* VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT      */, ///<
  CLAMP_TO_EDGE        = 2 /* VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE        */, ///<
  CLAMP_TO_BORDER      = 3 /* VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER      */, ///<
  MIRROR_CLAMP_TO_EDGE = 4 /* VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE */  ///<
};

enum class ComparisonOperation : uint32_t {
  NEVER            = 0 /* VK_COMPARE_OP_NEVER            */, ///<
  LESS             = 1 /* VK_COMPARE_OP_LESS             */, ///<
  EQUAL            = 2 /* VK_COMPARE_OP_EQUAL            */, ///<
  LESS_OR_EQUAL    = 3 /* VK_COMPARE_OP_LESS_OR_EQUAL    */, ///<
  GREATER          = 4 /* VK_COMPARE_OP_GREATER          */, ///<
  NOT_EQUAL        = 5 /* VK_COMPARE_OP_NOT_EQUAL        */, ///<
  GREATER_OR_EQUAL = 6 /* VK_COMPARE_OP_GREATER_OR_EQUAL */, ///<
  ALWAYS           = 7 /* VK_COMPARE_OP_ALWAYS           */  ///<
};

enum class BorderColor : uint32_t {
  FLOAT_TRANSPARENT_BLACK = 0 /* VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK */, ///<
  INT_TRANSPARENT_BLACK   = 1 /* VK_BORDER_COLOR_INT_TRANSPARENT_BLACK   */, ///<
  FLOAT_OPAQUE_BLACK      = 2 /* VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK      */, ///<
  INT_OPAQUE_BLACK        = 3 /* VK_BORDER_COLOR_INT_OPAQUE_BLACK        */, ///<
  FLOAT_OPAQUE_WHITE      = 4 /* VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE      */, ///<
  INT_OPAQUE_WHITE        = 5 /* VK_BORDER_COLOR_INT_OPAQUE_WHITE        */  ///<
};

enum class BufferUsage : uint32_t {
  TRANSFER_SRC               = 1      /* VK_BUFFER_USAGE_TRANSFER_SRC_BIT                          */, ///< Memory transfer source buffer.
  TRANSFER_DST               = 2      /* VK_BUFFER_USAGE_TRANSFER_DST_BIT                          */, ///< Memory transfer destination buffer.
  UNIFORM_TEXEL_BUFFER       = 4      /* VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT                  */, ///< Uniform texel buffer.
  STORAGE_TEXEL_BUFFER       = 8      /* VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT                  */, ///< Storage texel buffer.
  UNIFORM_BUFFER             = 16     /* VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT                        */, ///< Uniform buffer.
  STORAGE_BUFFER             = 32     /* VK_BUFFER_USAGE_STORAGE_BUFFER_BIT                        */, ///< Storage buffer.
  INDEX_BUFFER               = 64     /* VK_BUFFER_USAGE_INDEX_BUFFER_BIT                          */, ///< Index buffer.
  VERTEX_BUFFER              = 128    /* VK_BUFFER_USAGE_VERTEX_BUFFER_BIT                         */, ///< Vertex buffer.
  INDIRECT_BUFFER            = 256    /* VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT                       */, ///< Indirect buffer.
  SHADER_DEVICE_ADDRESS      = 131072 /* VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT                 */, ///<
  TRANSFORM_FEEDBACK         = 2048   /* VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT         */, ///<
  TRANSFORM_FEEDBACK_COUNTER = 4096   /* VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT */, ///<
  CONDITIONAL_RENDERING      = 512    /* VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT             */, ///<
  RAY_TRACING                = 1024   /* VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR                       */  ///<
};
MAKE_ENUM_FLAG(BufferUsage)

enum class MemoryProperty : uint32_t {
  DEVICE_LOCAL        = 1   /* VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT        */, ///<
  HOST_VISIBLE        = 2   /* VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT        */, ///<
  HOST_COHERENT       = 4   /* VK_MEMORY_PROPERTY_HOST_COHERENT_BIT       */, ///<
  HOST_CACHED         = 8   /* VK_MEMORY_PROPERTY_HOST_CACHED_BIT         */, ///<
  LAZILY_ALLOCATED    = 16  /* VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT    */, ///<
  PROTECTED           = 32  /* VK_MEMORY_PROPERTY_PROTECTED_BIT           */, ///<
  DEVICE_COHERENT_AMD = 64  /* VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD */, ///<
  DEVICE_UNCACHED_AMD = 128 /* VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD */  ///<
};
MAKE_ENUM_FLAG(MemoryProperty)

enum class AttachmentLoadOp : uint32_t {
  LOAD      = 0 /* VK_ATTACHMENT_LOAD_OP_LOAD      */, ///<
  CLEAR     = 1 /* VK_ATTACHMENT_LOAD_OP_CLEAR     */, ///<
  DONT_CARE = 2 /* VK_ATTACHMENT_LOAD_OP_DONT_CARE */  ///<
};

enum class AttachmentStoreOp : uint32_t {
  STORE     = 0 /* VK_ATTACHMENT_STORE_OP_STORE     */, ///<
  DONT_CARE = 1 /* VK_ATTACHMENT_STORE_OP_DONT_CARE */  ///<
};

enum class PipelineBindPoint : uint32_t {
  GRAPHICS    = 0          /* VK_PIPELINE_BIND_POINT_GRAPHICS        */, ///<
  COMPUTE     = 1          /* VK_PIPELINE_BIND_POINT_COMPUTE         */, ///<
  RAY_TRACING = 1000165000 /* VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR */  ///<
};

enum class PipelineStage : uint32_t {
  TOP_OF_PIPE                    = 1        /* VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT                      */, ///<
  DRAW_INDIRECT                  = 2        /* VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT                    */, ///<
  VERTEX_INPUT                   = 4        /* VK_PIPELINE_STAGE_VERTEX_INPUT_BIT                     */, ///<
  VERTEX_SHADER                  = 8        /* VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                    */, ///<
  TESSELLATION_CONTROL_SHADER    = 16       /* VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT      */, ///<
  TESSELLATION_EVALUATION_SHADER = 32       /* VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT   */, ///<
  GEOMETRY_SHADER                = 64       /* VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                  */, ///<
  FRAGMENT_SHADER                = 128      /* VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                  */, ///<
  EARLY_FRAGMENT_TESTS           = 256      /* VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT             */, ///<
  LATE_FRAGMENT_TESTS            = 512      /* VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT              */, ///<
  COLOR_ATTACHMENT_OUTPUT        = 1024     /* VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT          */, ///<
  COMPUTE_SHADER                 = 2048     /* VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                   */, ///<
  TRANSFER                       = 4096     /* VK_PIPELINE_STAGE_TRANSFER_BIT                         */, ///<
  BOTTOM_OF_PIPE                 = 8192     /* VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT                   */, ///<
  HOST                           = 16384    /* VK_PIPELINE_STAGE_HOST_BIT                             */, ///<
  ALL_GRAPHICS                   = 32768    /* VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT                     */, ///<
  ALL_COMMANDS                   = 65536    /* VK_PIPELINE_STAGE_ALL_COMMANDS_BIT                     */, ///<
  TRANSFORM_FEEDBACK             = 16777216 /* VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT           */, ///<
  CONDITIONAL_RENDERING          = 262144   /* VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT        */, ///<
  RAY_TRACING_SHADER             = 2097152  /* VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR           */, ///<
  ACCELERATION_STRUCTURE_BUILD   = 33554432 /* VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR */, ///<
  SHADING_RATE_IMAGE_NV          = 4194304  /* VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV            */, ///<
  TASK_SHADER_NV                 = 524288   /* VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV                   */, ///<
  MESH_SHADER_NV                 = 1048576  /* VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV                   */, ///<
  FRAGMENT_DENSITY_PROCESS       = 8388608  /* VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT     */, ///<
  COMMAND_PREPROCESS_NV          = 131072   /* VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV            */  ///<
};
MAKE_ENUM_FLAG(PipelineStage)

enum class MemoryAccess : uint32_t {
  INDIRECT_COMMAND_READ             = 1         /* VK_ACCESS_INDIRECT_COMMAND_READ_BIT                 */, ///<
  INDEX_READ                        = 2         /* VK_ACCESS_INDEX_READ_BIT                            */, ///<
  VERTEX_ATTRIBUTE_READ             = 4         /* VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT                 */, ///<
  UNIFORM_READ                      = 8         /* VK_ACCESS_UNIFORM_READ_BIT                          */, ///<
  INPUT_ATTACHMENT_READ             = 16        /* VK_ACCESS_INPUT_ATTACHMENT_READ_BIT                 */, ///<
  SHADER_READ                       = 32        /* VK_ACCESS_SHADER_READ_BIT                           */, ///<
  SHADER_WRITE                      = 64        /* VK_ACCESS_SHADER_WRITE_BIT                          */, ///<
  COLOR_ATTACHMENT_READ             = 128       /* VK_ACCESS_COLOR_ATTACHMENT_READ_BIT                 */, ///<
  COLOR_ATTACHMENT_WRITE            = 256       /* VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT                */, ///<
  DEPTH_STENCIL_ATTACHMENT_READ     = 512       /* VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT         */, ///<
  DEPTH_STENCIL_ATTACHMENT_WRITE    = 1024      /* VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT        */, ///<
  TRANSFER_READ                     = 2048      /* VK_ACCESS_TRANSFER_READ_BIT                         */, ///<
  TRANSFER_WRITE                    = 4096      /* VK_ACCESS_TRANSFER_WRITE_BIT                        */, ///<
  HOST_READ                         = 8192      /* VK_ACCESS_HOST_READ_BIT                             */, ///<
  HOST_WRITE                        = 16384     /* VK_ACCESS_HOST_WRITE_BIT                            */, ///<
  MEMORY_READ                       = 32768     /* VK_ACCESS_MEMORY_READ_BIT                           */, ///<
  MEMORY_WRITE                      = 65536     /* VK_ACCESS_MEMORY_WRITE_BIT                          */, ///<
  TRANSFORM_FEEDBACK_WRITE          = 33554432  /* VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT          */, ///<
  TRANSFORM_FEEDBACK_COUNTER_READ   = 67108864  /* VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT   */, ///<
  TRANSFORM_FEEDBACK_COUNTER_WRITE  = 134217728 /* VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT  */, ///<
  CONDITIONAL_RENDERING_READ        = 1048576   /* VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT        */, ///<
  COLOR_ATTACHMENT_READ_NONCOHERENT = 524288    /* VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT */, ///<
  ACCELERATION_STRUCTURE_READ       = 2097152   /* VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR       */, ///<
  ACCELERATION_STRUCTURE_WRITE      = 4194304   /* VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR      */, ///<
  SHADING_RATE_IMAGE_READ_NV        = 8388608   /* VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV            */, ///<
  FRAGMENT_DENSITY_MAP_READ         = 16777216  /* VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT         */, ///<
  COMMAND_PREPROCESS_READ_NV        = 131072    /* VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV            */, ///<
  COMMAND_PREPROCESS_WRITE_NV       = 262144    /* VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV           */  ///<
};
MAKE_ENUM_FLAG(MemoryAccess)

enum class CommandBufferLevel : uint32_t {
  PRIMARY   = 0 /* VK_COMMAND_BUFFER_LEVEL_PRIMARY   */, ///<
  SECONDARY = 1 /* VK_COMMAND_BUFFER_LEVEL_SECONDARY */  ///<
};

enum class CommandBufferUsage : uint32_t {
  ONE_TIME_SUBMIT      = 1 /* VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT      */, ///<
  RENDER_PASS_CONTINUE = 2 /* VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT */, ///<
  SIMULTANEOUS_USE     = 4 /* VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT     */  ///<
};
MAKE_ENUM_FLAG(CommandBufferUsage)

enum class DescriptorType : uint32_t {
  SAMPLER                  = 0          /* VK_DESCRIPTOR_TYPE_SAMPLER                    */, ///<
  COMBINED_IMAGE_SAMPLER   = 1          /* VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER     */, ///<
  SAMPLED_IMAGE            = 2          /* VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE              */, ///<
  STORAGE_IMAGE            = 3          /* VK_DESCRIPTOR_TYPE_STORAGE_IMAGE              */, ///<
  UNIFORM_TEXEL_BUFFER     = 4          /* VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER       */, ///<
  STORAGE_TEXEL_BUFFER     = 5          /* VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER       */, ///<
  UNIFORM_BUFFER           = 6          /* VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER             */, ///<
  STORAGE_BUFFER           = 7          /* VK_DESCRIPTOR_TYPE_STORAGE_BUFFER             */, ///<
  UNIFORM_BUFFER_DYNAMIC   = 8          /* VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC     */, ///<
  STORAGE_BUFFER_DYNAMIC   = 9          /* VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC     */, ///<
  INPUT_ATTACHMENT         = 10         /* VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT           */, ///<
  INLINE_UNIFORM_BLOCK_EXT = 1000138000 /* VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT   */, ///<
  ACCELERATION_STRUCTURE   = 1000165000 /* VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR */  ///<
};

// TODO: temporary GL enums to allow compilation

enum class MaskType : unsigned int {
  COLOR = 16384, // GL_COLOR_BUFFER_BIT
  DEPTH = 256    // GL_DEPTH_BUFFER_BIT
};
MAKE_ENUM_FLAG(MaskType)

enum class DepthFunction : unsigned int {
  EQUAL      = 514, // GL_EQUAL
  LESS       = 513, // GL_LESS
  LESS_EQUAL = 515  // GL_LEQUAL
};

enum class PolygonMode : uint32_t {
  POINT = 2, // VK_POLYGON_MODE_POINT
  LINE  = 1, // VK_POLYGON_MODE_LINE
  FILL  = 0  // VK_POLYGON_MODE_FILL
};

enum class BufferType : unsigned int {
  ARRAY_BUFFER   = 34962, // GL_ARRAY_BUFFER
  ELEMENT_BUFFER = 34963, // GL_ELEMENT_ARRAY_BUFFER
  UNIFORM_BUFFER = 35345  // GL_UNIFORM_BUFFER
};

enum class BufferDataUsage : unsigned int {
  STATIC_DRAW = 35044 // GL_STATIC_DRAW
};

enum class ShaderType : unsigned int {
  VERTEX   = 35633, // GL_VERTEX_SHADER
  FRAGMENT = 35632, // GL_FRAGMENT_SHADER
  GEOMETRY = 36313  // GL_GEOMETRY_SHADER
};

enum class TextureType : unsigned int {
  TEXTURE_2D    = 3553,  // GL_TEXTURE_2D
  CUBEMAP       = 34067, // GL_TEXTURE_CUBE_MAP
  CUBEMAP_POS_X = 34069, // GL_TEXTURE_CUBE_MAP_POSITIVE_X
  CUBEMAP_NEG_X = 34070, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
  CUBEMAP_POS_Y = 34071, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
  CUBEMAP_NEG_Y = 34072, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
  CUBEMAP_POS_Z = 34073, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
  CUBEMAP_NEG_Z = 34074  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

enum class TextureParam : unsigned int {
  MINIFY_FILTER  = 10241, // GL_TEXTURE_MIN_FILTER
  MAGNIFY_FILTER = 10240, // GL_TEXTURE_MAG_FILTER
  WRAP_S         = 10242, // GL_TEXTURE_WRAP_S
  WRAP_T         = 10243, // GL_TEXTURE_WRAP_T
  WRAP_R         = 32882, // GL_TEXTURE_WRAP_R
  SWIZZLE_RGBA   = 36422  // GL_TEXTURE_SWIZZLE_RGBA
};

enum class TextureParamValue : unsigned int {
  NEAREST              = 9728,  // GL_NEAREST
  LINEAR               = 9729,  // GL_LINEAR
  LINEAR_MIPMAP_LINEAR = 9987,  // GL_LINEAR_MIPMAP_LINEAR
  REPEAT               = 10497, // GL_REPEAT
  CLAMP_TO_EDGE        = 33071  // GL_CLAMP_TO_EDGE
};

enum class TextureFormat : unsigned int {
  RED           = 6403,  // GL_RED
  RG            = 33319, // GL_RG
  RGB           = 6407,  // GL_RGB
  BGR           = 32992, // GL_BGR
  RGBA          = 6408,  // GL_RGBA
  BGRA          = 32993, // GL_BGRA
  SRGB          = 35904, // GL_SRGB
  DEPTH         = 6402,  // GL_DEPTH_COMPONENT
  DEPTH_STENCIL = 34041  // GL_DEPTH_STENCIL
};

enum class TextureInternalFormat : unsigned int {
  // Base formats
  RGB   = static_cast<unsigned int>(TextureFormat::RGB),
  RGBA  = static_cast<unsigned int>(TextureFormat::RGBA),
  SRGB  = static_cast<unsigned int>(TextureFormat::SRGB),
  DEPTH = static_cast<unsigned int>(TextureFormat::DEPTH),

  // Sized formats
  RED16F   = 33325, // GL_R16F
  RG16F    = 33327, // GL_RG16F
  RGB16F   = 34843, // GL_RGB16F
  RGBA16F  = 34842, // GL_RGBA16F
  DEPTH32F = 36012  // GL_DEPTH_COMPONENT32F
};

enum class TextureDataType : unsigned int {
  UBYTE = 5121, // GL_UNSIGNED_BYTE
  FLOAT = 5126  // GL_FLOAT
};

enum class FramebufferAttachment : unsigned int {
  DEPTH  = 36096, // GL_DEPTH_ATTACHMENT
  COLOR0 = 36064  // GL_COLOR_ATTACHMENT0
};

enum class FramebufferType : unsigned int {
  FRAMEBUFFER = 36160 // GL_FRAMEBUFFER
};

enum class DrawBuffer : unsigned int {
  COLOR_ATTACHMENT0 = static_cast<unsigned int>(FramebufferAttachment::COLOR0)
};

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void initialize(GLFWwindow* windowHandle = nullptr);
  static bool isInitialized() { return s_isInitialized; }

  static void createRenderPass(VkRenderPass& renderPass,
                               VkFormat swapchainImageFormat,
                               SampleCount sampleCount,
                               AttachmentLoadOp colorDepthLoadOp,
                               AttachmentStoreOp colorDepthStoreOp,
                               AttachmentLoadOp stencilLoadOp,
                               AttachmentStoreOp stencilStoreOp,
                               ImageLayout initialLayout,
                               ImageLayout finalLayout,
                               ImageLayout referenceLayout,
                               PipelineBindPoint bindPoint,
                               uint32_t srcSubpass,
                               uint32_t dstSubpass,
                               PipelineStage srcStage,
                               PipelineStage dstStage,
                               MemoryAccess srcAccess,
                               MemoryAccess dstAccess);
  static void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout,
                                        std::initializer_list<VkDescriptorSetLayoutBinding> layoutBindings);
  static void createShaderModule(VkShaderModule& shaderModule, std::size_t shaderCodeSize, const char* shaderCodeStr);
  static void destroyShaderModule(VkShaderModule shaderModule);
  static void createPipelineLayout(VkPipelineLayout& pipelineLayout,
                                   uint32_t descriptorSetLayoutCount,
                                   const VkDescriptorSetLayout* descriptorSetLayouts,
                                   uint32_t pushConstantRangeCount,
                                   const VkPushConstantRange* pushConstantRanges);
  static void createPipelineLayout(VkPipelineLayout& pipelineLayout,
                                   std::initializer_list<VkDescriptorSetLayout> descriptorSetLayouts,
                                   std::initializer_list<VkPushConstantRange> pushConstantRanges);
  static void createCommandPool(VkCommandPool& commandPool, CommandPoolOption options, uint32_t queueFamilyIndex);
  static void createImage(VkImage& image,
                          VkDeviceMemory& imageMemory,
                          ImageType imgType,
                          uint32_t imgWidth,
                          uint32_t imgHeight,
                          uint32_t imgDepth,
                          uint32_t mipLevelCount,
                          uint32_t arrayLayerCount,
                          SampleCount sampleCount,
                          ImageTiling imgTiling,
                          ImageUsage imgUsage,
                          SharingMode sharingMode,
                          ImageLayout initialLayout);
  static void destroyImage(VkImage image, VkDeviceMemory imageMemory);
  static void createImageView(VkImageView& imageView,
                              VkImage image,
                              ImageViewType imageViewType,
                              VkFormat imageFormat,
                              ComponentSwizzle redComp,
                              ComponentSwizzle greenComp,
                              ComponentSwizzle blueComp,
                              ComponentSwizzle alphaComp,
                              ImageAspect imageAspect,
                              uint32_t firstMipLevel,
                              uint32_t mipLevelCount,
                              uint32_t firstArrayLayer,
                              uint32_t arrayLayerCount);
  static void destroyImageView(VkImageView imageView);
  static void createSampler(VkSampler& sampler,
                            TextureFilter magnifyFilter,
                            TextureFilter minifyFilter,
                            SamplerMipmapMode mipmapMode,
                            SamplerAddressMode addressModeU,
                            SamplerAddressMode addressModeV,
                            SamplerAddressMode addressModeW,
                            float mipmapLodBias,
                            bool enableAnisotropy,
                            float maxAnisotropy,
                            bool enableComparison,
                            ComparisonOperation comparisonOp,
                            float mipmapMinLod,
                            float mipmapMaxLod,
                            BorderColor borderColor,
                            bool unnormalizedCoordinates);
  static void destroySampler(VkSampler sampler);
  static void createBuffer(VkBuffer& buffer,
                           VkDeviceMemory& bufferMemory,
                           BufferUsage usageFlags,
                           MemoryProperty propertyFlags,
                           std::size_t bufferSize);
  static void createStagedBuffer(VkBuffer& buffer,
                                 VkDeviceMemory& bufferMemory,
                                 BufferUsage bufferType,
                                 const void* bufferData,
                                 std::size_t bufferSize,
                                 VkQueue queue,
                                 VkCommandPool commandPool);
  static void copyBuffer(VkBuffer srcBuffer,
                         VkBuffer dstBuffer,
                         VkDeviceSize bufferSize,
                         VkCommandPool commandPool,
                         VkQueue queue);
  static void copyBuffer(VkBuffer srcBuffer,
                         VkImage dstImage,
                         ImageAspect imgAspect,
                         uint32_t imgWidth,
                         uint32_t imgHeight,
                         uint32_t imgDepth,
                         ImageLayout imgLayout,
                         VkCommandPool commandPool,
                         VkQueue queue);
  static void destroyBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);
  static void beginCommandBuffer(VkCommandBuffer& commandBuffer,
                                 VkCommandPool commandPool,
                                 CommandBufferLevel commandBufferLevel,
                                 CommandBufferUsage commandBufferUsage);
  static void endCommandBuffer(VkCommandBuffer& commandBuffer, VkQueue queue, VkCommandPool commandPool);
  static void createDescriptorPool(VkDescriptorPool& descriptorPool,
                                   uint32_t maxSetCount,
                                   std::initializer_list<VkDescriptorPoolSize> poolSizes);

  static void recreateSwapchain();
  static void drawFrame();
  /// Destroys the renderer, deallocating Vulkan resources.
  static void destroy();

  // TODO: temporary GL functions to allow compilation

  static unsigned int getCurrentProgram() { return 0; }
  static void clearColor(float, float, float, float) {}
  static void clear(MaskType) {}
  static void clear(MaskType, MaskType) {}
  static void setDepthFunction(DepthFunction) {}
  static void setFaceCulling(CullingMode) {}
  static void setPolygonMode(CullingMode, PolygonMode) {}
  static void recoverFrame(unsigned int, unsigned int, TextureFormat, TextureDataType, void*) {}
  static void generateBuffer(unsigned int&) {}
  static void bindBuffer(BufferType, unsigned int) {}
  static void unbindBuffer(BufferType) {}
  static void bindBufferBase(BufferType, unsigned int, unsigned int) {}
  static void bindBufferRange(BufferType, unsigned int, unsigned int, std::ptrdiff_t, std::ptrdiff_t) {}
  static void sendBufferData(BufferType, std::ptrdiff_t, const void*, BufferDataUsage) {}
  static void sendBufferSubData(BufferType, std::ptrdiff_t, std::ptrdiff_t, const void*) {}
  static void deleteBuffer(unsigned int&) {}
  static bool isTexture(unsigned int) { return true; }
  static void sendImageData2D(TextureType, unsigned int, TextureInternalFormat, unsigned int, unsigned int, TextureFormat, TextureDataType, const void*) {}
  static void generateMipmap(TextureType) {}
  static void generateTexture(unsigned int&) {}
  static void bindTexture(TextureType, unsigned int) {}
  static void unbindTexture(TextureType) {}
  static void activateTexture(unsigned int) {}
  static void setTextureParameter(TextureType, TextureParam, const int*) {}
  static void setTextureParameter(TextureType, TextureParam, TextureParamValue) {}
  static void deleteTexture(unsigned int&) {}
  static void resizeViewport(int, int, unsigned int, unsigned int) {}
  static unsigned int createProgram() { return 0; }
  static bool isProgramLinked(unsigned int) { return true; }
  static void linkProgram(unsigned int) {}
  static void useProgram(unsigned int) {}
  static void deleteProgram(unsigned int) {}
  static unsigned int createShader(ShaderType) { return 0; }
  static bool isShaderCompiled(unsigned int) { return true; }
  static void sendShaderSource(unsigned int, const char*, int) {}
  static void sendShaderSource(unsigned int, const std::string&) {}
  static void compileShader(unsigned int) {}
  static void attachShader(unsigned int, unsigned int) {}
  static void detachShader(unsigned int, unsigned int) {}
  static bool isShaderAttached(unsigned int, unsigned int) { return true; }
  static void deleteShader(unsigned int) {}
  static int recoverUniformLocation(unsigned int, const char*) { return 0; }
  static void sendUniform(int, int) {}
  static void sendUniform(int, unsigned int) {}
  static void sendUniform(int, float) {}
  static void sendUniformVector2(int, const float*, int = 1) {}
  static void sendUniformVector3(int, const float*, int = 1) {}
  static void sendUniformVector4(int, const float*, int = 1) {}
  static void sendUniformMatrix2x2(int, const float*, int = 1, bool = false) {}
  static void sendUniformMatrix3x3(int, const float*, int = 1, bool = false) {}
  static void sendUniformMatrix4x4(int, const float*, int = 1, bool = false) {}
  static void generateFramebuffer(unsigned int&) {}
  static bool isFramebufferComplete(FramebufferType = FramebufferType::FRAMEBUFFER) { return true; }
  static void bindFramebuffer(unsigned int, FramebufferType = FramebufferType::FRAMEBUFFER) {}
  static void unbindFramebuffer(FramebufferType = FramebufferType::FRAMEBUFFER) {}
  static void setFramebufferTexture2D(FramebufferAttachment, TextureType, unsigned int, int, FramebufferType = FramebufferType::FRAMEBUFFER) {}
  static void setDrawBuffers(unsigned int, const DrawBuffer*) {}
  static void deleteFramebuffer(unsigned int&) {}
  static void printErrors() {}

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) noexcept = delete;

  ~Renderer() = delete;

private:
  static constexpr int MaxFramesInFlight = 2;

  static inline bool s_isInitialized = false;

  static inline VkInstance s_instance {};
  static inline VkSurfaceKHR s_surface {};
  static inline GLFWwindow* s_windowHandle {};
  static inline VkPhysicalDevice s_physicalDevice {};
  static inline VkDevice s_logicalDevice {};
  static inline VkQueue s_graphicsQueue {};
  static inline VkQueue s_presentQueue {};

  static inline VkSwapchainKHR m_swapchain {};
  static inline std::vector<VkImage> m_swapchainImages {};
  static inline VkFormat m_swapchainImageFormat {};
  static inline VkExtent2D m_swapchainExtent {};
  static inline std::vector<VkImageView> m_swapchainImageViews {};

  static inline VkRenderPass m_renderPass {};
  static inline VkDescriptorSetLayout m_descriptorSetLayout {};
  static inline VkPipelineLayout m_pipelineLayout {};
  static inline VkPipeline m_graphicsPipeline {};
  static inline std::vector<VkFramebuffer> m_swapchainFramebuffers {};
  static inline VkCommandPool m_commandPool {};

  static inline VkImage m_textureImage {};
  static inline VkDeviceMemory m_textureMemory {};
  static inline VkImageView m_textureImageView {};
  static inline VkSampler m_textureSampler {};
  static inline VkBuffer m_vertexBuffer {};
  static inline VkDeviceMemory m_vertexBufferMemory {};
  static inline VkBuffer m_indexBuffer {};
  static inline VkDeviceMemory m_indexBufferMemory {};
  static inline std::vector<VkBuffer> m_uniformBuffers {};
  static inline std::vector<VkDeviceMemory> m_uniformBuffersMemory {};
  static inline VkDescriptorPool m_descriptorPool {};
  static inline std::vector<VkDescriptorSet> m_descriptorSets {};
  static inline std::vector<VkCommandBuffer> m_commandBuffers {};

  static inline std::array<VkSemaphore, MaxFramesInFlight> m_imageAvailableSemaphores {};
  static inline std::array<VkSemaphore, MaxFramesInFlight> m_renderFinishedSemaphores {};
  static inline std::array<VkFence, MaxFramesInFlight> m_inFlightFences {};
  static inline std::vector<VkFence> m_imagesInFlight {};
  static inline std::size_t m_currentFrameIndex = 0;

  static inline bool m_framebufferResized = false;

#if !defined(NDEBUG)
  static inline VkDebugUtilsMessengerEXT m_debugMessenger {};
#endif
};

} // namespace Raz

#endif // RAZ_RENDERERVK_HPP
