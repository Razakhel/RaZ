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

    case TextureColorspace::GRAY_ALPHA:
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
      return TextureInternalFormat::R16F;

    case TextureColorspace::GRAY_ALPHA:
      return TextureInternalFormat::RG16F;

    case TextureColorspace::RGB:
      return TextureInternalFormat::RGB16F;

    case TextureColorspace::RGBA:
      return TextureInternalFormat::RGBA16F;

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

Texture2D::Texture2D() {
  Logger::debug("[Texture] Creating...");
  Renderer::generateTexture(m_index);
  Logger::debug("[Texture] Created (ID: " + std::to_string(m_index) + ')');
}

Texture2D::Texture2D(unsigned int width, unsigned int height, TextureColorspace colorspace, TextureDataType dataType)
  : Texture2D(colorspace, dataType) { resize(width, height); }

void Texture2D::bind() const {
  Renderer::bindTexture(TextureType::TEXTURE_2D, m_index);
}

void Texture2D::unbind() const {
  Renderer::unbindTexture(TextureType::TEXTURE_2D);
}

void Texture2D::setFilter(TextureFilter minify, TextureFilter magnify) const {
  bind();
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, recoverParam(minify));
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, recoverParam(magnify));
  unbind();
}

void Texture2D::setFilter(TextureFilter minify, TextureFilter mipmapMinify, TextureFilter magnify) const {
  bind();
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, recoverParam(minify, mipmapMinify));
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, recoverParam(magnify));
  unbind();
}

void Texture2D::setWrapping(TextureWrapping wrapping) const {
  const TextureParamValue value = recoverParam(wrapping);

  bind();
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_S, value);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_T, value);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_R, value);
  unbind();
}

void Texture2D::setParameters(unsigned int width, unsigned int height, TextureColorspace colorspace) {
  setColorspace(colorspace);
  resize(width, height);
}

void Texture2D::setParameters(unsigned int width, unsigned int height, TextureColorspace colorspace, TextureDataType dataType) {
  setColorspace(colorspace, dataType);
  resize(width, height);
}

void Texture2D::setColorspace(TextureColorspace colorspace) {
  setColorspace(colorspace, (colorspace == TextureColorspace::DEPTH ? TextureDataType::FLOAT : TextureDataType::BYTE));
}

void Texture2D::setColorspace(TextureColorspace colorspace, TextureDataType dataType) {
  assert("Error: A depth texture must have a floating-point data type." && (colorspace != TextureColorspace::DEPTH || dataType == TextureDataType::FLOAT));

  m_colorspace = colorspace;
  m_dataType   = dataType;

  setFilter((m_colorspace == TextureColorspace::DEPTH ? TextureFilter::NEAREST : TextureFilter::LINEAR));
  setWrapping(TextureWrapping::CLAMP);
}

void Texture2D::resize(unsigned int width, unsigned int height) {
  m_width  = width;
  m_height = height;

  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            width,
                            height,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::FLOAT ? PixelDataType::FLOAT : PixelDataType::UBYTE),
                            nullptr);
  unbind();
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
  m_dataType   = (image.getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT : TextureDataType::BYTE);

  if (createMipmaps)
    setFilter(TextureFilter::LINEAR, TextureFilter::LINEAR, TextureFilter::LINEAR);
  else
    setFilter(TextureFilter::LINEAR);

  setWrapping(TextureWrapping::REPEAT);

  bind();

  if (m_colorspace == TextureColorspace::GRAY || m_colorspace == TextureColorspace::GRAY_ALPHA) {
    const std::array<int, 4> swizzle = { static_cast<int>(TextureFormat::RED),
                                         static_cast<int>(TextureFormat::RED),
                                         static_cast<int>(TextureFormat::RED),
                                         (m_colorspace == TextureColorspace::GRAY_ALPHA ? static_cast<int>(TextureFormat::GREEN) : 1) };
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::SWIZZLE_RGBA, swizzle.data());
  }

  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            m_height,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::FLOAT ? PixelDataType::FLOAT : PixelDataType::UBYTE),
                            image.getDataPtr());

  if (createMipmaps)
    Renderer::generateMipmap(TextureType::TEXTURE_2D);

  unbind();
}

#if !defined(USE_OPENGL_ES) // Renderer::recoverTextureData() is unavailable with OpenGL ES
Image Texture2D::recoverImage() const {
  Image img(m_width, m_height, static_cast<ImageColorspace>(m_colorspace), (m_dataType == TextureDataType::FLOAT ? ImageDataType::FLOAT : ImageDataType::BYTE));

  bind();
  Renderer::recoverTextureData(TextureType::TEXTURE_2D,
                               0,
                               recoverFormat(m_colorspace),
                               (m_dataType == TextureDataType::FLOAT ? PixelDataType::FLOAT : PixelDataType::UBYTE),
                               img.getDataPtr());
  unbind();

  return img;
}
#endif

Texture2D::~Texture2D() {
  if (!m_index.isValid())
    return;

  Logger::debug("[Texture] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteTexture(m_index);
  Logger::debug("[Texture] Destroyed");
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

} // namespace Raz
