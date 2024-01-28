#pragma once

#ifndef RAZ_RENDERER_HPP
#define RAZ_RENDERER_HPP

#include "RaZ/Utils/EnumUtils.hpp"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#if !defined(USE_WEBGL) && defined(EMSCRIPTEN)
#define USE_WEBGL
#endif

#if !defined(USE_OPENGL_ES) && (defined(EMSCRIPTEN) || defined(USE_WEBGL))
#define USE_OPENGL_ES
#endif

namespace Raz {

enum class Capability : unsigned int {
  CULL           = 2884 /* GL_CULL_FACE      */, ///<
  DITHER         = 3024 /* GL_DITHER         */, ///<
  BLEND          = 3042 /* GL_BLEND          */, ///<
#if !defined(USE_OPENGL_ES)
  COLOR_LOGIC_OP = 3058 /* GL_COLOR_LOGIC_OP */, ///<
#endif

#if !defined(USE_OPENGL_ES)
  DEPTH_CLAMP  = 34383 /* GL_DEPTH_CLAMP  */, ///<
#endif
  DEPTH_TEST   = 2929  /* GL_DEPTH_TEST   */, ///<
  STENCIL_TEST = 2960  /* GL_STENCIL_TEST */, ///<
  SCISSOR_TEST = 3089  /* GL_SCISSOR_TEST */, ///<

#if !defined(USE_OPENGL_ES)
  LINE_SMOOTH          = 2848  /* GL_LINE_SMOOTH          */, ///<
  POLYGON_SMOOTH       = 2881  /* GL_POLYGON_SMOOTH       */, ///<
  POLYGON_OFFSET_POINT = 10753 /* GL_POLYGON_OFFSET_POINT */, ///<
  POLYGON_OFFSET_LINE  = 10754 /* GL_POLYGON_OFFSET_LINE  */, ///<
#endif
  POLYGON_OFFSET_FILL  = 32823 /* GL_POLYGON_OFFSET_FILL  */, ///<

#if !defined(USE_OPENGL_ES)
  MULTISAMPLE              = 32925 /* GL_MULTISAMPLE              */, ///<
  SAMPLE_SHADING           = 35894 /* GL_SAMPLE_SHADING           */, ///<
  SAMPLE_ALPHA_TO_ONE      = 32927 /* GL_SAMPLE_ALPHA_TO_ONE      */, ///<
#endif
  SAMPLE_ALPHA_TO_COVERAGE = 32926 /* GL_SAMPLE_ALPHA_TO_COVERAGE */, ///<
  SAMPLE_COVERAGE          = 32928 /* GL_SAMPLE_COVERAGE          */, ///<
  SAMPLE_MASK              = 36433 /* GL_SAMPLE_MASK              */, ///<

#if !defined(USE_OPENGL_ES)
  POINT_SIZE        = 34370 /* GL_PROGRAM_POINT_SIZE        */, ///<
  CUBEMAP_SEAMLESS  = 34895 /* GL_TEXTURE_CUBE_MAP_SEAMLESS */, ///<
  FRAMEBUFFER_SRGB  = 36281 /* GL_FRAMEBUFFER_SRGB          */, ///<
  PRIMITIVE_RESTART = 36765 /* GL_PRIMITIVE_RESTART         */, ///<

  DEBUG_OUTPUT             = 37600 /* GL_DEBUG_OUTPUT             */, ///<
  DEBUG_OUTPUT_SYNCHRONOUS = 33346 /* GL_DEBUG_OUTPUT_SYNCHRONOUS */  ///<
#endif
};

enum class ContextInfo : unsigned int {
  VENDOR                   = 7936  /* GL_VENDOR                   */, ///< Graphics card vendor.
  RENDERER                 = 7937  /* GL_RENDERER                 */, ///< Graphics card model.
  VERSION                  = 7938  /* GL_VERSION                  */, ///< OpenGL version.
  SHADING_LANGUAGE_VERSION = 35724 /* GL_SHADING_LANGUAGE_VERSION */  ///< GLSL version.
};

enum class StateParameter : unsigned int {
  MAJOR_VERSION   = 33307 /* GL_MAJOR_VERSION  */, ///< OpenGL major version (in the form major.minor).
  MINOR_VERSION   = 33308 /* GL_MINOR_VERSION  */, ///< OpenGL minor version (in the form major.minor).
  EXTENSION_COUNT = 33309 /* GL_NUM_EXTENSIONS */, ///< Number of extensions supported for the current context.

  ACTIVE_TEXTURE  = 34016 /* GL_ACTIVE_TEXTURE  */, ///< Currently active texture.
  CURRENT_PROGRAM = 35725 /* GL_CURRENT_PROGRAM */, ///< Currently used program.

  DEPTH_CLEAR_VALUE = 2931                                              /* GL_DEPTH_CLEAR_VALUE */, ///< Depth clear value.
  DEPTH_FUNC        = 2932                                              /* GL_DEPTH_FUNC        */, ///< Depth function.
  DEPTH_RANGE       = 2928                                              /* GL_DEPTH_RANGE       */, ///< Depth range.
  DEPTH_TEST        = static_cast<unsigned int>(Capability::DEPTH_TEST) /* GL_DEPTH_TEST        */, ///< Depth testing.
  DEPTH_WRITEMASK   = 2930                                              /* GL_DEPTH_WRITEMASK   */, ///< Depth write mask.

  COLOR_CLEAR_VALUE = 3106                                                  /* GL_COLOR_CLEAR_VALUE */, ///< Clear color.
#if !defined(USE_OPENGL_ES)
  COLOR_LOGIC_OP    = static_cast<unsigned int>(Capability::COLOR_LOGIC_OP) /* GL_COLOR_LOGIC_OP    */, ///<
#endif
  COLOR_WRITEMASK   = 3107                                                  /* GL_COLOR_WRITEMASK   */, ///< Color write mask.

  BLEND                = static_cast<unsigned int>(Capability::BLEND) /* GL_BLEND                */, ///< Blending.
  BLEND_COLOR          = 32773                                        /* GL_BLEND_COLOR          */, ///<
  BLEND_DST_RGB        = 32968                                        /* GL_BLEND_DST_RGB        */, ///<
  BLEND_DST_ALPHA      = 32970                                        /* GL_BLEND_DST_ALPHA      */, ///<
  BLEND_SRC_RGB        = 32969                                        /* GL_BLEND_SRC_RGB        */, ///<
  BLEND_SRC_ALPHA      = 32971                                        /* GL_BLEND_SRC_ALPHA      */, ///<
  BLEND_EQUATION_RGB   = 32777                                        /* GL_BLEND_EQUATION_RGB   */, ///<
  BLEND_EQUATION_ALPHA = 34877                                        /* GL_BLEND_EQUATION_ALPHA */, ///<

  ALIASED_LINE_WIDTH_RANGE      = 33902 /* GL_ALIASED_LINE_WIDTH_RANGE      */, ///<
#if !defined(USE_OPENGL_ES)
  SMOOTH_LINE_WIDTH_RANGE       = 2850  /* GL_SMOOTH_LINE_WIDTH_RANGE       */, ///<
  SMOOTH_LINE_WIDTH_GRANULARITY = 2851  /* GL_SMOOTH_LINE_WIDTH_GRANULARITY */, ///<
#endif

  DITHER                     = static_cast<unsigned int>(Capability::DITHER)     /* GL_DITHER                     */, ///< Dithering.
  CULL_FACE                  = static_cast<unsigned int>(Capability::CULL)       /* GL_CULL_FACE                  */, ///< Polygon culling.
#if !defined(USE_OPENGL_ES)
  POINT_SIZE                 = static_cast<unsigned int>(Capability::POINT_SIZE) /* GL_POINT_SIZE                 */, ///< Point size.
#endif
  COMPRESSED_TEXTURE_FORMATS = 34467                                             /* GL_COMPRESSED_TEXTURE_FORMATS */, ///<
  ARRAY_BUFFER_BINDING       = 34964                                             /* GL_ARRAY_BUFFER_BINDING       */  ///<
};

enum class MaskType : unsigned int {
  COLOR   = 16384 /* GL_COLOR_BUFFER_BIT   */, ///<
  DEPTH   = 256   /* GL_DEPTH_BUFFER_BIT   */, ///<
  STENCIL = 1024  /* GL_STENCIL_BUFFER_BIT */  ///<
};
MAKE_ENUM_FLAG(MaskType)

enum class DepthStencilFunction : unsigned int {
  NEVER         = 512 /* GL_NEVER    */, ///<
  EQUAL         = 514 /* GL_EQUAL    */, ///<
  NOT_EQUAL     = 517 /* GL_NOTEQUAL */, ///<
  LESS          = 513 /* GL_LESS     */, ///<
  LESS_EQUAL    = 515 /* GL_LEQUAL   */, ///<
  GREATER       = 516 /* GL_GREATER  */, ///<
  GREATER_EQUAL = 518 /* GL_GEQUAL   */, ///<
  ALWAYS        = 519 /* GL_ALWAYS   */  ///<
};

enum class StencilOperation : unsigned int {
  ZERO           = 0     /* GL_ZERO      */, ///<
  KEEP           = 7680  /* GL_KEEP      */, ///<
  REPLACE        = 7681  /* GL_REPLACE   */, ///<
  INCREMENT      = 7682  /* GL_INCR      */, ///<
  INCREMENT_WRAP = 34055 /* GL_INCR_WRAP */, ///<
  DECREMENT      = 7683  /* GL_DECR      */, ///<
  DECREMENT_WRAP = 34056 /* GL_DECR_WRAP */, ///<
  INVERT         = 5386  /* GL_INVERT    */  ///<
};

enum class FaceOrientation : unsigned int {
  FRONT      = 1028 /* GL_FRONT          */, ///<
  BACK       = 1029 /* GL_BACK           */, ///<
  FRONT_BACK = 1032 /* GL_FRONT_AND_BACK */  ///<
};

enum class BlendFactor : unsigned int {
  ZERO                     = 0     /* GL_ZERO                     */, ///<
  ONE                      = 1     /* GL_ONE                      */, ///<
  SRC_COLOR                = 768   /* GL_SRC_COLOR                */, ///<
  ONE_MINUS_SRC_COLOR      = 769   /* GL_ONE_MINUS_SRC_COLOR      */, ///<
  SRC_ALPHA                = 770   /* GL_SRC_ALPHA                */, ///<
  ONE_MINUS_SRC_ALPHA      = 771   /* GL_ONE_MINUS_SRC_ALPHA      */, ///<
  DST_ALPHA                = 772   /* GL_DST_ALPHA                */, ///<
  ONE_MINUS_DST_ALPHA      = 773   /* GL_ONE_MINUS_DST_ALPHA      */, ///<
  DST_COLOR                = 774   /* GL_DST_COLOR                */, ///<
  ONE_MINUS_DST_COLOR      = 775   /* GL_ONE_MINUS_DST_COLOR      */, ///<
  SRC_ALPHA_SATURATE       = 776   /* GL_SRC_ALPHA_SATURATE       */, ///<
  CONSTANT_COLOR           = 32769 /* GL_CONSTANT_COLOR           */, ///<
  ONE_MINUS_CONSTANT_COLOR = 32770 /* GL_ONE_MINUS_CONSTANT_COLOR */, ///<
  CONSTANT_ALPHA           = 32771 /* GL_CONSTANT_ALPHA           */, ///<
  ONE_MINUS_CONSTANT_ALPHA = 32772 /* GL_ONE_MINUS_CONSTANT_ALPHA */, ///<
#if !defined(USE_OPENGL_ES)
  SRC1_COLOR               = 35065 /* GL_SRC1_COLOR               */, ///<
  ONE_MINUS_SRC1_COLOR     = 35066 /* GL_ONE_MINUS_SRC1_COLOR     */, ///<
  SRC1_ALPHA               = 34185 /* GL_SRC1_ALPHA               */, ///<
  ONE_MINUS_SRC1_ALPHA     = 35067 /* GL_ONE_MINUS_SRC1_ALPHA     */  ///<
#endif
};

enum class PolygonMode : unsigned int {
  POINT = 6912 /* GL_POINT */, ///<
  LINE  = 6913 /* GL_LINE  */, ///<
  FILL  = 6914 /* GL_FILL  */  ///<
};

enum class ClipOrigin : unsigned int {
  LOWER_LEFT = 36001 /* GL_LOWER_LEFT */, ///<
  UPPER_LEFT = 36002 /* GL_UPPER_LEFT */  ///<
};

enum class ClipDepth : unsigned int {
  NEG_ONE_TO_ONE = 37726 /* GL_NEGATIVE_ONE_TO_ONE */, ///<
  ZERO_TO_ONE    = 37727 /* GL_ZERO_TO_ONE         */  ///<
};

enum class PatchParameter : unsigned int {
  DEFAULT_OUTER_LEVEL = 36468 /* GL_PATCH_DEFAULT_OUTER_LEVEL */, ///< Default outer level used if no tessellation control shader exists.
  DEFAULT_INNER_LEVEL = 36467 /* GL_PATCH_DEFAULT_INNER_LEVEL */  ///< Default inner level used if no tessellation control shader exists.
};

enum class PixelStorage : unsigned int {
  UNPACK_SWAP_BYTES   = 3312  /* GL_UNPACK_SWAP_BYTES  */, ///<
  UNPACK_LSB_FIRST    = 3313  /* GL_UNPACK_LSB_FIRST   */, ///<
  UNPACK_ROW_LENGTH   = 3314  /* GL_UNPACK_ROW_LENGTH  */, ///<
  UNPACK_SKIP_ROWS    = 3315  /* GL_UNPACK_SKIP_ROWS   */, ///<
  UNPACK_SKIP_PIXELS  = 3316  /* GL_UNPACK_SKIP_PIXELS */, ///<
  UNPACK_SKIP_IMAGES  = 32877 /* GL_PACK_SKIP_IMAGES   */, ///<
  UNPACK_IMAGE_HEIGHT = 32878 /* GL_PACK_IMAGE_HEIGHT  */, ///<
  UNPACK_ALIGNMENT    = 3317  /* GL_UNPACK_ALIGNMENT   */, ///<

  PACK_SWAP_BYTES     = 3328  /* GL_PACK_SWAP_BYTES   */, ///<
  PACK_LSB_FIRST      = 3329  /* GL_PACK_LSB_FIRST    */, ///<
  PACK_ROW_LENGTH     = 3330  /* GL_PACK_ROW_LENGTH   */, ///<
  PACK_SKIP_ROWS      = 3331  /* GL_PACK_SKIP_ROWS    */, ///<
  PACK_SKIP_PIXELS    = 3332  /* GL_PACK_SKIP_PIXELS  */, ///<
  PACK_SKIP_IMAGES    = 32875 /* GL_PACK_SKIP_IMAGES  */, ///<
  PACK_IMAGE_HEIGHT   = 32876 /* GL_PACK_IMAGE_HEIGHT */, ///<
  PACK_ALIGNMENT      = 3333  /* GL_PACK_ALIGNMENT    */  ///<
};

enum class AttribDataType : unsigned int {
  BYTE             = 5120  /* GL_BYTE                         */, ///< Byte.
  UBYTE            = 5121  /* GL_UNSIGNED_BYTE                */, ///< Unsigned byte.
  SHORT            = 5122  /* GL_SHORT                        */, ///< Short.
  USHORT           = 5123  /* GL_UNSIGNED_SHORT               */, ///< Unsigned short.
  INT              = 5124  /* GL_INT                          */, ///< Integer.
  INT_2_10_10_10   = 36255 /* GL_INT_2_10_10_10_REV           */, ///< Packed integer stored in an ABGR format (2 alpha bits & 10 BGR bits each).
  UINT             = 5125  /* GL_UNSIGNED_INT                 */, ///< Unsigned integer.
  UINT_2_10_10_10  = 33640 /* GL_UNSIGNED_INT_2_10_10_10_REV  */, ///< Packed unsigned integer stored in an ABGR format (2 alpha bits & 10 BGR bits each).
  HALF_FLOAT       = 5131  /* GL_HALF_FLOAT                   */, ///< Half-precision floating-point.
  FLOAT            = 5126  /* GL_FLOAT                        */, ///< Simple-precision floating-point.
  FIXED            = 5132  /* GL_FIXED                        */, ///< Fixed-precision.
  UINT_10F_11F_11F = 35899 /* GL_UNSIGNED_INT_10F_11F_11F_REV */, ///< Packed floating-point stored in a BGR format (10 blue bits & 11 green & red bits each). Requires OpenGL 4.4+.
#if !defined(USE_OPENGL_ES)
  DOUBLE           = 5130  /* GL_DOUBLE                       */  ///< Double-precision floating-point.
#endif
};

enum class BufferType : unsigned int {
  ARRAY_BUFFER   = 34962 /* GL_ARRAY_BUFFER         */, ///<
  ELEMENT_BUFFER = 34963 /* GL_ELEMENT_ARRAY_BUFFER */, ///<
  UNIFORM_BUFFER = 35345 /* GL_UNIFORM_BUFFER       */  ///<
};

enum class BufferDataUsage : unsigned int {
  STREAM_DRAW  = 35040 /* GL_STREAM_DRAW */, ///<
  STREAM_READ  = 35041 /* GL_STREAM_READ */, ///<
  STREAM_COPY  = 35042 /* GL_STREAM_COPY */, ///<

  STATIC_DRAW  = 35044 /* GL_STATIC_DRAW */, ///<
  STATIC_READ  = 35045 /* GL_STATIC_READ */, ///<
  STATIC_COPY  = 35046 /* GL_STATIC_COPY */, ///<

  DYNAMIC_DRAW = 35048 /* GL_DYNAMIC_DRAW */, ///<
  DYNAMIC_READ = 35049 /* GL_DYNAMIC_READ */, ///<
  DYNAMIC_COPY = 35050 /* GL_DYNAMIC_COPY */  ///<
};

enum class TextureType : unsigned int {
#if !defined(USE_OPENGL_ES)
  TEXTURE_1D       = 3552  /* GL_TEXTURE_1D                  */, ///<
#endif
  TEXTURE_2D       = 3553  /* GL_TEXTURE_2D                  */, ///<
  TEXTURE_3D       = 32879 /* GL_TEXTURE_3D                  */, ///<
#if !defined(USE_OPENGL_ES)
  TEXTURE_1D_ARRAY = 35864 /* GL_TEXTURE_1D_ARRAY            */, ///<
#endif
  TEXTURE_2D_ARRAY = 35866 /* GL_TEXTURE_2D_ARRAY            */, ///<
  CUBEMAP          = 34067 /* GL_TEXTURE_CUBE_MAP            */, ///<
  CUBEMAP_POS_X    = 34069 /* GL_TEXTURE_CUBE_MAP_POSITIVE_X */, ///<
  CUBEMAP_NEG_X    = 34070 /* GL_TEXTURE_CUBE_MAP_NEGATIVE_X */, ///<
  CUBEMAP_POS_Y    = 34071 /* GL_TEXTURE_CUBE_MAP_POSITIVE_Y */, ///<
  CUBEMAP_NEG_Y    = 34072 /* GL_TEXTURE_CUBE_MAP_NEGATIVE_Y */, ///<
  CUBEMAP_POS_Z    = 34073 /* GL_TEXTURE_CUBE_MAP_POSITIVE_Z */, ///<
  CUBEMAP_NEG_Z    = 34074 /* GL_TEXTURE_CUBE_MAP_NEGATIVE_Z */, ///<
#if !defined(USE_OPENGL_ES)
  CUBEMAP_ARRAY    = 36873 /* GL_TEXTURE_CUBE_MAP_ARRAY      */  ///<
#endif
};

enum class TextureParam : unsigned int {
  MINIFY_FILTER  = 10241 /* GL_TEXTURE_MIN_FILTER   */, ///<
  MAGNIFY_FILTER = 10240 /* GL_TEXTURE_MAG_FILTER   */, ///<
  WRAP_S         = 10242 /* GL_TEXTURE_WRAP_S       */, ///<
  WRAP_T         = 10243 /* GL_TEXTURE_WRAP_T       */, ///<
  WRAP_R         = 32882 /* GL_TEXTURE_WRAP_R       */, ///<
  SWIZZLE_R      = 36418 /* GL_TEXTURE_SWIZZLE_R    */, ///<
  SWIZZLE_G      = 36419 /* GL_TEXTURE_SWIZZLE_G    */, ///<
  SWIZZLE_B      = 36420 /* GL_TEXTURE_SWIZZLE_B    */, ///<
  SWIZZLE_A      = 36421 /* GL_TEXTURE_SWIZZLE_A    */, ///<
#if !defined(USE_OPENGL_ES)
  SWIZZLE_RGBA   = 36422 /* GL_TEXTURE_SWIZZLE_RGBA */  ///<
#endif
};

enum class TextureParamValue : unsigned int {
  NEAREST                = 9728  /* GL_NEAREST                */, ///<
  LINEAR                 = 9729  /* GL_LINEAR                 */, ///<
  NEAREST_MIPMAP_NEAREST = 9984  /* GL_NEAREST_MIPMAP_NEAREST */, ///<
  LINEAR_MIPMAP_NEAREST  = 9985  /* GL_LINEAR_MIPMAP_NEAREST  */, ///<
  NEAREST_MIPMAP_LINEAR  = 9986  /* GL_NEAREST_MIPMAP_LINEAR  */, ///<
  LINEAR_MIPMAP_LINEAR   = 9987  /* GL_LINEAR_MIPMAP_LINEAR   */, ///<
  REPEAT                 = 10497 /* GL_REPEAT                 */, ///<
  CLAMP_TO_EDGE          = 33071 /* GL_CLAMP_TO_EDGE          */  ///<
};

enum class TextureAttribute : unsigned int {
  WIDTH                 = 4096  /* GL_TEXTURE_WIDTH                 */, ///<
  HEIGHT                = 4097  /* GL_TEXTURE_HEIGHT                */, ///<
  DEPTH                 = 32881 /* GL_TEXTURE_DEPTH                 */, ///<
  INTERNAL_FORMAT       = 4099  /* GL_TEXTURE_INTERNAL_FORMAT       */, ///<
  RED_SIZE              = 32860 /* GL_TEXTURE_RED_SIZE              */, ///<
  GREEN_SIZE            = 32861 /* GL_TEXTURE_GREEN_SIZE            */, ///<
  BLUE_SIZE             = 32862 /* GL_TEXTURE_BLUE_SIZE             */, ///<
  ALPHA_SIZE            = 32863 /* GL_TEXTURE_ALPHA_SIZE            */, ///<
  DEPTH_SIZE            = 34890 /* GL_TEXTURE_DEPTH_SIZE            */, ///<
  COMPRESSED            = 34465 /* GL_TEXTURE_COMPRESSED            */, ///<
  COMPRESSED_IMAGE_SIZE = 34464 /* GL_TEXTURE_COMPRESSED_IMAGE_SIZE */  ///<
};

enum class TextureFormat : unsigned int {
  RED           = 6403  /* GL_RED             */, ///<
  GREEN         = 6404  /* GL_GREEN           */, ///<
  BLUE          = 6405  /* GL_BLUE            */, ///<
  RG            = 33319 /* GL_RG              */, ///<
  RGB           = 6407  /* GL_RGB             */, ///<
  BGR           = 32992 /* GL_BGR             */, ///<
  RGBA          = 6408  /* GL_RGBA            */, ///<
  BGRA          = 32993 /* GL_BGRA            */, ///<
  SRGB          = 35904 /* GL_SRGB            */, ///<
  DEPTH         = 6402  /* GL_DEPTH_COMPONENT */, ///<
  STENCIL       = 6401  /* GL_STENCIL_INDEX   */, ///<
  DEPTH_STENCIL = 34041 /* GL_DEPTH_STENCIL   */  ///<
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
  R8          = 33321 /* GL_R8           */, ///<
  RG8         = 33323 /* GL_RG8          */, ///<
  RGB8        = 32849 /* GL_RGB8         */, ///<
  RGBA8       = 32856 /* GL_RGBA8        */, ///<
  SRGB8       = 35905 /* GL_SRGB8        */, ///<
  SRGBA8      = 35907 /* GL_SRGB8_ALPHA8 */, ///<
  R8I         = 33329 /* GL_R8I          */, ///<
  RG8I        = 33335 /* GL_RG8I         */, ///<
  RGB8I       = 36239 /* GL_RGB8I        */, ///<
  RGBA8I      = 36238 /* GL_RGBA8I       */, ///<
  R8UI        = 33330 /* GL_R8UI         */, ///<
  RG8UI       = 33336 /* GL_RG8UI        */, ///<
  RGB8UI      = 36221 /* GL_RGB8UI       */, ///<
  RGBA8UI     = 36220 /* GL_RGBA8UI      */, ///<
  R8_SNORM    = 36756 /* GL_R8_SNORM     */, ///<
  RG8_SNORM   = 36757 /* GL_RG8_SNORM    */, ///<
  RGB8_SNORM  = 36758 /* GL_RGB8_SNORM   */, ///<
  RGBA8_SNORM = 36759 /* GL_RGBA8_SNORM  */, ///<

  R16          = 33322 /* GL_R16          */, ///<
  RG16         = 33324 /* GL_RG16         */, ///<
  RGBA16       = 32859 /* GL_RGBA16       */, ///<
  R16I         = 33331 /* GL_R16I         */, ///<
  RG16I        = 33337 /* GL_RG16I        */, ///<
  RGB16I       = 36233 /* GL_RGB16I       */, ///<
  RGBA16I      = 36232 /* GL_RGBA16I      */, ///<
  R16UI        = 33332 /* GL_R16UI        */, ///<
  RG16UI       = 33338 /* GL_RG16UI       */, ///<
  RGB16UI      = 36215 /* GL_RGB16UI      */, ///<
  RGBA16UI     = 36214 /* GL_RGBA16UI     */, ///<
  R16F         = 33325 /* GL_R16F         */, ///<
  RG16F        = 33327 /* GL_RG16F        */, ///<
  RGB16F       = 34843 /* GL_RGB16F       */, ///<
  RGBA16F      = 34842 /* GL_RGBA16F      */, ///<
  R16_SNORM    = 36760 /* GL_R16_SNORM    */, ///<
  RG16_SNORM   = 36761 /* GL_RG16_SNORM   */, ///<
  RGB16_SNORM  = 36762 /* GL_RGB16_SNORM  */, ///<
  RGBA16_SNORM = 36763 /* GL_RGBA16_SNORM */, ///<

  R32I     = 33333 /* GL_R32I               */, ///<
  RG32I    = 33339 /* GL_RG32I              */, ///<
  RGB32I   = 36227 /* GL_RGB32I             */, ///<
  RGBA32I  = 36226 /* GL_RGBA32I            */, ///<
  R32UI    = 33334 /* GL_R32UI              */, ///<
  RG32UI   = 33340 /* GL_RG32UI             */, ///<
  RGB32UI  = 36209 /* GL_RGB32UI            */, ///<
  RGBA32UI = 36208 /* GL_RGBA32UI           */, ///<
  R32F     = 33326 /* GL_R32F               */, ///<
  RG32F    = 33328 /* GL_RG32F              */, ///<
  RGB32F   = 34837 /* GL_RGB32F             */, ///<
  RGBA32F  = 34836 /* GL_RGBA32F            */, ///<

  DEPTH16           = 33189 /* GL_DEPTH_COMPONENT16  */, ///<
  DEPTH24           = 33190 /* GL_DEPTH_COMPONENT24  */, ///<
  DEPTH24_STENCIL8  = 35056 /* GL_DEPTH24_STENCIL8   */, ///<
  DEPTH32F          = 36012 /* GL_DEPTH_COMPONENT32F */, ///<
  DEPTH32F_STENCIL8 = 36013 /* GL_DEPTH32F_STENCIL8  */, ///<

  RGB10_A2       = 32857 /* GL_RGB10_A2       */, ///<
  RGB10_A2UI     = 36975 /* GL_RGB10_A2UI     */, ///<
  R11F_G11F_B10F = 35898 /* GL_R11F_G11F_B10F */  ///<
};

enum class PixelDataType : unsigned int {
  UBYTE = 5121 /* GL_UNSIGNED_BYTE */, ///< Unsigned byte data type.
  FLOAT = 5126 /* GL_FLOAT         */  ///< Single precision floating-point data type.
};

enum class ImageAccess : unsigned int {
  READ       = 35000 /* GL_READ_ONLY  */, ///< Read-only image access.
  WRITE      = 35001 /* GL_WRITE_ONLY */, ///< Write-only image access.
  READ_WRITE = 35002 /* GL_READ_WRITE */  ///< Both read & write image access.
};

enum class ImageInternalFormat : unsigned int {
#if !defined(USE_OPENGL_ES)
  R8          = static_cast<unsigned int>(TextureInternalFormat::R8),          ///<
  RG8         = static_cast<unsigned int>(TextureInternalFormat::RG8),         ///<
#endif
  RGBA8       = static_cast<unsigned int>(TextureInternalFormat::RGBA8),       ///<
#if !defined(USE_OPENGL_ES)
  R8I         = static_cast<unsigned int>(TextureInternalFormat::R8I),         ///<
  RG8I        = static_cast<unsigned int>(TextureInternalFormat::RG8I),        ///<
#endif
  RGBA8I      = static_cast<unsigned int>(TextureInternalFormat::RGBA8I),      ///<
#if !defined(USE_OPENGL_ES)
  R8UI        = static_cast<unsigned int>(TextureInternalFormat::R8UI),        ///<
  RG8UI       = static_cast<unsigned int>(TextureInternalFormat::RG8UI),       ///<
#endif
  RGBA8UI     = static_cast<unsigned int>(TextureInternalFormat::RGBA8UI),     ///<
#if !defined(USE_OPENGL_ES)
  R8_SNORM    = static_cast<unsigned int>(TextureInternalFormat::R8_SNORM),    ///<
  RG8_SNORM   = static_cast<unsigned int>(TextureInternalFormat::RG8_SNORM),   ///<
#endif
  RGBA8_SNORM = static_cast<unsigned int>(TextureInternalFormat::RGBA8_SNORM), ///<

#if !defined(USE_OPENGL_ES)
  R16          = static_cast<unsigned int>(TextureInternalFormat::R16),          ///<
  RG16         = static_cast<unsigned int>(TextureInternalFormat::RG16),         ///<
  RGBA16       = static_cast<unsigned int>(TextureInternalFormat::RGBA16),       ///<
  R16I         = static_cast<unsigned int>(TextureInternalFormat::R16I),         ///<
  RG16I        = static_cast<unsigned int>(TextureInternalFormat::RG16I),        ///<
#endif
  RGBA16I      = static_cast<unsigned int>(TextureInternalFormat::RGBA16I),      ///<
#if !defined(USE_OPENGL_ES)
  R16UI        = static_cast<unsigned int>(TextureInternalFormat::R16UI),        ///<
  RG16UI       = static_cast<unsigned int>(TextureInternalFormat::RG16UI),       ///<
#endif
  RGBA16UI     = static_cast<unsigned int>(TextureInternalFormat::RGBA16UI),     ///<
#if !defined(USE_OPENGL_ES)
  R16F         = static_cast<unsigned int>(TextureInternalFormat::R16F),         ///<
  RG16F        = static_cast<unsigned int>(TextureInternalFormat::RG16F),        ///<
#endif
  RGBA16F      = static_cast<unsigned int>(TextureInternalFormat::RGBA16F),      ///<
#if !defined(USE_OPENGL_ES)
  R16_SNORM    = static_cast<unsigned int>(TextureInternalFormat::R16_SNORM),    ///<
  RG16_SNORM   = static_cast<unsigned int>(TextureInternalFormat::RG16_SNORM),   ///<
  RGBA16_SNORM = static_cast<unsigned int>(TextureInternalFormat::RGBA16_SNORM), ///<
#endif

  R32I     = static_cast<unsigned int>(TextureInternalFormat::R32I),     ///<
#if !defined(USE_OPENGL_ES)
  RG32I    = static_cast<unsigned int>(TextureInternalFormat::RG32I),    ///<
#endif
  RGBA32I  = static_cast<unsigned int>(TextureInternalFormat::RGBA32I),  ///<
  R32UI    = static_cast<unsigned int>(TextureInternalFormat::R32UI),    ///<
#if !defined(USE_OPENGL_ES)
  RG32UI   = static_cast<unsigned int>(TextureInternalFormat::RG32UI),   ///<
#endif
  RGBA32UI = static_cast<unsigned int>(TextureInternalFormat::RGBA32UI), ///<
  R32F     = static_cast<unsigned int>(TextureInternalFormat::R32F),     ///<
#if !defined(USE_OPENGL_ES)
  RG32F    = static_cast<unsigned int>(TextureInternalFormat::RG32F),    ///<
#endif
  RGBA32F  = static_cast<unsigned int>(TextureInternalFormat::RGBA32F),  ///<

#if !defined(USE_OPENGL_ES)
  RGB10_A2       = static_cast<unsigned int>(TextureInternalFormat::RGB10_A2),      ///<
  RGB10_A2UI     = static_cast<unsigned int>(TextureInternalFormat::RGB10_A2UI),    ///<
  R11F_G11F_B10F = static_cast<unsigned int>(TextureInternalFormat::R11F_G11F_B10F) ///<
#endif
};

enum class ProgramParameter : unsigned int {
  DELETE_STATUS                         = 35712 /* GL_DELETE_STATUS                         */, ///<
  LINK_STATUS                           = 35714 /* GL_LINK_STATUS                           */, ///<
  VALIDATE_STATUS                       = 35715 /* GL_VALIDATE_STATUS                       */, ///<
  INFO_LOG_LENGTH                       = 35716 /* GL_INFO_LOG_LENGTH                       */, ///<
  ATTACHED_SHADERS                      = 35717 /* GL_ATTACHED_SHADERS                      */, ///<
  ACTIVE_ATTRIBUTES                     = 35721 /* GL_ACTIVE_ATTRIBUTES                     */, ///<
  ACTIVE_ATTRIBUTE_MAX_LENGTH           = 35722 /* GL_ACTIVE_ATTRIBUTE_MAX_LENGTH           */, ///<
  ACTIVE_UNIFORMS                       = 35718 /* GL_ACTIVE_UNIFORMS                       */, ///<
  ACTIVE_UNIFORM_MAX_LENGTH             = 35719 /* GL_ACTIVE_UNIFORM_MAX_LENGTH             */, ///<
  ACTIVE_UNIFORM_BLOCKS                 = 35382 /* GL_ACTIVE_UNIFORM_BLOCKS                 */, ///<
  ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH  = 35381 /* GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH  */, ///<
  TRANSFORM_FEEDBACK_BUFFER_MODE        = 35967 /* GL_TRANSFORM_FEEDBACK_BUFFER_MODE        */, ///<
  TRANSFORM_FEEDBACK_VARYINGS           = 35971 /* GL_TRANSFORM_FEEDBACK_VARYINGS           */, ///<
  TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH = 35958 /* GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH */, ///<
  GEOMETRY_VERTICES_OUT                 = 35094 /* GL_GEOMETRY_VERTICES_OUT                 */, ///<
  GEOMETRY_INPUT_TYPE                   = 35095 /* GL_GEOMETRY_INPUT_TYPE                   */, ///<
  GEOMETRY_OUTPUT_TYPE                  = 35096 /* GL_GEOMETRY_OUTPUT_TYPE                  */  ///<
};

enum class ShaderType : unsigned int {
  VERTEX                  = 35633 /* GL_VERTEX_SHADER          */, ///< Vertex shader.
  TESSELLATION_CONTROL    = 36488 /* GL_TESS_CONTROL_SHADER    */, ///< Tessellation control shader. Requires OpenGL 4.0+.
  TESSELLATION_EVALUATION = 36487 /* GL_TESS_EVALUATION_SHADER */, ///< Tessellation evaluation shader. Requires OpenGL 4.0+.
  FRAGMENT                = 35632 /* GL_FRAGMENT_SHADER        */, ///< Fragment shader.
  GEOMETRY                = 36313 /* GL_GEOMETRY_SHADER        */, ///< Geometry shader.
  COMPUTE                 = 37305 /* GL_COMPUTE_SHADER         */  ///< Compute shader. Requires OpenGL 4.3+.
};

enum class ShaderInfo : unsigned int {
  TYPE            = 35663 /* GL_SHADER_TYPE          */, ///<
  DELETE_STATUS   = 35712 /* GL_DELETE_STATUS        */, ///<
  COMPILE_STATUS  = 35713 /* GL_COMPILE_STATUS       */, ///<
  INFO_LOG_LENGTH = 35716 /* GL_INFO_LOG_LENGTH      */, ///<
  SOURCE_LENGTH   = 35720 /* GL_SHADER_SOURCE_LENGTH */  ///<
};

enum class UniformType : unsigned int {
  // Primitive types
  FLOAT  = 5126  /* GL_FLOAT        */, ///< Single precision floating-point value.
#if !defined(USE_OPENGL_ES)
  DOUBLE = 5130  /* GL_DOUBLE       */, ///< Double precision floating-point value. Requires OpenGL 4.1+.
#endif
  INT    = 5124  /* GL_INT          */, ///< Integer value.
  UINT   = 5125  /* GL_UNSIGNED_INT */, ///< Unsigned integer value.
  BOOL   = 35670 /* GL_BOOL         */, ///< Boolean value.

  // Vectors
  VEC2  = 35664 /* GL_FLOAT_VEC2        */, ///< Single precision floating-point 2D vector.
  VEC3  = 35665 /* GL_FLOAT_VEC3        */, ///< Single precision floating-point 3D vector.
  VEC4  = 35666 /* GL_FLOAT_VEC4        */, ///< Single precision floating-point 4D vector.
#if !defined(USE_OPENGL_ES)
  DVEC2 = 36860 /* GL_DOUBLE_VEC2       */, ///< Double precision floating-point 2D vector. Requires OpenGL 4.1+.
  DVEC3 = 36861 /* GL_DOUBLE_VEC3       */, ///< Double precision floating-point 3D vector. Requires OpenGL 4.1+.
  DVEC4 = 36862 /* GL_DOUBLE_VEC4       */, ///< Double precision floating-point 4D vector. Requires OpenGL 4.1+.
#endif
  IVEC2 = 35667 /* GL_INT_VEC2          */, ///<
  IVEC3 = 35668 /* GL_INT_VEC3          */, ///<
  IVEC4 = 35669 /* GL_INT_VEC4          */, ///<
  UVEC2 = 36294 /* GL_UNSIGNED_INT_VEC2 */, ///<
  UVEC3 = 36295 /* GL_UNSIGNED_INT_VEC3 */, ///<
  UVEC4 = 36296 /* GL_UNSIGNED_INT_VEC4 */, ///<
  BVEC2 = 35671 /* GL_BOOL_VEC2         */, ///<
  BVEC3 = 35672 /* GL_BOOL_VEC3         */, ///<
  BVEC4 = 35673 /* GL_BOOL_VEC4         */, ///<

  // Matrices
  MAT2    = 35674 /* GL_FLOAT_MAT2    */, ///< Single precision floating-point 2x2 matrix.
  MAT3    = 35675 /* GL_FLOAT_MAT3    */, ///< Single precision floating-point 3x3 matrix.
  MAT4    = 35676 /* GL_FLOAT_MAT4    */, ///< Single precision floating-point 4x4 matrix.
  MAT2x3  = 35685 /* GL_FLOAT_MAT2x3  */, ///< Single precision floating-point 2x3 matrix.
  MAT2x4  = 35686 /* GL_FLOAT_MAT2x4  */, ///< Single precision floating-point 2x4 matrix.
  MAT3x2  = 35687 /* GL_FLOAT_MAT3x2  */, ///< Single precision floating-point 3x2 matrix.
  MAT3x4  = 35688 /* GL_FLOAT_MAT3x4  */, ///< Single precision floating-point 3x4 matrix.
  MAT4x2  = 35689 /* GL_FLOAT_MAT4x2  */, ///< Single precision floating-point 4x2 matrix.
  MAT4x3  = 35690 /* GL_FLOAT_MAT4x3  */, ///< Single precision floating-point 4x3 matrix.
#if !defined(USE_OPENGL_ES)
  DMAT2   = 36678 /* GL_DOUBLE_MAT2   */, ///< Double precision floating-point 2x2 matrix. Requires OpenGL 4.1+.
  DMAT3   = 36679 /* GL_DOUBLE_MAT3   */, ///< Double precision floating-point 3x3 matrix. Requires OpenGL 4.1+.
  DMAT4   = 36680 /* GL_DOUBLE_MAT4   */, ///< Double precision floating-point 4x4 matrix. Requires OpenGL 4.1+.
  DMAT2x3 = 36681 /* GL_DOUBLE_MAT2x3 */, ///< Double precision floating-point 2x3 matrix. Requires OpenGL 4.1+.
  DMAT2x4 = 36682 /* GL_DOUBLE_MAT2x4 */, ///< Double precision floating-point 2x4 matrix. Requires OpenGL 4.1+.
  DMAT3x2 = 36683 /* GL_DOUBLE_MAT3x2 */, ///< Double precision floating-point 3x2 matrix. Requires OpenGL 4.1+.
  DMAT3x4 = 36684 /* GL_DOUBLE_MAT3x4 */, ///< Double precision floating-point 3x4 matrix. Requires OpenGL 4.1+.
  DMAT4x2 = 36685 /* GL_DOUBLE_MAT4x2 */, ///< Double precision floating-point 4x2 matrix. Requires OpenGL 4.1+.
  DMAT4x3 = 36686 /* GL_DOUBLE_MAT4x3 */, ///< Double precision floating-point 4x3 matrix. Requires OpenGL 4.1+.
#endif

  // Samplers
#if !defined(USE_OPENGL_ES)
  SAMPLER_1D                        = 35677 /* GL_SAMPLER_1D                                */, ///<
#endif
  SAMPLER_2D                        = 35678 /* GL_SAMPLER_2D                                */, ///<
  SAMPLER_3D                        = 35679 /* GL_SAMPLER_3D                                */, ///<
  SAMPLER_CUBE                      = 35680 /* GL_SAMPLER_CUBE                              */, ///<
#if !defined(USE_OPENGL_ES)
  SAMPLER_1D_SHADOW                 = 35681 /* GL_SAMPLER_1D_SHADOW                         */, ///<
#endif
  SAMPLER_2D_SHADOW                 = 35682 /* GL_SAMPLER_2D_SHADOW                         */, ///<
#if !defined(USE_OPENGL_ES)
  SAMPLER_1D_ARRAY                  = 36288 /* GL_SAMPLER_1D_ARRAY                          */, ///<
#endif
  SAMPLER_2D_ARRAY                  = 36289 /* GL_SAMPLER_2D_ARRAY                          */, ///<
#if !defined(USE_OPENGL_ES)
  SAMPLER_1D_ARRAY_SHADOW           = 36291 /* GL_SAMPLER_1D_ARRAY_SHADOW                   */, ///<
#endif
  SAMPLER_2D_ARRAY_SHADOW           = 36292 /* GL_SAMPLER_2D_ARRAY_SHADOW                   */, ///<
#if !defined(USE_OPENGL_ES)
  SAMPLER_2D_MULTISAMPLE            = 37128 /* GL_SAMPLER_2D_MULTISAMPLE                    */, ///<
  SAMPLER_2D_MULTISAMPLE_ARRAY      = 37131 /* GL_SAMPLER_2D_MULTISAMPLE_ARRAY              */, ///<
#endif
  SAMPLER_CUBE_SHADOW               = 36293 /* GL_SAMPLER_CUBE_SHADOW                       */, ///<
#if !defined(USE_OPENGL_ES)
  SAMPLER_BUFFER                    = 36290 /* GL_SAMPLER_BUFFER                            */, ///<
  SAMPLER_2D_RECT                   = 35683 /* GL_SAMPLER_2D_RECT                           */, ///<
  SAMPLER_2D_RECT_SHADOW            = 35684 /* GL_SAMPLER_2D_RECT_SHADOW                    */, ///<
  INT_SAMPLER_1D                    = 36297 /* GL_INT_SAMPLER_1D                            */, ///<
#endif
  INT_SAMPLER_2D                    = 36298 /* GL_INT_SAMPLER_2D                            */, ///<
  INT_SAMPLER_3D                    = 36299 /* GL_INT_SAMPLER_3D                            */, ///<
  INT_SAMPLER_CUBE                  = 36300 /* GL_INT_SAMPLER_CUBE                          */, ///<
#if !defined(USE_OPENGL_ES)
  INT_SAMPLER_1D_ARRAY              = 36302 /* GL_INT_SAMPLER_1D_ARRAY                      */, ///<
#endif
  INT_SAMPLER_2D_ARRAY              = 36303 /* GL_INT_SAMPLER_2D_ARRAY                      */, ///<
#if !defined(USE_OPENGL_ES)
  INT_SAMPLER_2D_MULTISAMPLE        = 37129 /* GL_INT_SAMPLER_2D_MULTISAMPLE                */, ///<
  INT_SAMPLER_2D_MULTISAMPLE_ARRAY  = 37132 /* GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY          */, ///<
  INT_SAMPLER_BUFFER                = 36304 /* GL_INT_SAMPLER_BUFFER                        */, ///<
  INT_SAMPLER_2D_RECT               = 36301 /* GL_INT_SAMPLER_2D_RECT                       */, ///<
  UINT_SAMPLER_1D                   = 36305 /* GL_UNSIGNED_INT_SAMPLER_1D                   */, ///<
#endif
  UINT_SAMPLER_2D                   = 36306 /* GL_UNSIGNED_INT_SAMPLER_2D                   */, ///<
  UINT_SAMPLER_3D                   = 36307 /* GL_UNSIGNED_INT_SAMPLER_3D                   */, ///<
  UINT_SAMPLER_CUBE                 = 36308 /* GL_UNSIGNED_INT_SAMPLER_CUBE                 */, ///<
#if !defined(USE_OPENGL_ES)
  UINT_SAMPLER_1D_ARRAY             = 36310 /* GL_UNSIGNED_INT_SAMPLER_1D_ARRAY             */, ///<
#endif
  UINT_SAMPLER_2D_ARRAY             = 36311 /* GL_UNSIGNED_INT_SAMPLER_2D_ARRAY             */, ///<
#if !defined(USE_OPENGL_ES)
  UINT_SAMPLER_2D_MULTISAMPLE       = 37130 /* GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE       */, ///<
  UINT_SAMPLER_2D_MULTISAMPLE_ARRAY = 37133 /* GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY */, ///<
  UINT_SAMPLER_BUFFER               = 36312 /* GL_UNSIGNED_INT_SAMPLER_BUFFER               */, ///<
  UINT_SAMPLER_2D_RECT              = 36309 /* GL_UNSIGNED_INT_SAMPLER_2D_RECT              */, ///<
#endif

  // Images
#if !defined(USE_OPENGL_ES)
  IMAGE_1D                        = 36940 /* GL_IMAGE_1D                                */, ///< 1D image. Requires OpenGL 4.2+.
  IMAGE_2D                        = 36941 /* GL_IMAGE_2D                                */, ///< 2D image. Requires OpenGL 4.2+.
  IMAGE_3D                        = 36942 /* GL_IMAGE_3D                                */, ///< 3D image. Requires OpenGL 4.2+.
  IMAGE_2D_RECT                   = 36943 /* GL_IMAGE_2D_RECT                           */, ///< 2D rectangle image. Requires OpenGL 4.2+.
  IMAGE_CUBE                      = 36944 /* GL_IMAGE_CUBE                              */, ///< Cube image. Requires OpenGL 4.2+.
  IMAGE_BUFFER                    = 36945 /* GL_IMAGE_BUFFER                            */, ///< Buffer image. Requires OpenGL 4.2+.
  IMAGE_1D_ARRAY                  = 36946 /* GL_IMAGE_1D_ARRAY                          */, ///< 1D array image. Requires OpenGL 4.2+.
  IMAGE_2D_ARRAY                  = 36947 /* GL_IMAGE_2D_ARRAY                          */, ///< 2D array image. Requires OpenGL 4.2+.
  IMAGE_2D_MULTISAMPLE            = 36949 /* GL_IMAGE_2D_MULTISAMPLE                    */, ///< 2D multisample image. Requires OpenGL 4.2+.
  IMAGE_2D_MULTISAMPLE_ARRAY      = 36950 /* GL_IMAGE_2D_MULTISAMPLE_ARRAY              */, ///< 2D array multisample image. Requires OpenGL 4.2+.
  INT_IMAGE_1D                    = 36951 /* GL_INT_IMAGE_1D                            */, ///< 1D integer image. Requires OpenGL 4.2+.
  INT_IMAGE_2D                    = 36952 /* GL_INT_IMAGE_2D                            */, ///< 2D integer image. Requires OpenGL 4.2+.
  INT_IMAGE_3D                    = 36953 /* GL_INT_IMAGE_3D                            */, ///< 3D integer image. Requires OpenGL 4.2+.
  INT_IMAGE_2D_RECT               = 36954 /* GL_INT_IMAGE_2D_RECT                       */, ///< 2D rectangle integer image. Requires OpenGL 4.2+.
  INT_IMAGE_CUBE                  = 36955 /* GL_INT_IMAGE_CUBE                          */, ///< Cube integer image. Requires OpenGL 4.2+.
  INT_IMAGE_BUFFER                = 36956 /* GL_INT_IMAGE_BUFFER                        */, ///< Integer buffer image. Requires OpenGL 4.2+.
  INT_IMAGE_1D_ARRAY              = 36957 /* GL_INT_IMAGE_1D_ARRAY                      */, ///< 1D array integer image. Requires OpenGL 4.2+.
  INT_IMAGE_2D_ARRAY              = 36958 /* GL_INT_IMAGE_2D_ARRAY                      */, ///< 2D array integer image. Requires OpenGL 4.2+.
  INT_IMAGE_2D_MULTISAMPLE        = 36960 /* GL_INT_IMAGE_2D_MULTISAMPLE                */, ///< 2D multisample integer image. Requires OpenGL 4.2+.
  INT_IMAGE_2D_MULTISAMPLE_ARRAY  = 36961 /* GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY          */, ///< 2D array multisample integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_1D                   = 36962 /* GL_UNSIGNED_INT_IMAGE_1D                   */, ///< 1D unsigned integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_2D                   = 36963 /* GL_UNSIGNED_INT_IMAGE_2D                   */, ///< 2D unsigned integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_3D                   = 36964 /* GL_UNSIGNED_INT_IMAGE_3D                   */, ///< 3D unsigned integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_2D_RECT              = 36965 /* GL_UNSIGNED_INT_IMAGE_2D_RECT              */, ///< 2D rectangle unsigned int image. Requires OpenGL 4.2+.
  UINT_IMAGE_CUBE                 = 36966 /* GL_UNSIGNED_INT_IMAGE_CUBE                 */, ///< Cube unsigned integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_BUFFER               = 36967 /* GL_UNSIGNED_INT_IMAGE_BUFFER               */, ///< Unsigned integer buffer image. Requires OpenGL 4.2+.
  UINT_IMAGE_1D_ARRAY             = 36968 /* GL_UNSIGNED_INT_IMAGE_1D_ARRAY             */, ///< 1D array unsigned integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_2D_ARRAY             = 36969 /* GL_UNSIGNED_INT_IMAGE_2D_ARRAY             */, ///< 2D array unsigned integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_2D_MULTISAMPLE       = 36971 /* GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE       */, ///< 2D multisample unsigned integer image. Requires OpenGL 4.2+.
  UINT_IMAGE_2D_MULTISAMPLE_ARRAY = 36972 /* GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY */, ///< 2D array multisample unsigned integer image. Requires OpenGL 4.2+.
#endif

  // Misc
#if !defined(USE_OPENGL_ES)
  UINT_ATOMIC_COUNTER = 37595 /* GL_UNSIGNED_INT_ATOMIC_COUNTER */ ///< . Requires OpenGL 4.2+.
#endif
};

enum class FramebufferType : unsigned int {
  READ_FRAMEBUFFER = 36008 /* GL_READ_FRAMEBUFFER */, ///<
  DRAW_FRAMEBUFFER = 36009 /* GL_DRAW_FRAMEBUFFER */, ///<
  FRAMEBUFFER      = 36160 /* GL_FRAMEBUFFER      */  ///<
};

enum class FramebufferStatus : unsigned int {
  COMPLETE                      = 36053 /* GL_FRAMEBUFFER_COMPLETE                      */, ///<
  UNDEFINED                     = 33305 /* GL_FRAMEBUFFER_UNDEFINED                     */, ///<
  INCOMPLETE_ATTACHMENT         = 36054 /* GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT         */, ///<
  INCOMPLETE_MISSING_ATTACHMENT = 36055 /* GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT */, ///<
  INCOMPLETE_DRAW_BUFFER        = 36059 /* GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER        */, ///<
  INCOMPLETE_READ_BUFFER        = 36060 /* GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER        */, ///<
  UNSUPPORTED                   = 36061 /* GL_FRAMEBUFFER_UNSUPPORTED                   */, ///<
  INCOMPLETE_MULTISAMPLE        = 36182 /* GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE        */, ///<
  INCOMPLETE_LAYER_TARGETS      = 36264 /* GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS      */  ///<
};

enum class FramebufferAttachment : unsigned int {
  DEPTH         = 36096 /* GL_DEPTH_ATTACHMENT         */, ///< Depth attachment.
  STENCIL       = 36128 /* GL_STENCIL_ATTACHMENT       */, ///< Stencil attachment.
  DEPTH_STENCIL = 33306 /* GL_DEPTH_STENCIL_ATTACHMENT */, ///< Depth & stencil attachment.

  COLOR0 = 36064 /* GL_COLOR_ATTACHMENT0 */, ///< Color attachment 0.
  COLOR1 = 36065 /* GL_COLOR_ATTACHMENT1 */, ///< Color attachment 1.
  COLOR2 = 36066 /* GL_COLOR_ATTACHMENT2 */, ///< Color attachment 2.
  COLOR3 = 36067 /* GL_COLOR_ATTACHMENT3 */, ///< Color attachment 3.
  COLOR4 = 36068 /* GL_COLOR_ATTACHMENT4 */, ///< Color attachment 4.
  COLOR5 = 36069 /* GL_COLOR_ATTACHMENT5 */, ///< Color attachment 5.
  COLOR6 = 36070 /* GL_COLOR_ATTACHMENT6 */, ///< Color attachment 6.
  COLOR7 = 36071 /* GL_COLOR_ATTACHMENT7 */  ///< Color attachment 7.
};

enum class ReadBuffer : unsigned int {
  NONE = 0    /* GL_NONE */, ///<
  BACK = 1029 /* GL_BACK */, ///<

#if !defined(USE_OPENGL_ES)
  FRONT       = 1028 /* GL_FRONT       */, ///<
  LEFT        = 1030 /* GL_LEFT        */, ///<
  RIGHT       = 1031 /* GL_RIGHT       */, ///<
  FRONT_LEFT  = 1024 /* GL_FRONT_LEFT  */, ///<
  FRONT_RIGHT = 1025 /* GL_FRONT_RIGHT */, ///<
  BACK_LEFT   = 1026 /* GL_BACK_LEFT   */, ///<
  BACK_RIGHT  = 1027 /* GL_BACK_RIGHT  */, ///<
#endif

  COLOR_ATTACHMENT0 = static_cast<unsigned int>(FramebufferAttachment::COLOR0), ///< Color attachment 0.
  COLOR_ATTACHMENT1 = static_cast<unsigned int>(FramebufferAttachment::COLOR1), ///< Color attachment 1.
  COLOR_ATTACHMENT2 = static_cast<unsigned int>(FramebufferAttachment::COLOR2), ///< Color attachment 2.
  COLOR_ATTACHMENT3 = static_cast<unsigned int>(FramebufferAttachment::COLOR3), ///< Color attachment 3.
  COLOR_ATTACHMENT4 = static_cast<unsigned int>(FramebufferAttachment::COLOR4), ///< Color attachment 4.
  COLOR_ATTACHMENT5 = static_cast<unsigned int>(FramebufferAttachment::COLOR5), ///< Color attachment 5.
  COLOR_ATTACHMENT6 = static_cast<unsigned int>(FramebufferAttachment::COLOR6), ///< Color attachment 6.
  COLOR_ATTACHMENT7 = static_cast<unsigned int>(FramebufferAttachment::COLOR7)  ///< Color attachment 7.
};

enum class DrawBuffer : unsigned int {
  NONE = static_cast<unsigned int>(ReadBuffer::NONE), ///<

#if !defined(USE_OPENGL_ES)
  FRONT_LEFT  = static_cast<unsigned int>(ReadBuffer::FRONT_LEFT),  ///<
  FRONT_RIGHT = static_cast<unsigned int>(ReadBuffer::FRONT_RIGHT), ///<
  BACK_LEFT   = static_cast<unsigned int>(ReadBuffer::BACK_LEFT),   ///<
  BACK_RIGHT  = static_cast<unsigned int>(ReadBuffer::BACK_RIGHT),  ///<
#else
  BACK = static_cast<unsigned int>(ReadBuffer::BACK), ///<
#endif

  COLOR_ATTACHMENT0 = static_cast<unsigned int>(FramebufferAttachment::COLOR0), ///< Color attachment 0.
  COLOR_ATTACHMENT1 = static_cast<unsigned int>(FramebufferAttachment::COLOR1), ///< Color attachment 1.
  COLOR_ATTACHMENT2 = static_cast<unsigned int>(FramebufferAttachment::COLOR2), ///< Color attachment 2.
  COLOR_ATTACHMENT3 = static_cast<unsigned int>(FramebufferAttachment::COLOR3), ///< Color attachment 3.
  COLOR_ATTACHMENT4 = static_cast<unsigned int>(FramebufferAttachment::COLOR4), ///< Color attachment 4.
  COLOR_ATTACHMENT5 = static_cast<unsigned int>(FramebufferAttachment::COLOR5), ///< Color attachment 5.
  COLOR_ATTACHMENT6 = static_cast<unsigned int>(FramebufferAttachment::COLOR6), ///< Color attachment 6.
  COLOR_ATTACHMENT7 = static_cast<unsigned int>(FramebufferAttachment::COLOR7)  ///< Color attachment 7.
};

enum class BlitFilter : unsigned int {
  NEAREST = static_cast<unsigned int>(TextureParamValue::NEAREST), ///<
  LINEAR  = static_cast<unsigned int>(TextureParamValue::LINEAR)   ///<
};

enum class PrimitiveType : unsigned int {
  POINTS                   = 0  /* GL_POINTS                   */, ///<
  LINES                    = 1  /* GL_LINES                    */, ///<
  LINE_LOOP                = 2  /* GL_LINE_LOOP                */, ///<
  LINE_STRIP               = 3  /* GL_LINE_STRIP               */, ///<
#if !defined(USE_OPENGL_ES)
  LINES_ADJACENCY          = 10 /* GL_LINES_ADJACENCY          */, ///<
  LINE_STRIP_ADJACENCY     = 11 /* GL_LINE_STRIP_ADJACENCY     */, ///<
#endif
  TRIANGLES                = 4  /* GL_TRIANGLES                */, ///<
  TRIANGLE_STRIP           = 5  /* GL_TRIANGLE_STRIP           */, ///<
  TRIANGLE_FAN             = 6  /* GL_TRIANGLE_FAN             */, ///<
#if !defined(USE_OPENGL_ES)
  TRIANGLES_ADJACENCY      = 12 /* GL_TRIANGLES_ADJACENCY      */, ///<
  TRIANGLE_STRIP_ADJACENCY = 13 /* GL_TRIANGLE_STRIP_ADJACENCY */, ///<
  PATCHES                  = 14 /* GL_PATCHES                  */  ///<
#endif
};

enum class ElementDataType : unsigned int {
  UBYTE  = 5121 /* GL_UNSIGNED_BYTE  */, ///<
  USHORT = 5123 /* GL_UNSIGNED_SHORT */, ///<
  UINT   = 5125 /* GL_UNSIGNED_INT   */  ///<
};

enum class BarrierType : unsigned int {
  VERTEX_ATTRIB_ARRAY = 1          /* GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT */, ///<
  ELEMENT_ARRAY       = 2          /* GL_ELEMENT_ARRAY_BARRIER_BIT       */, ///<
  UNIFORM             = 4          /* GL_UNIFORM_BARRIER_BIT             */, ///<
  TEXTURE_FETCH       = 8          /* GL_TEXTURE_FETCH_BARRIER_BIT       */, ///<
  SHADER_IMAGE_ACCESS = 32         /* GL_SHADER_IMAGE_ACCESS_BARRIER_BIT */, ///<
  COMMAND             = 64         /* GL_COMMAND_BARRIER_BIT             */, ///<
  PIXEL_BUFFER        = 128        /* GL_PIXEL_BUFFER_BARRIER_BIT        */, ///<
  TEXTURE_UPDATE      = 256        /* GL_TEXTURE_UPDATE_BARRIER_BIT      */, ///<
  BUFFER_UPDATE       = 512        /* GL_BUFFER_UPDATE_BARRIER_BIT       */, ///<
  FRAMEBUFFER         = 1024       /* GL_FRAMEBUFFER_BARRIER_BIT         */, ///<
  TRANSFORM_FEEDBACK  = 2048       /* GL_TRANSFORM_FEEDBACK_BARRIER_BIT  */, ///<
  ATOMIC_COUNTER      = 4096       /* GL_ATOMIC_COUNTER_BARRIER_BIT      */, ///<
  SHADER_STORAGE      = 8192       /* GL_SHADER_STORAGE_BARRIER_BIT      */, ///<
  ALL                 = 4294967295 /* GL_ALL_BARRIER_BITS                */  ///<
};
MAKE_ENUM_FLAG(BarrierType)

enum class RegionBarrierType : unsigned int {
  ATOMIC_COUNTER      = static_cast<unsigned int>(BarrierType::ATOMIC_COUNTER),      ///<
  FRAMEBUFFER         = static_cast<unsigned int>(BarrierType::FRAMEBUFFER),         ///<
  SHADER_IMAGE_ACCESS = static_cast<unsigned int>(BarrierType::SHADER_IMAGE_ACCESS), ///<
  SHADER_STORAGE      = static_cast<unsigned int>(BarrierType::SHADER_STORAGE),      ///<
  TEXTURE_FETCH       = static_cast<unsigned int>(BarrierType::TEXTURE_FETCH),       ///<
  UNIFORM             = static_cast<unsigned int>(BarrierType::UNIFORM),             ///<
  ALL                 = static_cast<unsigned int>(BarrierType::ALL)                  ///<
};
MAKE_ENUM_FLAG(RegionBarrierType)

enum class QueryType : unsigned int {
#if !defined(USE_OPENGL_ES)
  SAMPLES                       = 35092 /* GL_SAMPLES_PASSED                        */, ///<
#endif
  ANY_SAMPLES                   = 35887 /* GL_ANY_SAMPLES_PASSED                    */, ///<
  ANY_SAMPLES_CONSERVATIVE      = 36202 /* GL_ANY_SAMPLES_PASSED_CONSERVATIVE       */, ///<
#if !defined(USE_OPENGL_ES)
  PRIMITIVES                    = 35975 /* GL_PRIMITIVES_GENERATED                  */, ///<
#endif
  TRANSFORM_FEEDBACK_PRIMITIVES = 35976 /* GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN */, ///<
#if !defined(USE_OPENGL_ES)
  TIME_ELAPSED                  = 35007 /* GL_TIME_ELAPSED                          */  ///<
#endif
};

enum class RenderObjectType : unsigned int {
  BUFFER             = 33504                                                   /* GL_BUFFER             */, ///<
  TEXTURE            = 5890                                                    /* GL_TEXTURE            */, ///<
  SAMPLER            = 33510                                                   /* GL_SAMPLER            */, ///<
  SHADER             = 33505                                                   /* GL_SHADER             */, ///<
  PROGRAM            = 33506                                                   /* GL_PROGRAM            */, ///<
  QUERY              = 33507                                                   /* GL_QUERY              */, ///<
  FRAMEBUFFER        = static_cast<unsigned int>(FramebufferType::FRAMEBUFFER) /* GL_FRAMEBUFFER        */, ///<
  RENDERBUFFER       = 36161                                                   /* GL_RENDERBUFFER       */, ///<
  VERTEX_ARRAY       = 32884                                                   /* GL_VERTEX_ARRAY       */, ///<
  PROGRAM_PIPELINE   = 33508                                                   /* GL_PROGRAM_PIPELINE   */, ///<
  TRANSFORM_FEEDBACK = 36386                                                   /* GL_TRANSFORM_FEEDBACK */  ///<
};

/// Possible errors obtained after any renderer call; see https://www.khronos.org/opengl/wiki/OpenGL_Error#Meaning_of_errors for a more detailed description.
/// \see Renderer::recoverErrors()
enum class ErrorCode : unsigned int {
  NONE                          = 0    /* GL_NO_ERROR                      */, ///< No error.
  INVALID_ENUM                  = 1280 /* GL_INVALID_ENUM                  */, ///< Invalid enumeration value given.
  INVALID_VALUE                 = 1281 /* GL_INVALID_VALUE                 */, ///< Invalid value given.
  INVALID_OPERATION             = 1282 /* GL_INVALID_OPERATION             */, ///< Invalid parameters combination given.
  STACK_OVERFLOW                = 1283 /* GL_STACK_OVERFLOW                */, ///< Stack overflow.
  STACK_UNDERFLOW               = 1284 /* GL_STACK_UNDERFLOW               */, ///< Stack underflow.
  OUT_OF_MEMORY                 = 1285 /* GL_OUT_OF_MEMORY                 */, ///< Out of memory; the result of the required operation is undefined.
  INVALID_FRAMEBUFFER_OPERATION = 1286 /* GL_INVALID_FRAMEBUFFER_OPERATION */, ///< Operation asked on an incomplete framebuffer.
  CONTEXT_LOST                  = 1287 /* GL_CONTEXT_LOST                  */  ///< Context lost due to a GPU reset. Requires OpenGL 4.5+.
};

/// ErrorCodes structure, holding codes of errors that may have happened on prior Renderer calls.
struct ErrorCodes {
  /// Checks if no error code has been set.
  /// \return True if no code has been set, false otherwise.
  bool isEmpty() const noexcept { return codes.none(); }
  /// Checks if the given error code has been set.
  /// \param code Error code to be checked.
  /// \return True if the code has been set, false otherwise.
  constexpr bool get(ErrorCode code) const { return codes[static_cast<unsigned int>(code) - static_cast<unsigned int>(ErrorCode::INVALID_ENUM)]; }
  /// Checks if the given error code has been set.
  /// \param code Error code to be checked.
  /// \return True if the code has been set, false otherwise.
  constexpr bool operator[](ErrorCode code) const { return get(code); }

  std::bitset<8> codes {};
};

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void initialize();
  static bool isInitialized() noexcept { return s_isInitialized; }
  static int getMajorVersion() noexcept { return s_majorVersion; }
  static int getMinorVersion() noexcept { return s_minorVersion; }
  /// Checks that the current version is higher than or equal to the given one.
  /// \param major Major version.
  /// \param minor Minor version.
  /// \return True if the given version is higher than or equal to the current one, false otherwise.
  static bool checkVersion(int major, int minor) noexcept { return (s_majorVersion > major || (s_majorVersion == major && s_minorVersion >= minor)); }
  static bool isExtensionSupported(const std::string& extension) { return (s_extensions.find(extension) != s_extensions.cend()); }
  static void enable(Capability capability);
  static void disable(Capability capability);
  static bool isEnabled(Capability capability);
  static std::string getContextInfo(ContextInfo info);
  /// Gets the name of the extension located at the given index.
  /// \see getParameter(StateParameter::EXTENSION_COUNT)
  /// \param extIndex Index of the extension. Must be less than the total number of extensions.
  /// \return Name of the extension.
  static std::string getExtension(unsigned int extIndex);
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
  static void clear(MaskType mask);
  static void setDepthFunction(DepthStencilFunction func);
  /// Sets the function to evaluate for stencil testing.
  /// \param func Function to be evaluated.
  /// \param ref Reference value to compare the stencil with.
  /// \param mask Bitmask to compare the reference & the stencil with.
  /// \param orientation Face orientation for which to evaluate the function.
  static void setStencilFunction(DepthStencilFunction func, int ref, unsigned int mask, FaceOrientation orientation = FaceOrientation::FRONT_BACK);
  /// Sets operations to perform on stencil tests.
  /// \param stencilFailOp Action to be performed if the stencil test fails.
  /// \param depthFailOp Action to be performed if the stencil test succeeds, but the depth test fails.
  /// \param successOp Action to be performed if both stencil & depth tests succeed, or if only the former does and there is no depth testing or no depth buffer.
  /// \param orientation Face orientation for which to set the operations.
  static void setStencilOperations(StencilOperation stencilFailOp,
                                   StencilOperation depthFailOp,
                                   StencilOperation successOp,
                                   FaceOrientation orientation = FaceOrientation::FRONT_BACK);
  /// Enables overwriting stencil values to the bits represented by the given mask.
  /// \param mask Bitmask defining which stencil bits can be written.
  /// \param orientation Face orientation for which to set the mask.
  static void setStencilMask(unsigned int mask, FaceOrientation orientation = FaceOrientation::FRONT_BACK);
  static void setBlendFunction(BlendFactor source, BlendFactor destination);
  static void setFaceCulling(FaceOrientation orientation);
#if !defined(USE_OPENGL_ES)
  static void setPolygonMode(FaceOrientation orientation, PolygonMode mode);
  static void setClipControl(ClipOrigin origin, ClipDepth depth);
  static void setPatchVertexCount(int value);
  static void setPatchParameter(PatchParameter param, const float* values);
#endif
  static void setPixelStorage(PixelStorage storage, unsigned int value);
  static void recoverFrame(unsigned int width, unsigned int height, TextureFormat format, PixelDataType dataType, void* data);
  static void generateVertexArrays(unsigned int count, unsigned int* indices);
  static void generateVertexArray(unsigned int& index) { generateVertexArrays(1, &index); }
  static void bindVertexArray(unsigned int index);
  static void unbindVertexArray() { bindVertexArray(0); }
  static void enableVertexAttribArray(unsigned int index);
  static void setVertexAttrib(unsigned int index, AttribDataType dataType, uint8_t size, unsigned int stride, unsigned int offset, bool normalize = false);
  static void setVertexAttribDivisor(unsigned int index, unsigned int divisor);
  static void deleteVertexArrays(unsigned int count, unsigned int* indices);
  static void deleteVertexArray(unsigned int& index) { deleteVertexArrays(1, &index); }
  static void generateBuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void generateBuffers(unsigned int (&indices)[N]) { generateBuffers(N, indices); }
  static void generateBuffer(unsigned int& index) { generateBuffers(1, &index); }
  static void bindBuffer(BufferType type, unsigned int index);
  static void unbindBuffer(BufferType type) { bindBuffer(type, 0); }
  static void bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex);
  static void bindBufferRange(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex, std::ptrdiff_t offset, std::ptrdiff_t size);
  static void sendBufferData(BufferType type, std::ptrdiff_t size, const void* data, BufferDataUsage usage);
  static void sendBufferSubData(BufferType type, std::ptrdiff_t offset, std::ptrdiff_t dataSize, const void* data);
  template <typename T> static void sendBufferSubData(BufferType type, std::ptrdiff_t offset, const T& data) { sendBufferSubData(type, offset,
                                                                                                                                 sizeof(T), &data); }
  static void deleteBuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteBuffers(unsigned int (&indices)[N]) { deleteBuffers(N, indices); }
  static void deleteBuffer(unsigned int& index) { deleteBuffers(1, &index); }
  static bool isTexture(unsigned int index);
  static void generateTextures(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void generateTextures(unsigned int (&indices)[N]) { generateTextures(N, indices); }
  static void generateTexture(unsigned int& index) { generateTextures(1, &index); }
  static void bindTexture(TextureType type, unsigned int index);
  static void unbindTexture(TextureType type) { bindTexture(type, 0); }
#if !defined(USE_WEBGL)
  static void bindImageTexture(unsigned int imageUnitIndex, unsigned int textureIndex, int textureLevel,
                               bool isLayered, int layer,
                               ImageAccess imgAccess, ImageInternalFormat imgFormat);
#endif
  static void activateTexture(unsigned int index);
  /// Sets a parameter to the currently bound texture.
  /// \param type Type of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param value Value to be set.
  static void setTextureParameter(TextureType type, TextureParam param, int value);
  /// Sets a parameter to the currently bound texture.
  /// \param type Type of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param value Value to be set.
  static void setTextureParameter(TextureType type, TextureParam param, float value);
  /// Sets a parameter to the currently bound texture.
  /// \param type Type of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param values Values to be set.
  static void setTextureParameter(TextureType type, TextureParam param, const int* values);
  /// Sets a parameter to the currently bound texture.
  /// \param type Type of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param values Values to be set.
  static void setTextureParameter(TextureType type, TextureParam param, const float* values);
  /// Sets a parameter to the currently bound texture.
  /// \param type Type of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param value Value to be set.
  static void setTextureParameter(TextureType type, TextureParam param, TextureParamValue value) { setTextureParameter(type, param, static_cast<int>(value)); }
#if !defined(USE_OPENGL_ES)
  /// Sets a parameter to the given texture.
  /// \note Requires OpenGL 4.5+.
  /// \param textureIndex Index of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param value Value to be set.
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, int value);
  /// Sets a parameter to the given texture.
  /// \note Requires OpenGL 4.5+.
  /// \param textureIndex Index of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param value Value to be set.
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, float value);
  /// Sets a parameter to the given texture.
  /// \note Requires OpenGL 4.5+.
  /// \param textureIndex Index of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param values Values to be set.
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, const int* values);
  /// Sets a parameter to the given texture.
  /// \note Requires OpenGL 4.5+.
  /// \param textureIndex Index of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param values Values to be set.
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, const float* values);
  /// Sets a parameter to the given texture.
  /// \note Requires OpenGL 4.5+.
  /// \param textureIndex Index of the texture to set the parameter to.
  /// \param param Parameter to set.
  /// \param value Value to be set.
  static void setTextureParameter(unsigned int textureIndex, TextureParam param, TextureParamValue value) { setTextureParameter(textureIndex, param,
                                                                                                                                static_cast<int>(value)); }
  /// Sends the image's data corresponding to the currently bound 1D texture.
  /// \note Unavailable with OpenGL ES; use a Nx1 2D texture instead.
  /// \param type Type of the texture.
  /// \param mipmapLevel Mipmap (level of detail) of the texture. 0 is the most detailed.
  /// \param internalFormat Image internal format.
  /// \param width Image width.
  /// \param format Image format.
  /// \param dataType Type of the data to be sent.
  /// \param data Data to be sent.
  static void sendImageData1D(TextureType type,
                              unsigned int mipmapLevel,
                              TextureInternalFormat internalFormat,
                              unsigned int width,
                              TextureFormat format,
                              PixelDataType dataType, const void* data);
  /// Sends the image's sub-data corresponding to the currently bound 1D texture.
  /// \note Unavailable with OpenGL ES; use a Nx1 2D texture instead.
  /// \param type Type of the texture.
  /// \param mipmapLevel Mipmap (level of detail) of the texture. 0 is the most detailed.
  /// \param offsetX Width offset.
  /// \param width Image width.
  /// \param format Image format.
  /// \param dataType Type of the data to be sent.
  /// \param data Data to be sent.
  static void sendImageSubData1D(TextureType type,
                                 unsigned int mipmapLevel,
                                 unsigned int offsetX,
                                 unsigned int width,
                                 TextureFormat format,
                                 PixelDataType dataType, const void* data);
#endif
  /// Sends the image's data corresponding to the currently bound 2D texture.
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
                              PixelDataType dataType, const void* data);
  /// Sends the image's sub-data corresponding to the currently bound 2D texture.
  /// \param type Type of the texture.
  /// \param mipmapLevel Mipmap (level of detail) of the texture. 0 is the most detailed.
  /// \param offsetX Width offset.
  /// \param offsetY Height offset.
  /// \param width Image width.
  /// \param height Image height.
  /// \param format Image format.
  /// \param dataType Type of the data to be sent.
  /// \param data Data to be sent.
  static void sendImageSubData2D(TextureType type,
                                 unsigned int mipmapLevel,
                                 unsigned int offsetX, unsigned int offsetY,
                                 unsigned int width, unsigned int height,
                                 TextureFormat format,
                                 PixelDataType dataType, const void* data);
  /// Sends the image's data corresponding to the currently bound 3D texture.
  /// \param type Type of the texture.
  /// \param mipmapLevel Mipmap (level of detail) of the texture. 0 is the most detailed.
  /// \param internalFormat Image internal format.
  /// \param width Image width.
  /// \param height Image height.
  /// \param depth Image depth.
  /// \param format Image format.
  /// \param dataType Type of the data to be sent.
  /// \param data Data to be sent.
  static void sendImageData3D(TextureType type,
                              unsigned int mipmapLevel,
                              TextureInternalFormat internalFormat,
                              unsigned int width, unsigned int height, unsigned depth,
                              TextureFormat format,
                              PixelDataType dataType, const void* data);
  /// Sends the image's sub-data corresponding to the currently bound 3D texture.
  /// \param type Type of the texture.
  /// \param mipmapLevel Mipmap (level of detail) of the texture. 0 is the most detailed.
  /// \param offsetX Width offset.
  /// \param offsetY Height offset.
  /// \param offsetZ Depth offset.
  /// \param width Image width.
  /// \param height Image height.
  /// \param depth Image depth.
  /// \param format Image format.
  /// \param dataType Type of the data to be sent.
  /// \param data Data to be sent.
  static void sendImageSubData3D(TextureType type,
                                 unsigned int mipmapLevel,
                                 unsigned int offsetX, unsigned int offsetY, unsigned int offsetZ,
                                 unsigned int width, unsigned int height, unsigned depth,
                                 TextureFormat format,
                                 PixelDataType dataType, const void* data);
#if !defined(USE_OPENGL_ES)
  static void recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, int* values);
  static void recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, float* values);
  static int recoverTextureWidth(TextureType type, unsigned int mipmapLevel = 0);
  static int recoverTextureHeight(TextureType type, unsigned int mipmapLevel = 0);
  static int recoverTextureDepth(TextureType type, unsigned int mipmapLevel = 0);
  static TextureInternalFormat recoverTextureInternalFormat(TextureType type, unsigned int mipmapLevel = 0);
  static void recoverTextureData(TextureType type, unsigned int mipmapLevel, TextureFormat format, PixelDataType dataType, void* data);
#endif
  /// Generate mipmaps (levels of detail) for the currently bound texture.
  /// \param type Type of the texture to generate mipmaps for.
  static void generateMipmap(TextureType type);
#if !defined(USE_OPENGL_ES)
  /// Generate mipmaps (levels of detail) for the given texture.
  /// \note Requires OpenGL 4.5+.
  /// \param textureIndex Index of the texture to generate mipmaps for.
  static void generateMipmap(unsigned int textureIndex);
#endif
  static void deleteTextures(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteTextures(unsigned int (&indices)[N]) { deleteTextures(N, indices); }
  static void deleteTexture(unsigned int& index) { deleteTextures(1, &index); }
  static void resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height);
  static unsigned int createProgram();
  static void getProgramParameter(unsigned int index, ProgramParameter parameter, int* parameters);
  static bool isProgramLinked(unsigned int index);
  static unsigned int recoverActiveUniformCount(unsigned int programIndex);
  static std::vector<unsigned int> recoverAttachedShaders(unsigned int programIndex);
  static void linkProgram(unsigned int index);
  static void useProgram(unsigned int index);
  static void deleteProgram(unsigned int index);
  static unsigned int createShader(ShaderType type);
  static int recoverShaderInfo(unsigned int index, ShaderInfo info);
  static ShaderType recoverShaderType(unsigned int index) { return static_cast<ShaderType>(recoverShaderInfo(index, ShaderInfo::TYPE)); }
  static bool isShaderCompiled(unsigned int index) { return (recoverShaderInfo(index, ShaderInfo::COMPILE_STATUS) == 1); }
  static void sendShaderSource(unsigned int index, const char* source, int length);
  static void sendShaderSource(unsigned int index, const std::string& source) { sendShaderSource(index, source.c_str(), static_cast<int>(source.size())); }
  static void sendShaderSource(unsigned int index, std::string_view source) { sendShaderSource(index, source.data(), static_cast<int>(source.size())); }
  static std::string recoverShaderSource(unsigned int index);
  static void compileShader(unsigned int index);
  static void attachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void detachShader(unsigned int programIndex, unsigned int shaderIndex);
  static bool isShaderAttached(unsigned int programIndex, unsigned int shaderIndex);
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
  /// \see recoverActiveUniformCount().
  static void recoverUniformInfo(unsigned int programIndex, unsigned int uniformIndex, UniformType& type, std::string& name, int* size = nullptr);
  static UniformType recoverUniformType(unsigned int programIndex, unsigned int uniformIndex);
  static std::string recoverUniformName(unsigned int programIndex, unsigned int uniformIndex);
  static void recoverUniformData(unsigned int programIndex, int uniformIndex, int* data);
  static void recoverUniformData(unsigned int programIndex, int uniformIndex, unsigned int* data);
  static void recoverUniformData(unsigned int programIndex, int uniformIndex, float* data);
#if !defined(USE_OPENGL_ES)
  static void recoverUniformData(unsigned int programIndex, int uniformIndex, double* data);
#endif
  static void bindUniformBlock(unsigned int programIndex, unsigned int uniformBlockIndex, unsigned int bindingIndex);
  static unsigned int recoverUniformBlockIndex(unsigned int programIndex, const char* uniformName);
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
  /// Sends an integer 1D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector1i(int uniformIndex, const int* values, int count = 1);
  /// Sends an integer 2D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector2i(int uniformIndex, const int* values, int count = 1);
  /// Sends an integer 3D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector3i(int uniformIndex, const int* values, int count = 1);
  /// Sends an integer 4D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector4i(int uniformIndex, const int* values, int count = 1);
  /// Sends an unsigned integer 1D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector1ui(int uniformIndex, const unsigned int* values, int count = 1);
  /// Sends an unsigned integer 2D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector2ui(int uniformIndex, const unsigned int* values, int count = 1);
  /// Sends an unsigned integer 3D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector3ui(int uniformIndex, const unsigned int* values, int count = 1);
  /// Sends an unsigned integer 4D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param values Array of values to be sent.
  /// \param count Number of vectors to be sent.
  static void sendUniformVector4ui(int uniformIndex, const unsigned int* values, int count = 1);
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
  static void bindFramebuffer(unsigned int index, FramebufferType type = FramebufferType::FRAMEBUFFER);
  static void unbindFramebuffer(FramebufferType type = FramebufferType::FRAMEBUFFER) { bindFramebuffer(0, type); }
  static FramebufferStatus getFramebufferStatus(FramebufferType type = FramebufferType::FRAMEBUFFER);
  static bool isFramebufferComplete(FramebufferType type = FramebufferType::FRAMEBUFFER) { return getFramebufferStatus(type) == FramebufferStatus::COMPLETE; }
#if !defined(USE_OPENGL_ES)
  static void setFramebufferTexture(FramebufferAttachment attachment,
                                    unsigned int textureIndex, unsigned int mipmapLevel,
                                    FramebufferType type = FramebufferType::FRAMEBUFFER);
  static void setFramebufferTexture1D(FramebufferAttachment attachment,
                                      unsigned int textureIndex, unsigned int mipmapLevel,
                                      FramebufferType type = FramebufferType::FRAMEBUFFER);
#endif
  static void setFramebufferTexture2D(FramebufferAttachment attachment,
                                      unsigned int textureIndex, unsigned int mipmapLevel,
                                      TextureType textureType = TextureType::TEXTURE_2D,
                                      FramebufferType type = FramebufferType::FRAMEBUFFER);
#if !defined(USE_OPENGL_ES)
  static void setFramebufferTexture3D(FramebufferAttachment attachment,
                                      unsigned int textureIndex, unsigned int mipmapLevel, unsigned int layer,
                                      FramebufferType type = FramebufferType::FRAMEBUFFER);
#endif
  static void setReadBuffer(ReadBuffer buffer);
  static void setDrawBuffers(unsigned int count, const DrawBuffer* buffers);
  template <std::size_t N> static void setDrawBuffers(DrawBuffer (&buffers)[N]) { setDrawBuffers(N, buffers); }
  static void blitFramebuffer(int readMinX, int readMinY, int readMaxX, int readMaxY,
                              int writeMinX, int writeMinY, int writeMaxX, int writeMaxY,
                              MaskType mask, BlitFilter filter);
  static void deleteFramebuffers(unsigned int count, unsigned int* indices);
  template <std::size_t N> static void deleteFramebuffers(unsigned int (&indices)[N]) { deleteFramebuffers(N, indices); }
  static void deleteFramebuffer(unsigned int& index) { deleteFramebuffers(1, &index); }
  static void drawArrays(PrimitiveType type, unsigned int first, unsigned int count);
  static void drawArrays(PrimitiveType type, unsigned int count) { drawArrays(type, 0, count); }
  static void drawArraysInstanced(PrimitiveType type, unsigned int first, unsigned int primitiveCount, unsigned int instanceCount);
  static void drawArraysInstanced(PrimitiveType type, unsigned int primitiveCount,
                                  unsigned int instanceCount) { drawArraysInstanced(type, 0, primitiveCount, instanceCount); }
  static void drawElements(PrimitiveType type, unsigned int count, ElementDataType dataType, const void* indices);
  static void drawElements(PrimitiveType type, unsigned int count, const uint8_t* indices) { drawElements(type, count, ElementDataType::UBYTE, indices); }
  static void drawElements(PrimitiveType type, unsigned int count, const unsigned short* indices) { drawElements(type, count,
                                                                                                                 ElementDataType::USHORT, indices); }
  static void drawElements(PrimitiveType type, unsigned int count, const unsigned int* indices) { drawElements(type, count, ElementDataType::UINT, indices); }
  static void drawElements(PrimitiveType type, unsigned int count) { drawElements(type, count, ElementDataType::UINT, nullptr); }
  static void drawElementsInstanced(PrimitiveType type, unsigned int primitiveCount, ElementDataType dataType, const void* indices, unsigned int instanceCount);
  static void drawElementsInstanced(PrimitiveType type, unsigned int primitiveCount, const uint8_t* indices, unsigned int instanceCount) {
    drawElementsInstanced(type, primitiveCount, ElementDataType::UBYTE, indices, instanceCount);
  }
  static void drawElementsInstanced(PrimitiveType type, unsigned int primitiveCount, const unsigned short* indices, unsigned int instanceCount) {
    drawElementsInstanced(type, primitiveCount, ElementDataType::USHORT, indices, instanceCount);
  }
  static void drawElementsInstanced(PrimitiveType type, unsigned int primitiveCount, const unsigned int* indices, unsigned int instanceCount) {
    drawElementsInstanced(type, primitiveCount, ElementDataType::UINT, indices, instanceCount);
  }
  static void drawElementsInstanced(PrimitiveType type, unsigned int primitiveCount, unsigned int instanceCount) {
    drawElementsInstanced(type, primitiveCount, ElementDataType::UINT, nullptr, instanceCount);
  }
  static void dispatchCompute(unsigned int groupCountX, unsigned int groupCountY = 1, unsigned int groupCountZ = 1);
  /// Sets a memory synchronization barrier.
  /// \note Requires OpenGL 4.2+ or ES 3.1+.
  /// \param type Type of the barrier to be set.
  static void setMemoryBarrier(BarrierType type);
  /// Sets a localized memory synchronization barrier.
  /// \note Requires OpenGL 4.5+ or ES 3.1+.
  /// \param type Type of the barrier to be set.
  static void setMemoryBarrierByRegion(RegionBarrierType type);
  static void generateQueries(unsigned int count, unsigned int* indices);
  static void generateQuery(unsigned int& index) { generateQueries(1, &index); }
  static void beginQuery(QueryType type, unsigned int index);
  static void endQuery(QueryType type);
#if !defined (USE_OPENGL_ES)
  static void recoverQueryResult(unsigned int index, int64_t& result);
  static void recoverQueryResult(unsigned int index, uint64_t& result);
#endif
  static void deleteQueries(unsigned int count, unsigned int* indices);
  static void deleteQuery(unsigned int& index) { deleteQueries(1, &index); }
#if !defined (USE_OPENGL_ES)
  /// Assigns a label to a graphic object.
  /// \note Requires OpenGL 4.3+.
  /// \param type Type of the object to assign the label to.
  /// \param objectIndex Index of the object to assign the label to.
  /// \param label Label to assign; must be null-terminated.
  static void setLabel(RenderObjectType type, unsigned int objectIndex, const char* label);
  /// Assigns a label to a graphic object.
  /// \note Requires OpenGL 4.3+.
  /// \param type Type of the object to assign the label to.
  /// \param objectIndex Index of the object to assign the label to.
  /// \param label Label to assign.
  static void setLabel(RenderObjectType type, unsigned int objectIndex, const std::string& label) { setLabel(type, objectIndex, label.c_str()); }
  /// Removes label from a graphic object.
  /// \note Requires OpenGL 4.3+.
  /// \param type Type of the object to remove the label from.
  /// \param objectIndex Index of the object to remove the label from.
  static void resetLabel(RenderObjectType type, unsigned int objectIndex) { setLabel(type, objectIndex, nullptr); }
  /// Retrieves a previously assigned label on a given object.
  /// \note Requires OpenGL 4.3+.
  /// \param type Type of the object to retrieve the label from.
  /// \param objectIndex Index of the object to retrieve the label from.
  /// \return Label of the object.
  static std::string recoverLabel(RenderObjectType type, unsigned int objectIndex);
  /// Adds an entry into the debug group stack. This can be used to define regions which are shown in graphics debuggers.
  /// \note Requires OpenGL 4.3+.
  /// \param name Name of the group.
  static void pushDebugGroup(const std::string& name);
  /// Removes the latest pushed group.
  /// \note Requires OpenGL 4.3+.
  static void popDebugGroup();
#endif
  static ErrorCodes recoverErrors() noexcept;
  static bool hasErrors() noexcept { return !recoverErrors().isEmpty(); }
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

  static inline int s_majorVersion {};
  static inline int s_minorVersion {};
  static inline std::unordered_set<std::string> s_extensions {};
};

} // namespace Raz

#endif //RAZ_RENDERER_HPP
