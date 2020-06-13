#pragma once

#ifndef RAZ_RENDERER_HPP
#define RAZ_RENDERER_HPP

#include "RaZ/Utils/EnumUtils.hpp"

#include <bitset>
#include <cstddef>
#include <string>

namespace Raz {

enum class Capability : unsigned int {
  CULL           = 2884,  // GL_CULL_FACE
  DITHER         = 3024,  // GL_DITHER
  BLEND          = 3042,  // GL_BLEND
  COLOR_LOGIC_OP = 3058,  // GL_COLOR_LOGIC_OP

  DEPTH_CLAMP  = 34383, // GL_DEPTH_CLAMP
  DEPTH_TEST   = 2929,  // GL_DEPTH_TEST
  SCISSOR_TEST = 3089,  // GL_SCISSOR_TEST
  STENCIL_TEST = 2960,  // GL_STENCIL_TEST

  LINE_SMOOTH          = 2848,  // GL_LINE_SMOOTH
  POLYGON_SMOOTH       = 2881,  // GL_POLYGON_SMOOTH
  POLYGON_OFFSET_POINT = 10753, // GL_POLYGON_OFFSET_POINT
  POLYGON_OFFSET_LINE  = 10754, // GL_POLYGON_OFFSET_LINE
  POLYGON_OFFSET_FILL  = 32823, // GL_POLYGON_OFFSET_FILL

  MULTISAMPLE              = 32925, // GL_MULTISAMPLE
  SAMPLE_ALPHA_TO_COVERAGE = 32926, // GL_SAMPLE_ALPHA_TO_COVERAGE
  SAMPLE_ALPHA_TO_ONE      = 32927, // GL_SAMPLE_ALPHA_TO_ONE
  SAMPLE_COVERAGE          = 32928, // GL_SAMPLE_COVERAGE
  SAMPLE_SHADING           = 35894, // GL_SAMPLE_SHADING
  SAMPLE_MASK              = 36433, // GL_SAMPLE_MASK

  POINT_SIZE        = 34370, // GL_PROGRAM_POINT_SIZE
  CUBEMAP_SEAMLESS  = 34895, // GL_TEXTURE_CUBE_MAP_SEAMLESS
  FRAMEBUFFER_SRGB  = 36281, // GL_FRAMEBUFFER_SRGB
  PRIMITIVE_RESTART = 36765, // GL_PRIMITIVE_RESTART

  DEBUG_OUTPUT             = 37600, // GL_DEBUG_OUTPUT
  DEBUG_OUTPUT_SYNCHRONOUS = 33346  // GL_DEBUG_OUTPUT_SYNCHRONOUS
};

enum class StateParameter : unsigned int {
  ACTIVE_TEXTURE                = 34016                                                 /* GL_ACTIVE_TEXTURE                */, ///< Currently active texture.
  ALIASED_LINE_WIDTH_RANGE      = 33902                                                 /* GL_ALIASED_LINE_WIDTH_RANGE      */, ///<
  SMOOTH_LINE_WIDTH_RANGE       = 2850                                                  /* GL_SMOOTH_LINE_WIDTH_RANGE       */, ///<
  SMOOTH_LINE_WIDTH_GRANULARITY = 2851                                                  /* GL_SMOOTH_LINE_WIDTH_GRANULARITY */, ///<
  ARRAY_BUFFER_BINDING          = 34964                                                 /* GL_ARRAY_BUFFER_BINDING          */, ///<
  BLEND                         = static_cast<unsigned int>(Capability::BLEND)          /* GL_BLEND                         */, ///< Blending.
  BLEND_COLOR                   = 32773                                                 /* GL_BLEND_COLOR                   */, ///<
  BLEND_DST_RGB                 = 32968                                                 /* GL_BLEND_DST_RGB                 */, ///<
  BLEND_DST_ALPHA               = 32970                                                 /* GL_BLEND_DST_ALPHA               */, ///<
  BLEND_SRC_RGB                 = 32969                                                 /* GL_BLEND_SRC_RGB                 */, ///<
  BLEND_SRC_ALPHA               = 32971                                                 /* GL_BLEND_SRC_ALPHA               */, ///<
  BLEND_EQUATION_RGB            = 32777                                                 /* GL_BLEND_EQUATION_RGB            */, ///<
  BLEND_EQUATION_ALPHA          = 34877                                                 /* GL_BLEND_EQUATION_ALPHA          */, ///<
  COLOR_CLEAR_VALUE             = 3106                                                  /* GL_COLOR_CLEAR_VALUE             */, ///< Clear color.
  COLOR_LOGIC_OP                = static_cast<unsigned int>(Capability::COLOR_LOGIC_OP) /* GL_COLOR_LOGIC_OP                */, ///<
  COLOR_WRITEMASK               = 3107                                                  /* GL_COLOR_WRITEMASK               */, ///< Color write mask.
  COMPRESSED_TEXTURE_FORMATS    = 34467                                                 /* GL_COMPRESSED_TEXTURE_FORMATS    */, ///<
  CULL_FACE                     = static_cast<unsigned int>(Capability::CULL)           /* GL_CULL_FACE                     */, ///< Polygon culling.
  CURRENT_PROGRAM               = 35725                                                 /* GL_CURRENT_PROGRAM               */, ///< Currently used program.
  DEPTH_CLEAR_VALUE             = 2931                                                  /* GL_DEPTH_CLEAR_VALUE             */, ///< Depth clear value.
  DEPTH_FUNC                    = 2932                                                  /* GL_DEPTH_FUNC                    */, ///< Depth function.
  DEPTH_RANGE                   = 2928                                                  /* GL_DEPTH_RANGE                   */, ///< Depth range.
  DEPTH_TEST                    = static_cast<unsigned int>(Capability::DEPTH_TEST)     /* GL_DEPTH_TEST                    */, ///< Depth testing.
  DEPTH_WRITEMASK               = 2930                                                  /* GL_DEPTH_WRITEMASK               */, ///< Depth write mask.
  DITHER                        = static_cast<unsigned int>(Capability::DITHER)         /* GL_DITHER                        */, ///< Dithering.
  POINT_SIZE                    = static_cast<unsigned int>(Capability::POINT_SIZE)     /* GL_POINT_SIZE                    */  ///< Point size.
};

enum class MaskType : unsigned int {
  COLOR   = 16384, // GL_COLOR_BUFFER_BIT
  DEPTH   = 256,   // GL_DEPTH_BUFFER_BIT
  STENCIL = 1024   // GL_STENCIL_BUFFER_BIT
};
MAKE_ENUM_FLAG(MaskType)

enum class DepthFunction : unsigned int {
  NEVER         = 512, // GL_NEVER
  EQUAL         = 514, // GL_EQUAL
  NOT_EQUAL     = 517, // GL_NOTEQUAL
  LESS          = 513, // GL_LESS
  LESS_EQUAL    = 515, // GL_LEQUAL
  GREATER       = 516, // GL_GREATER
  GREATER_EQUAL = 518, // GL_GEQUAL
  ALWAYS        = 519  // GL_ALWAYS
};

enum class FaceOrientation : unsigned int {
  FRONT      = 1028, // GL_FRONT
  BACK       = 1029, // GL_BACK
  FRONT_BACK = 1032  // GL_FRONT_AND_BACK
};

enum class PolygonMode : unsigned int {
  POINT = 6912, // GL_POINT
  LINE  = 6913, // GL_LINE
  FILL  = 6914  // GL_FILL
};

enum class PixelStorage : unsigned int {
  UNPACK_SWAP_BYTES   = 3312,  // GL_UNPACK_SWAP_BYTES
  UNPACK_LSB_FIRST    = 3313,  // GL_UNPACK_LSB_FIRST
  UNPACK_ROW_LENGTH   = 3314,  // GL_UNPACK_ROW_LENGTH
  UNPACK_SKIP_ROWS    = 3315,  // GL_UNPACK_SKIP_ROWS
  UNPACK_SKIP_PIXELS  = 3316,  // GL_UNPACK_SKIP_PIXELS
  UNPACK_SKIP_IMAGES  = 32877, // GL_PACK_SKIP_IMAGES
  UNPACK_IMAGE_HEIGHT = 32878, // GL_PACK_IMAGE_HEIGHT
  UNPACK_ALIGNMENT    = 3317,  // GL_UNPACK_ALIGNMENT

  PACK_SWAP_BYTES     = 3328,  // GL_PACK_SWAP_BYTES
  PACK_LSB_FIRST      = 3329,  // GL_PACK_LSB_FIRST
  PACK_ROW_LENGTH     = 3330,  // GL_PACK_ROW_LENGTH
  PACK_SKIP_ROWS      = 3331,  // GL_PACK_SKIP_ROWS
  PACK_SKIP_PIXELS    = 3332,  // GL_PACK_SKIP_PIXELS
  PACK_SKIP_IMAGES    = 32875, // GL_PACK_SKIP_IMAGES
  PACK_IMAGE_HEIGHT   = 32876, // GL_PACK_IMAGE_HEIGHT
  PACK_ALIGNMENT      = 3333   // GL_PACK_ALIGNMENT
};

enum class BufferType : unsigned int {
  ARRAY_BUFFER   = 34962, // GL_ARRAY_BUFFER
  ELEMENT_BUFFER = 34963, // GL_ELEMENT_ARRAY_BUFFER
  UNIFORM_BUFFER = 35345  // GL_UNIFORM_BUFFER
};

enum class BufferDataUsage : unsigned int {
  STREAM_DRAW  = 35040, // GL_STREAM_DRAW
  STREAM_READ  = 35041, // GL_STREAM_READ
  STREAM_COPY  = 35042, // GL_STREAM_COPY

  STATIC_DRAW  = 35044, // GL_STATIC_DRAW
  STATIC_READ  = 35045, // GL_STATIC_READ
  STATIC_COPY  = 35046, // GL_STATIC_COPY

  DYNAMIC_DRAW = 35048, // GL_DYNAMIC_DRAW
  DYNAMIC_READ = 35049, // GL_DYNAMIC_READ
  DYNAMIC_COPY = 35050, // GL_DYNAMIC_COPY
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
  NEAREST                = 9728,  // GL_NEAREST
  LINEAR                 = 9729,  // GL_LINEAR
  NEAREST_MIPMAP_NEAREST = 9984,  // GL_NEAREST_MIPMAP_NEAREST
  LINEAR_MIPMAP_NEAREST  = 9985,  // GL_LINEAR_MIPMAP_NEAREST
  NEAREST_MIPMAP_LINEAR  = 9986,  // GL_NEAREST_MIPMAP_LINEAR
  LINEAR_MIPMAP_LINEAR   = 9987,  // GL_LINEAR_MIPMAP_LINEAR
  REPEAT                 = 10497, // GL_REPEAT
  CLAMP_TO_EDGE          = 33071  // GL_CLAMP_TO_EDGE
};

enum class TextureAttribute : unsigned int {
  WIDTH                 = 4096,  // GL_TEXTURE_WIDTH
  HEIGHT                = 4097,  // GL_TEXTURE_HEIGHT
  DEPTH                 = 32881, // GL_TEXTURE_DEPTH
  INTERNAL_FORMAT       = 4099,  // GL_TEXTURE_INTERNAL_FORMAT
  RED_SIZE              = 32860, // GL_TEXTURE_RED_SIZE
  GREEN_SIZE            = 32861, // GL_TEXTURE_GREEN_SIZE
  BLUE_SIZE             = 32862, // GL_TEXTURE_BLUE_SIZE
  ALPHA_SIZE            = 32863, // GL_TEXTURE_ALPHA_SIZE
  DEPTH_SIZE            = 34890, // GL_TEXTURE_DEPTH_SIZE
  COMPRESSED            = 34465, // GL_TEXTURE_COMPRESSED
  COMPRESSED_IMAGE_SIZE = 34464  // GL_TEXTURE_COMPRESSED_IMAGE_SIZE
};

enum class TextureFormat : unsigned int {
  RED           = 6403,  // GL_RED
  GREEN         = 6404,  // GL_GREEN
  BLUE          = 6405,  // GL_BLUE
  RG            = 33319, // GL_RG
  RGB           = 6407,  // GL_RGB
  BGR           = 32992, // GL_BGR
  RGBA          = 6408,  // GL_RGBA
  BGRA          = 32993, // GL_BGRA
  SRGB          = 35904, // GL_SRGB
  DEPTH         = 6402,  // GL_DEPTH_COMPONENT
  STENCIL       = 6401,  // GL_STENCIL_INDEX
  DEPTH_STENCIL = 34041  // GL_DEPTH_STENCIL
};

enum class TextureInternalFormat : unsigned int {
  // Base formats
  RED           = static_cast<unsigned int>(TextureFormat::RED),
  RG            = static_cast<unsigned int>(TextureFormat::RG),
  RGB           = static_cast<unsigned int>(TextureFormat::RGB),
  BGR           = static_cast<unsigned int>(TextureFormat::BGR),
  RGBA          = static_cast<unsigned int>(TextureFormat::RGBA),
  BGRA          = static_cast<unsigned int>(TextureFormat::BGRA),
  SRGB          = static_cast<unsigned int>(TextureFormat::SRGB),
  DEPTH         = static_cast<unsigned int>(TextureFormat::DEPTH),
  DEPTH_STENCIL = static_cast<unsigned int>(TextureFormat::DEPTH_STENCIL),

  // Sized formats
  RED16F   = 33325, // GL_R16F
  RG16F    = 33327, // GL_RG16F
  RGB16F   = 34843, // GL_RGB16F
  RGBA16F  = 34842, // GL_RGBA16F
  DEPTH32F = 36012, // GL_DEPTH_COMPONENT32F
};

enum class TextureDataType : unsigned int {
  UBYTE = 5121, // GL_UNSIGNED_BYTE
  FLOAT = 5126  // GL_FLOAT
};

enum class ProgramParameter : unsigned int {
  DELETE_STATUS                         = 35712, // GL_DELETE_STATUS
  LINK_STATUS                           = 35714, // GL_LINK_STATUS
  VALIDATE_STATUS                       = 35715, // GL_VALIDATE_STATUS
  INFO_LOG_LENGTH                       = 35716, // GL_INFO_LOG_LENGTH
  ATTACHED_SHADERS                      = 35717, // GL_ATTACHED_SHADERS
  ACTIVE_ATTRIBUTES                     = 35721, // GL_ACTIVE_ATTRIBUTES
  ACTIVE_ATTRIBUTE_MAX_LENGTH           = 35722, // GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
  ACTIVE_UNIFORMS                       = 35718, // GL_ACTIVE_UNIFORMS
  ACTIVE_UNIFORM_MAX_LENGTH             = 35719, // GL_ACTIVE_UNIFORM_MAX_LENGTH
  ACTIVE_UNIFORM_BLOCKS                 = 35382, // GL_ACTIVE_UNIFORM_BLOCKS
  ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH  = 35381, // GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH
  TRANSFORM_FEEDBACK_BUFFER_MODE        = 35967, // GL_TRANSFORM_FEEDBACK_BUFFER_MODE
  TRANSFORM_FEEDBACK_VARYINGS           = 35971, // GL_TRANSFORM_FEEDBACK_VARYINGS
  TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH = 35958, // GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
  GEOMETRY_VERTICES_OUT                 = 35094, // GL_GEOMETRY_VERTICES_OUT
  GEOMETRY_INPUT_TYPE                   = 35095, // GL_GEOMETRY_INPUT_TYPE
  GEOMETRY_OUTPUT_TYPE                  = 35096  // GL_GEOMETRY_OUTPUT_TYPE
};

enum class ShaderType : unsigned int {
  VERTEX   = 35633, // GL_VERTEX_SHADER
  FRAGMENT = 35632, // GL_FRAGMENT_SHADER
  GEOMETRY = 36313  // GL_GEOMETRY_SHADER
};

enum class ShaderStatus : unsigned int {
  COMPILE = 35713 // GL_COMPILE_STATUS
};

enum class UniformType : unsigned int {
  // Primitive types
  FLOAT  = 5126,  // GL_FLOAT
#if defined(RAZ_USE_GL4)
  DOUBLE = 5130,  // GL_DOUBLE
#endif
  INT    = 5124,  // GL_INT
  UINT   = 5125,  // GL_UNSIGNED_INT
  BOOL   = 35670, // GL_BOOL

  // Vectors
  VEC2  = 35664, // GL_FLOAT_VEC2
  VEC3  = 35665, // GL_FLOAT_VEC3
  VEC4  = 35666, // GL_FLOAT_VEC4
#if defined(RAZ_USE_GL4)
  DVEC2 = 36860, // GL_DOUBLE_VEC2
  DVEC3 = 36861, // GL_DOUBLE_VEC3
  DVEC4 = 36862, // GL_DOUBLE_VEC4
#endif
  IVEC2 = 35667, // GL_INT_VEC2
  IVEC3 = 35668, // GL_INT_VEC3
  IVEC4 = 35669, // GL_INT_VEC4
  UVEC2 = 36294, // GL_UNSIGNED_INT_VEC2
  UVEC3 = 36295, // GL_UNSIGNED_INT_VEC3
  UVEC4 = 36296, // GL_UNSIGNED_INT_VEC4
  BVEC2 = 35671, // GL_BOOL_VEC2
  BVEC3 = 35672, // GL_BOOL_VEC3
  BVEC4 = 35673, // GL_BOOL_VEC4

  // Matrices
  MAT2    = 35674, // GL_FLOAT_MAT2
  MAT3    = 35675, // GL_FLOAT_MAT3
  MAT4    = 35676, // GL_FLOAT_MAT4
  MAT2x3  = 35685, // GL_FLOAT_MAT2x3
  MAT2x4  = 35686, // GL_FLOAT_MAT2x4
  MAT3x2  = 35687, // GL_FLOAT_MAT3x2
  MAT3x4  = 35688, // GL_FLOAT_MAT3x4
  MAT4x2  = 35689, // GL_FLOAT_MAT4x2
  MAT4x3  = 35690, // GL_FLOAT_MAT4x3
#if defined(RAZ_USE_GL4)
  DMAT2   = 36678, // GL_DOUBLE_MAT2
  DMAT3   = 36679, // GL_DOUBLE_MAT3
  DMAT4   = 36680, // GL_DOUBLE_MAT4
  DMAT2x3 = 36681, // GL_DOUBLE_MAT2x3
  DMAT2x4 = 36682, // GL_DOUBLE_MAT2x4
  DMAT3x2 = 36683, // GL_DOUBLE_MAT3x2
  DMAT3x4 = 36684, // GL_DOUBLE_MAT3x4
  DMAT4x2 = 36685, // GL_DOUBLE_MAT4x2
  DMAT4x3 = 36686, // GL_DOUBLE_MAT4x3
#endif

  // Samplers
  SAMPLER_1D                                = 35677, // GL_SAMPLER_1D
  SAMPLER_2D                                = 35678, // GL_SAMPLER_2D
  SAMPLER_3D                                = 35679, // GL_SAMPLER_3D
  SAMPLER_CUBE                              = 35680, // GL_SAMPLER_CUBE
  SAMPLER_1D_SHADOW                         = 35681, // GL_SAMPLER_1D_SHADOW
  SAMPLER_2D_SHADOW                         = 35682, // GL_SAMPLER_2D_SHADOW
  SAMPLER_1D_ARRAY                          = 36288, // GL_SAMPLER_1D_ARRAY
  SAMPLER_2D_ARRAY                          = 36289, // GL_SAMPLER_2D_ARRAY
  SAMPLER_1D_ARRAY_SHADOW                   = 36291, // GL_SAMPLER_1D_ARRAY_SHADOW
  SAMPLER_2D_ARRAY_SHADOW                   = 36292, // GL_SAMPLER_2D_ARRAY_SHADOW
  SAMPLER_2D_MULTISAMPLE                    = 37128, // GL_SAMPLER_2D_MULTISAMPLE
  SAMPLER_2D_MULTISAMPLE_ARRAY              = 37131, // GL_SAMPLER_2D_MULTISAMPLE_ARRAY
  SAMPLER_CUBE_SHADOW                       = 36293, // GL_SAMPLER_CUBE_SHADOW
  SAMPLER_BUFFER                            = 36290, // GL_SAMPLER_BUFFER
  SAMPLER_2D_RECT                           = 35683, // GL_SAMPLER_2D_RECT
  SAMPLER_2D_RECT_SHADOW                    = 35684, // GL_SAMPLER_2D_RECT_SHADOW
  INT_SAMPLER_1D                            = 36297, // GL_INT_SAMPLER_1D
  INT_SAMPLER_2D                            = 36298, // GL_INT_SAMPLER_2D
  INT_SAMPLER_3D                            = 36299, // GL_INT_SAMPLER_3D
  INT_SAMPLER_CUBE                          = 36300, // GL_INT_SAMPLER_CUBE
  INT_SAMPLER_1D_ARRAY                      = 36302, // GL_INT_SAMPLER_1D_ARRAY
  INT_SAMPLER_2D_ARRAY                      = 36303, // GL_INT_SAMPLER_2D_ARRAY
  INT_SAMPLER_2D_MULTISAMPLE                = 37129, // GL_INT_SAMPLER_2D_MULTISAMPLE
  INT_SAMPLER_2D_MULTISAMPLE_ARRAY          = 37132, // GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
  INT_SAMPLER_BUFFER                        = 36304, // GL_INT_SAMPLER_BUFFER
  INT_SAMPLER_2D_RECT                       = 36301, // GL_INT_SAMPLER_2D_RECT
  UNSIGNED_INT_SAMPLER_1D                   = 36305, // GL_UNSIGNED_INT_SAMPLER_1D
  UNSIGNED_INT_SAMPLER_2D                   = 36306, // GL_UNSIGNED_INT_SAMPLER_2D
  UNSIGNED_INT_SAMPLER_3D                   = 36307, // GL_UNSIGNED_INT_SAMPLER_3D
  UNSIGNED_INT_SAMPLER_CUBE                 = 36308, // GL_UNSIGNED_INT_SAMPLER_CUBE
  UNSIGNED_INT_SAMPLER_1D_ARRAY             = 36310, // GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
  UNSIGNED_INT_SAMPLER_2D_ARRAY             = 36311, // GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
  UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE       = 37130, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
  UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 37133, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
  UNSIGNED_INT_SAMPLER_BUFFER               = 36312, // GL_UNSIGNED_INT_SAMPLER_BUFFER
  UNSIGNED_INT_SAMPLER_2D_RECT              = 36309  // GL_UNSIGNED_INT_SAMPLER_2D_RECT
};

enum class FramebufferType : unsigned int {
  READ_FRAMEBUFFER = 36008, // GL_READ_FRAMEBUFFER
  DRAW_FRAMEBUFFER = 36009, // GL_DRAW_FRAMEBUFFER
  FRAMEBUFFER      = 36160  // GL_FRAMEBUFFER
};

enum class FramebufferStatus : unsigned int {
  COMPLETE                      = 36053, // GL_FRAMEBUFFER_COMPLETE
  UNDEFINED                     = 33305, // GL_FRAMEBUFFER_UNDEFINED
  INCOMPLETE_ATTACHMENT         = 36054, // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
  INCOMPLETE_MISSING_ATTACHMENT = 36055, // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
  INCOMPLETE_DRAW_BUFFER        = 36059, // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
  INCOMPLETE_READ_BUFFER        = 36060, // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
  UNSUPPORTED                   = 36061, // GL_FRAMEBUFFER_UNSUPPORTED
  INCOMPLETE_MULTISAMPLE        = 36182, // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
  INCOMPLETE_LAYER_TARGETS      = 36264  // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
};

enum class FramebufferAttachment : unsigned int {
  DEPTH         = 36096, // GL_DEPTH_ATTACHMENT
  STENCIL       = 36128, // GL_STENCIL_ATTACHMENT
  DEPTH_STENCIL = 33306, // GL_DEPTH_STENCIL_ATTACHMENT

  COLOR0 = 36064, // GL_COLOR_ATTACHMENT0
  COLOR1 = 36065, // GL_COLOR_ATTACHMENT1
  COLOR2 = 36066, // GL_COLOR_ATTACHMENT2
  COLOR3 = 36067, // GL_COLOR_ATTACHMENT3
  COLOR4 = 36068, // GL_COLOR_ATTACHMENT4
  COLOR5 = 36069, // GL_COLOR_ATTACHMENT5
  COLOR6 = 36070, // GL_COLOR_ATTACHMENT6
  COLOR7 = 36071  // GL_COLOR_ATTACHMENT7
};

enum class DrawBuffer : unsigned int {
  NONE        = 0,     // GL_NONE
  FRONT_LEFT  = 1024,  // GL_FRONT_LEFT
  FRONT_RIGHT = 1025,  // GL_FRONT_RIGHT
  BACK_LEFT   = 1026,  // GL_BACK_LEFT
  BACK_RIGHT  = 1027,  // GL_BACK_RIGHT

  COLOR_ATTACHMENT0 = static_cast<unsigned int>(FramebufferAttachment::COLOR0),
  COLOR_ATTACHMENT1 = static_cast<unsigned int>(FramebufferAttachment::COLOR1),
  COLOR_ATTACHMENT2 = static_cast<unsigned int>(FramebufferAttachment::COLOR2),
  COLOR_ATTACHMENT3 = static_cast<unsigned int>(FramebufferAttachment::COLOR3),
  COLOR_ATTACHMENT4 = static_cast<unsigned int>(FramebufferAttachment::COLOR4),
  COLOR_ATTACHMENT5 = static_cast<unsigned int>(FramebufferAttachment::COLOR5),
  COLOR_ATTACHMENT6 = static_cast<unsigned int>(FramebufferAttachment::COLOR6),
  COLOR_ATTACHMENT7 = static_cast<unsigned int>(FramebufferAttachment::COLOR7)
};

enum class ErrorCode : unsigned int {
  INVALID_ENUM                  = 1280, // GL_INVALID_ENUM
  INVALID_VALUE                 = 1281, // GL_INVALID_VALUE
  INVALID_OPERATION             = 1282, // GL_INVALID_OPERATION
  STACK_OVERFLOW                = 1283, // GL_STACK_OVERFLOW
  STACK_UNDERFLOW               = 1284, // GL_STACK_UNDERFLOW
  OUT_OF_MEMORY                 = 1285, // GL_OUT_OF_MEMORY
  INVALID_FRAMEBUFFER_OPERATION = 1286, // GL_INVALID_FRAMEBUFFER_OPERATION
#ifdef RAZ_USE_GL4
  CONTEXT_LOST                  = 1287, // GL_CONTEXT_LOST
#endif
  NONE                          = 0     // GL_NO_ERROR
};

#ifdef RAZ_USE_GL4
using ErrorCodes = std::bitset<8>;
#else
using ErrorCodes = std::bitset<7>;
#endif

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void initialize();
  static bool isInitialized() { return s_isInitialized; }
  static void enable(Capability capability);
  static void disable(Capability capability);
  static bool isEnabled(Capability capability);
  static void getParameter(StateParameter parameter, unsigned char* values);
  static void getParameter(StateParameter parameter, int* values);
  static void getParameter(StateParameter parameter, int64_t* values);
  static void getParameter(StateParameter parameter, float* values);
  static void getParameter(StateParameter parameter, double* values);
  static void getParameter(StateParameter parameter, unsigned int index, unsigned char* values);
  static void getParameter(StateParameter parameter, unsigned int index, int* values);
  static void getParameter(StateParameter parameter, unsigned int index, int64_t* values);
  /// Gets the active texture's index.
  /// \note This returns an index starting from 0, not from GL_TEXTURE0.
  /// \return Index of the currently active texture.
  static unsigned int getActiveTexture();
  /// Gets the current program's index.
  /// \return Index of the currently used program.
  static unsigned int getCurrentProgram();
  static void clearColor(float red, float green, float blue, float alpha);
  static void clearColor(float values[4]) { clearColor(values[0], values[1], values[2], values[3]); }
  static void clear(MaskType mask);
  static void setDepthFunction(DepthFunction func);
  static void setFaceCulling(FaceOrientation orientation);
#if !defined(USE_OPENGL_ES) // glPolygonMode is not available with OpenGL ES
  static void setPolygonMode(FaceOrientation orientation, PolygonMode mode);
#endif
  static void setPixelStorage(PixelStorage storage, unsigned int value);
  static void recoverFrame(unsigned int width, unsigned int height, TextureFormat format, TextureDataType dataType, void* data);
  static void generateBuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void generateBuffers(unsigned int (&indices)[N]) { generateBuffers(N, indices); }
  static void generateBuffer(unsigned int& index) { generateBuffers(1, &index); }
  static void bindBuffer(BufferType type, unsigned int index);
  static void unbindBuffer(BufferType type) { bindBuffer(type, 0); }
  static void bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex);
  static void bindBufferRange(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex, std::ptrdiff_t offset, std::ptrdiff_t size);
  static void sendBufferData(BufferType type, std::ptrdiff_t size, const void* data, BufferDataUsage usage);
  static void sendBufferSubData(BufferType type, std::ptrdiff_t offset, std::ptrdiff_t dataSize, const void* data);
  template <typename T> static void sendBufferSubData(BufferType type, std::ptrdiff_t offset, const T& data) { sendBufferSubData(type, offset, sizeof(T), &data); }
  static void deleteBuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteBuffers(unsigned int (&indices)[N]) { deleteBuffers(N, indices); }
  static void deleteBuffer(unsigned int& index) { deleteBuffers(1, &index); }
  static void generateTextures(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void generateTextures(unsigned int (&indices)[N]) { generateTextures(N, indices); }
  static void generateTexture(unsigned int& index) { generateTextures(1, &index); }
  static void activateTexture(unsigned int index);
  static void setTextureParameter(TextureType type, TextureParam param, int value);
  static void setTextureParameter(TextureType type, TextureParam param, float value);
  static void setTextureParameter(TextureType type, TextureParam param, const int* values);
  static void setTextureParameter(TextureType type, TextureParam param, const float* values);
  static void setTextureParameter(TextureType type, TextureParam param, TextureParamValue value) { setTextureParameter(type, param, static_cast<int>(value)); }
#ifdef RAZ_USE_GL4
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, int value);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, float value);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, const int* values);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, const float* values);
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, TextureParamValue value) { setTextureParameter(textureIndex, param,
                                                                                                                                static_cast<int>(value)); }
#endif
  /// Sends the image's data corresponding to the currently bound texture.
  /// \param type Type of the texture.
  /// \param mipmapLevel Mipmap (level of detail) of the texture. 0 is the most detailed.
  /// \param internalFormat Image internal format.
  /// \param width Image width.
  /// \param height Image height.
  /// \param format Image format.
  /// \param dataType Type of the data to be sent.
  /// \param data Data to be sent.
  static void sendImageData2D(TextureType type,
                              unsigned int mipmapLevel,
                              TextureInternalFormat internalFormat,
                              unsigned int width, unsigned int height,
                              TextureFormat format,
                              TextureDataType dataType, const void* data);
  static void recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, int* values);
  static void recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, float* values);
  static int recoverTextureWidth(TextureType type, unsigned int mipmapLevel = 0);
  static int recoverTextureHeight(TextureType type, unsigned int mipmapLevel = 0);
  static int recoverTextureDepth(TextureType type, unsigned int mipmapLevel = 0);
  static TextureInternalFormat recoverTextureInternalFormat(TextureType type, unsigned int mipmapLevel = 0);
  static void recoverTextureData(TextureType type, unsigned int mipmapLevel, TextureFormat format, TextureDataType dataType, void* data);
  /// Generate mipmaps (levels of detail) of the currently bound texture.
  /// \param type Type of the texture to generate mipmaps from.
  static void generateMipmap(TextureType type);
#ifdef RAZ_USE_GL4
  static void generateMipmap(unsigned int textureIndex);
#endif
  static void bindTexture(TextureType type, unsigned int index);
  static void unbindTexture(TextureType type) { bindTexture(type, 0); }
  static void deleteTextures(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteTextures(unsigned int (&indices)[N]) { deleteTextures(N, indices); }
  static void deleteTexture(unsigned int& index) { deleteTextures(1, &index); }
  static void resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height);
  static unsigned int createProgram();
  static void getProgramParameter(unsigned int index, ProgramParameter parameter, int* parameters);
  static bool isProgramLinked(unsigned int index);
  static unsigned int recoverActiveUniformCount(unsigned int index);
  static void linkProgram(unsigned int index);
  static void useProgram(unsigned int index);
  static void deleteProgram(unsigned int index);
  static unsigned int createShader(ShaderType type);
  static int getShaderStatus(unsigned int index, ShaderStatus status);
  static bool isShaderCompiled(unsigned int index);
  static void sendShaderSource(unsigned int index, const char* source, int length);
  static void sendShaderSource(unsigned int index, const std::string& source) { sendShaderSource(index, source.c_str(), static_cast<int>(source.size())); }
  static void compileShader(unsigned int index);
  static void attachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void detachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void deleteShader(unsigned int index);
  /// Gets the uniform's location (ID) corresponding to the given name.
  /// \note Location will be -1 if the name is incorrect or if the uniform isn't used in the shader(s) (will be optimized out).
  /// \param programIndex Index of the shader program to which is bound the uniform.
  /// \param uniformName Name of the uniform to recover the location from.
  /// \return Location (ID) of the uniform.
  static int recoverUniformLocation(unsigned int programIndex, const char* uniformName);
  /// Gets the uniform's information (type, name & size).
  /// \param programIndex Index of the program to recover the uniform from.
  /// \param uniformIndex Index of the uniform to recover. This is NOT the uniform's location, rather the actual index from 0 to the uniform count.
  /// \param type Type of the uniform to recover.
  /// \param name Name of the uniform to recover.
  /// \param size Size of the uniform to recover. Will be 1 for non-array uniforms, greater for arrays.
  static void recoverUniformInfo(unsigned int programIndex, unsigned int uniformIndex, UniformType& type, std::string& name, int* size = nullptr);
  static UniformType recoverUniformType(unsigned int programIndex, unsigned int uniformIndex);
  static std::string recoverUniformName(unsigned int programIndex, unsigned int uniformIndex);
  /// Sends an integer as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param value Integer to be sent.
  static void sendUniform(int uniformIndex, int value);
  /// Sends an unsigned integer as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param value Unsigned integer to be sent.
  static void sendUniform(int uniformIndex, unsigned int value);
  /// Sends a floating-point value as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param value Floating-point value to be sent.
  static void sendUniform(int uniformIndex, float value);
  /// Sends a floating-point 1D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector1(int uniformIndex, const float* values, int count = 1);
  /// Sends a floating-point 2D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector2(int uniformIndex, const float* values, int count = 1);
  /// Sends a floating-point 3D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector3(int uniformIndex, const float* values, int count = 1);
  /// Sends a floating-point 4D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector4(int uniformIndex, const float* values, int count = 1);
  /// Sends a floating-point 2x2 matrix as uniform.
  /// \param uniformIndex Index of the uniform to send the matrix's data to.
  /// \param values Array of values to be sent.
  /// \param count Number of matrices to be sent.
  /// \param transpose Defines whether the matrix should be transposed when sent; false if sending it as column-major, true if row-major.
  static void sendUniformMatrix2x2(int uniformIndex, const float* values, int count = 1, bool transpose = false);
  /// Sends a floating-point 3x3 matrix as uniform.
  /// \param uniformIndex Index of the uniform to send the matrix's data to.
  /// \param values Array of values to be sent.
  /// \param count Number of matrices to be sent.
  /// \param transpose Defines whether the matrix should be transposed when sent; false if sending it as column-major, true if row-major.
  static void sendUniformMatrix3x3(int uniformIndex, const float* values, int count = 1, bool transpose = false);
  /// Sends a floating-point 4x4 matrix as uniform.
  /// \param uniformIndex Index of the uniform to send the matrix's data to.
  /// \param values Array of values to be sent.
  /// \param count Number of matrices to be sent.
  /// \param transpose Defines whether the matrix should be transposed when sent; false if sending it as column-major, true if row-major.
  static void sendUniformMatrix4x4(int uniformIndex, const float* values, int count = 1, bool transpose = false);
  static void generateFramebuffers(int count, unsigned int* indices);
  template <std::size_t N> static void generateFramebuffers(unsigned int (&indices)[N]) { generateFramebuffers(N, indices); }
  static void generateFramebuffer(unsigned int& index) { generateFramebuffers(1, &index); }
  static FramebufferStatus getFramebufferStatus(FramebufferType type = FramebufferType::FRAMEBUFFER);
  static bool isFramebufferComplete(FramebufferType type = FramebufferType::FRAMEBUFFER) { return getFramebufferStatus(type) == FramebufferStatus::COMPLETE; }
  static void setFramebufferTexture2D(FramebufferAttachment attachment,
                                      TextureType textureType, unsigned int textureIndex, int mipmapLevel,
                                      FramebufferType type = FramebufferType::FRAMEBUFFER);
  static void setDrawBuffers(unsigned int count, const DrawBuffer* buffers);
  template <std::size_t N> static void setDrawBuffers(DrawBuffer (&buffers)[N]) { setDrawBuffers(N, buffers); }
  static void bindFramebuffer(unsigned int index, FramebufferType type = FramebufferType::FRAMEBUFFER);
  static void unbindFramebuffer(FramebufferType type = FramebufferType::FRAMEBUFFER) { bindFramebuffer(0, type); }
  static void deleteFramebuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteFramebuffers(unsigned int (&indices)[N]) { deleteFramebuffers(N, indices); }
  static void deleteFramebuffer(unsigned int& index) { deleteFramebuffers(1, &index); }
  static ErrorCodes recoverErrors();
  static bool hasErrors() { return recoverErrors().any(); }
  static void printErrors();

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) noexcept = delete;

  ~Renderer() = delete;

private:
  /// Prints OpenGL errors only in Debug mode and if SKIP_RENDERER_ERRORS hasn't been defined.
  static void printConditionalErrors() {
#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
    printErrors();
#endif
  }

  static inline bool s_isInitialized = false;
};

} // namespace Raz

#endif //RAZ_RENDERER_HPP
