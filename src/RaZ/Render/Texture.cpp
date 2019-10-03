#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

Texture::Texture() {
  Renderer::generateTexture(m_index);
}

Texture::Texture(unsigned int width, unsigned int height, ImageColorspace colorspace) : Texture() {
  bind();

  if (colorspace != ImageColorspace::DEPTH) {
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);

    Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                              0,
                              static_cast<TextureInternalFormat>(colorspace),
                              static_cast<int>(width),
                              static_cast<int>(height),
                              static_cast<TextureFormat>(colorspace),
                              TextureDataType::UBYTE,
                              nullptr);
    Renderer::generateMipmap(TextureType::TEXTURE_2D);
  } else {
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::NEAREST);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::NEAREST);

    Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                              0,
                              TextureInternalFormat::DEPTH32F,
                              static_cast<int>(width),
                              static_cast<int>(height),
                              static_cast<TextureFormat>(colorspace),
                              TextureDataType::FLOAT,
                              nullptr);
  }

  unbind();
}

Texture::Texture(Texture&& texture) noexcept
  : m_index{ std::exchange(texture.m_index, GL_INVALID_INDEX) }, m_image{ std::move(texture.m_image) } {}

TexturePtr Texture::recoverTexture(TexturePreset preset) {
  static const std::array<TexturePtr, static_cast<std::size_t>(TexturePreset::PRESET_COUNT)> texturePresets = {
    Texture::create(0),  // BLACK
    Texture::create(255) // WHITE
  };

  return texturePresets[static_cast<std::size_t>(preset)];
}

void Texture::load(const std::string& filePath, bool flipVertically) {
  m_image = Image::create(filePath, flipVertically);

  if (!m_image->isEmpty()) {
    bind();
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_S, TextureParamValue::REPEAT);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_T, TextureParamValue::REPEAT);

    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR_MIPMAP_LINEAR);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);

    if (m_image->getColorspace() == ImageColorspace::GRAY || m_image->getColorspace() == ImageColorspace::GRAY_ALPHA) {
      const std::array<int, 4> swizzle = { GL_RED,
                                           GL_RED,
                                           GL_RED,
                                           (m_image->getColorspace() == ImageColorspace::GRAY ? GL_ONE : GL_GREEN) };
      Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::SWIZZLE_RGBA, swizzle.data());
    }

    // Default internal format is the image's own colorspace; modified if the image is a floating point one
    auto colorFormat = static_cast<TextureInternalFormat>(m_image->getColorspace());

    if (m_image->getDataType() == ImageDataType::FLOAT) {
      switch (m_image->getColorspace()) {
        case ImageColorspace::GRAY:
          colorFormat = TextureInternalFormat::RED16F;
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

        case ImageColorspace::DEPTH: // Unhandled here
          break;
      }
    }

    Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                              0,
                              colorFormat,
                              static_cast<int>(m_image->getWidth()),
                              static_cast<int>(m_image->getHeight()),
                              static_cast<TextureFormat>(m_image->getColorspace()),
                              (m_image->getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT : TextureDataType::UBYTE),
                              m_image->getDataPtr());
    Renderer::generateMipmap(TextureType::TEXTURE_2D);
    unbind();
  } else { // Image not found, deleting it & defaulting texture to pure white
    m_image.reset();
    makePlainColored(Vec3b(static_cast<uint8_t>(TexturePreset::WHITE)));
  }
}

void Texture::bind() const {
  Renderer::bindTexture(TextureType::TEXTURE_2D, m_index);
}

void Texture::unbind() const {
  Renderer::unbindTexture(TextureType::TEXTURE_2D);
}

Texture& Texture::operator=(Texture&& texture) noexcept {
  std::swap(m_index, texture.m_index);
  m_image = std::move(texture.m_image);

  return *this;
}

Texture::~Texture() {
  if (m_index == GL_INVALID_INDEX)
    return;

  glDeleteTextures(1, &m_index);

  // This currently goes on an infinite error-printing loop
  //Renderer::deleteTexture(m_index);
}

void Texture::makePlainColored(const Vec3b& color) const {
  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D, 0, TextureInternalFormat::RGB, 1, 1, TextureFormat::RGB, TextureDataType::UBYTE, color.getDataPtr());
  unbind();
}

} // namespace Raz
