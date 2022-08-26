#include "GL/glew.h"
#include "RaZ/Data/Color.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <utility>

namespace Raz {

namespace {

TextureFormat recoverFormat(const Image& image) {
  TextureFormat colorFormat {};

  switch (image.getColorspace()) {
    case ImageColorspace::GRAY:
      colorFormat = TextureFormat::RED;
      break;

    case ImageColorspace::GRAY_ALPHA:
      colorFormat = TextureFormat::RG;
      break;

    case ImageColorspace::RGB:
    case ImageColorspace::SRGB:
    default:
      colorFormat = TextureFormat::RGB;
      break;

    case ImageColorspace::RGBA:
    case ImageColorspace::SRGBA:
      colorFormat = TextureFormat::RGBA;
      break;

    case ImageColorspace::DEPTH:
      colorFormat = TextureFormat::DEPTH;
      break;
  }

  return colorFormat;
}

TextureInternalFormat recoverInternalFormat(const Image& image) {
  // If the image is of a byte data type and not an sRGB colorspace, its internal format is the same as its format
  if (image.getDataType() == ImageDataType::BYTE && image.getColorspace() != ImageColorspace::SRGB && image.getColorspace() != ImageColorspace::SRGBA)
    return static_cast<TextureInternalFormat>(recoverFormat(image));

  TextureInternalFormat colorFormat {};

  switch (image.getColorspace()) {
    case ImageColorspace::GRAY:
      colorFormat = TextureInternalFormat::R16F;
      break;

    case ImageColorspace::GRAY_ALPHA:
      colorFormat = TextureInternalFormat::RG16F;
      break;

    case ImageColorspace::RGB:
    default:
      colorFormat = TextureInternalFormat::RGB16F;
      break;

    case ImageColorspace::RGBA:
      colorFormat = TextureInternalFormat::RGBA16F;
      break;

    case ImageColorspace::SRGB:
      colorFormat = TextureInternalFormat::SRGB8;
      break;

    case ImageColorspace::SRGBA:
      colorFormat = TextureInternalFormat::SRGBA8;
      break;

    case ImageColorspace::DEPTH:
      colorFormat = TextureInternalFormat::DEPTH32F;
      break;
  }

  return colorFormat;
}

} // namespace

Texture::Texture() {
  Logger::debug("[Texture] Creating...");
  Renderer::generateTexture(m_index);
  Logger::debug("[Texture] Created (ID: " + std::to_string(m_index) + ')');
}

Texture::Texture(ImageColorspace colorspace)
  : Texture(colorspace, (colorspace == ImageColorspace::DEPTH ? ImageDataType::FLOAT : ImageDataType::BYTE)) {}

Texture::Texture(ImageColorspace colorspace, ImageDataType dataType) : Texture() {
  m_image = Image(colorspace, dataType);

  bind();

  const TextureParamValue textureParam = (colorspace == ImageColorspace::DEPTH ? TextureParamValue::NEAREST : TextureParamValue::LINEAR);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, textureParam);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, textureParam);

  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_S, TextureParamValue::CLAMP_TO_EDGE);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_T, TextureParamValue::CLAMP_TO_EDGE);

  unbind();
}

Texture::Texture(unsigned int width, unsigned int height, ImageColorspace colorspace, ImageDataType dataType)
  : Texture(colorspace, dataType) { resize(width, height); }

Texture::Texture(const Color& color)
  : Texture() { makePlainColored(color); }

Texture::Texture(Texture&& texture) noexcept
  : m_index{ std::exchange(texture.m_index, std::numeric_limits<unsigned int>::max()) },
    m_image{ std::move(texture.m_image) } {}

void Texture::load(Image&& image, bool createMipmaps) {
  m_image = std::move(image);
  load(m_image, createMipmaps);
}

void Texture::load(const Image& image, bool createMipmaps) {
  if (image.isEmpty()) {
    // Image not found, defaulting texture to pure white
    makePlainColored(ColorPreset::White);
    return;
  }

  bind();

  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_S, TextureParamValue::REPEAT);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_T, TextureParamValue::REPEAT);

  Renderer::setTextureParameter(TextureType::TEXTURE_2D,
                                TextureParam::MINIFY_FILTER,
                                (createMipmaps ? TextureParamValue::LINEAR_MIPMAP_LINEAR : TextureParamValue::LINEAR));
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);

  if (image.getColorspace() == ImageColorspace::GRAY || image.getColorspace() == ImageColorspace::GRAY_ALPHA) {
    const std::array<int, 4> swizzle = { GL_RED,
                                         GL_RED,
                                         GL_RED,
                                         (image.getColorspace() == ImageColorspace::GRAY_ALPHA ? GL_GREEN : GL_ONE) };
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::SWIZZLE_RGBA, swizzle.data());
  }

  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(image),
                            image.getWidth(),
                            image.getHeight(),
                            recoverFormat(image),
                            (image.getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT : TextureDataType::UBYTE),
                            image.getDataPtr());

  if (createMipmaps)
    Renderer::generateMipmap(TextureType::TEXTURE_2D);

  unbind();
}

void Texture::bind() const {
  Renderer::bindTexture(TextureType::TEXTURE_2D, m_index);
}

void Texture::unbind() const {
  Renderer::unbindTexture(TextureType::TEXTURE_2D);
}

void Texture::resize(unsigned int width, unsigned int height) {
  m_image.m_width  = width;
  m_image.m_height = height;

  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(m_image),
                            width,
                            height,
                            recoverFormat(m_image),
                            (m_image.m_dataType == ImageDataType::FLOAT ? TextureDataType::FLOAT : TextureDataType::UBYTE),
                            (m_image.isEmpty() ? nullptr : m_image.getDataPtr()));
  unbind();
}

Texture& Texture::operator=(Texture&& texture) noexcept {
  std::swap(m_index, texture.m_index);
  m_image = std::move(texture.m_image);

  return *this;
}

Texture::~Texture() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Logger::debug("[Texture] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteTexture(m_index);
  Logger::debug("[Texture] Destroyed");
}

void Texture::makePlainColored(const Color& color) {
  m_image.m_width  = 1;
  m_image.m_height = 1;

  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D, 0, TextureInternalFormat::RGB, 1, 1, TextureFormat::RGB, TextureDataType::UBYTE, Vec3b(color).getDataPtr());
  unbind();
}

} // namespace Raz
