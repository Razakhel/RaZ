#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <utility>

namespace Raz {

namespace {

inline TextureFormat recoverFormat(TextureColorspace colorspace) {
  switch (colorspace) {
    case TextureColorspace::INVALID:
      break;

    case TextureColorspace::GRAY:
      return TextureFormat::RED;

    case TextureColorspace::RG:
      return TextureFormat::RG;

    case TextureColorspace::RGB:
    case TextureColorspace::SRGB:
      return TextureFormat::RGB;

    case TextureColorspace::RGBA:
    case TextureColorspace::SRGBA:
      return TextureFormat::RGBA;

    case TextureColorspace::DEPTH:
      return TextureFormat::DEPTH;
  }

  throw std::invalid_argument("Error: Invalid texture colorspace");
}

inline TextureInternalFormat recoverInternalFormat(TextureColorspace colorspace, TextureDataType dataType) {
  // If the texture is of a byte data type and not an sRGB colorspace, its internal format is the same as its format
  if (dataType == TextureDataType::BYTE && colorspace != TextureColorspace::SRGB && colorspace != TextureColorspace::SRGBA)
    return static_cast<TextureInternalFormat>(recoverFormat(colorspace));

  switch (colorspace) {
    case TextureColorspace::INVALID:
      break;

    case TextureColorspace::GRAY:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::R16F : TextureInternalFormat::R32F);

    case TextureColorspace::RG:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::RG16F : TextureInternalFormat::RG32F);

    case TextureColorspace::RGB:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::RGB16F : TextureInternalFormat::RGB32F);

    case TextureColorspace::RGBA:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::RGBA16F : TextureInternalFormat::RGBA32F);

    case TextureColorspace::SRGB:
      return TextureInternalFormat::SRGB8;

    case TextureColorspace::SRGBA:
      return TextureInternalFormat::SRGBA8;

    case TextureColorspace::DEPTH:
      return TextureInternalFormat::DEPTH32F;
  }

  throw std::invalid_argument("Error: Invalid texture colorspace");
}

inline TextureParamValue recoverParam(TextureFilter filter) {
  switch (filter) {
    case TextureFilter::NEAREST:
      return TextureParamValue::NEAREST;

    case TextureFilter::LINEAR:
      return TextureParamValue::LINEAR;
  }

  throw std::invalid_argument("Error: Invalid texture filter");
}

inline TextureParamValue recoverParam(TextureFilter filter, TextureFilter mipmapFilter) {
  switch (filter) {
    case TextureFilter::NEAREST:
      return (mipmapFilter == TextureFilter::NEAREST ? TextureParamValue::NEAREST_MIPMAP_NEAREST : TextureParamValue::NEAREST_MIPMAP_LINEAR);

    case TextureFilter::LINEAR:
      return (mipmapFilter == TextureFilter::NEAREST ? TextureParamValue::LINEAR_MIPMAP_NEAREST : TextureParamValue::LINEAR_MIPMAP_LINEAR);
  }

  throw std::invalid_argument("Error: Invalid texture filter");
}

inline TextureParamValue recoverParam(TextureWrapping wrapping) {
  switch (wrapping) {
    case TextureWrapping::REPEAT:
      return TextureParamValue::REPEAT;

    case TextureWrapping::CLAMP:
      return TextureParamValue::CLAMP_TO_EDGE;
  }

  throw std::invalid_argument("Error: Invalid texture wrapping");
}

} // namespace

void Texture::bind() const {
  Renderer::bindTexture(m_type, m_index);
}

void Texture::unbind() const {
  Renderer::unbindTexture(m_type);
}

void Texture::setFilter(TextureFilter minify, TextureFilter magnify) const {
  bind();
  Renderer::setTextureParameter(m_type, TextureParam::MINIFY_FILTER, recoverParam(minify));
  Renderer::setTextureParameter(m_type, TextureParam::MAGNIFY_FILTER, recoverParam(magnify));
  unbind();
}

void Texture::setFilter(TextureFilter minify, TextureFilter mipmapMinify, TextureFilter magnify) const {
  bind();
  Renderer::setTextureParameter(m_type, TextureParam::MINIFY_FILTER, recoverParam(minify, mipmapMinify));
  Renderer::setTextureParameter(m_type, TextureParam::MAGNIFY_FILTER, recoverParam(magnify));
  unbind();
}

void Texture::setWrapping(TextureWrapping wrapping) const {
  const TextureParamValue value = recoverParam(wrapping);

  bind();
  Renderer::setTextureParameter(m_type, TextureParam::WRAP_S, value);
  Renderer::setTextureParameter(m_type, TextureParam::WRAP_T, value);
  Renderer::setTextureParameter(m_type, TextureParam::WRAP_R, value);
  unbind();
}

void Texture::setColorspace(TextureColorspace colorspace) {
  setColorspace(colorspace, (colorspace == TextureColorspace::DEPTH ? TextureDataType::FLOAT32 : TextureDataType::BYTE));
}

void Texture::setColorspace(TextureColorspace colorspace, TextureDataType dataType) {
  assert("Error: A depth texture must have a 32-bit floating-point data type."
      && (colorspace != TextureColorspace::DEPTH || dataType == TextureDataType::FLOAT32));
  assert("Error: A depth texture cannot be three-dimensional." && (colorspace != TextureColorspace::DEPTH || m_type != TextureType::TEXTURE_3D));

  m_colorspace = colorspace;
  m_dataType   = dataType;

  load();

  if (m_colorspace == TextureColorspace::DEPTH)
    setFilter(TextureFilter::NEAREST);
}

Texture::~Texture() {
  if (!m_index.isValid())
    return;

  Logger::debug("[Texture] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteTexture(m_index);
  Logger::debug("[Texture] Destroyed");
}

Texture::Texture(TextureType type) : m_type{ type } {
  Logger::debug("[Texture] Creating...");
  Renderer::generateTexture(m_index);
  Logger::debug("[Texture] Created (ID: " + std::to_string(m_index) + ')');

  setFilter(TextureFilter::LINEAR);
  setWrapping(TextureWrapping::CLAMP);
}

#if !defined(USE_OPENGL_ES)
Texture1D::Texture1D()
  : Texture(TextureType::TEXTURE_1D) {}

Texture1D::Texture1D(unsigned int width, TextureColorspace colorspace, TextureDataType dataType)
  : Texture1D(colorspace, dataType) { resize(width); }

void Texture1D::resize(unsigned int width) {
  m_width = width;

  load();
}

void Texture1D::load() const {
  if (m_colorspace == TextureColorspace::INVALID)
    return; // No colorspace has been set yet, the texture can't be loaded

  bind();
  Renderer::sendImageData1D(TextureType::TEXTURE_1D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            nullptr);
  unbind();
}

void Texture1D::makePlainColored(const Color& color) {
  m_width      = 1;
  m_colorspace = TextureColorspace::RGB;
  m_dataType   = TextureDataType::BYTE;

  bind();
  Renderer::sendImageData1D(TextureType::TEXTURE_1D, 0, TextureInternalFormat::RGB, 1, TextureFormat::RGB, PixelDataType::UBYTE, Vec3b(color).getDataPtr());
  unbind();
}
#endif

Texture2D::Texture2D()
  : Texture(TextureType::TEXTURE_2D) {}

Texture2D::Texture2D(unsigned int width, unsigned int height, TextureColorspace colorspace, TextureDataType dataType)
  : Texture2D(colorspace, dataType) { resize(width, height); }

void Texture2D::resize(unsigned int width, unsigned int height) {
  m_width  = width;
  m_height = height;

  load();
}

void Texture2D::load(const Image& image, bool createMipmaps) {
  if (image.isEmpty()) {
    // Image not found, defaulting texture to pure white
    makePlainColored(ColorPreset::White);
    return;
  }

  m_width      = image.getWidth();
  m_height     = image.getHeight();
  m_colorspace = static_cast<TextureColorspace>(image.getColorspace());
  m_dataType   = (image.getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT16 : TextureDataType::BYTE);

  if (createMipmaps)
    setFilter(TextureFilter::LINEAR, TextureFilter::LINEAR, TextureFilter::LINEAR);
  else
    setFilter(TextureFilter::LINEAR);

  setWrapping(TextureWrapping::REPEAT);

  bind();

  if (m_colorspace == TextureColorspace::GRAY || m_colorspace == TextureColorspace::RG) {
    const std::array<int, 4> swizzle = { static_cast<int>(TextureFormat::RED),
                                         static_cast<int>(TextureFormat::RED),
                                         static_cast<int>(TextureFormat::RED),
                                         (m_colorspace == TextureColorspace::RG ? static_cast<int>(TextureFormat::GREEN) : 1) };
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::SWIZZLE_RGBA, swizzle.data());
  }

  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            m_height,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            image.getDataPtr());

  if (createMipmaps)
    Renderer::generateMipmap(TextureType::TEXTURE_2D);

  unbind();
}

#if !defined(USE_OPENGL_ES) // Renderer::recoverTextureData() is unavailable with OpenGL ES
Image Texture2D::recoverImage() const {
  Image img(m_width, m_height, static_cast<ImageColorspace>(m_colorspace), (m_dataType == TextureDataType::BYTE ? ImageDataType::BYTE : ImageDataType::FLOAT));

  bind();
  Renderer::recoverTextureData(TextureType::TEXTURE_2D,
                               0,
                               recoverFormat(m_colorspace),
                               (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                               img.getDataPtr());
  unbind();

  return img;
}
#endif

void Texture2D::load() const {
  if (m_colorspace == TextureColorspace::INVALID)
    return; // No colorspace has been set yet, the texture can't be loaded

  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            m_height,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            nullptr);
  unbind();
}

void Texture2D::makePlainColored(const Color& color) {
  m_width      = 1;
  m_height     = 1;
  m_colorspace = TextureColorspace::RGB;
  m_dataType   = TextureDataType::BYTE;

  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D, 0, TextureInternalFormat::RGB, 1, 1, TextureFormat::RGB, PixelDataType::UBYTE, Vec3b(color).getDataPtr());
  unbind();
}

Texture3D::Texture3D()
  : Texture(TextureType::TEXTURE_3D) {}

Texture3D::Texture3D(unsigned int width, unsigned int height, unsigned int depth, TextureColorspace colorspace, TextureDataType dataType)
  : Texture3D(colorspace, dataType) { resize(width, height, depth); }

void Texture3D::resize(unsigned int width, unsigned int height, unsigned int depth) {
  m_width  = width;
  m_height = height;
  m_depth  = depth;

  load();
}

void Texture3D::load() const {
  if (m_colorspace == TextureColorspace::INVALID)
    return; // No colorspace has been set yet, the texture can't be loaded

  bind();
  Renderer::sendImageData3D(TextureType::TEXTURE_3D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            m_height,
                            m_depth,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            nullptr);
  unbind();
}

void Texture3D::makePlainColored(const Color& color) {
  m_width      = 1;
  m_height     = 1;
  m_depth      = 1;
  m_colorspace = TextureColorspace::RGB;
  m_dataType   = TextureDataType::BYTE;

  bind();
  Renderer::sendImageData3D(TextureType::TEXTURE_3D, 0, TextureInternalFormat::RGB, 1, 1, 1, TextureFormat::RGB, PixelDataType::UBYTE, Vec3b(color).getDataPtr());
  unbind();
}

} // namespace Raz
