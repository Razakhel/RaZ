#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

Texture::Texture() {

}

Texture::Texture(ColorPreset preset, int bindingIndex) : Texture(bindingIndex) {
  const auto red   = static_cast<uint8_t>(static_cast<uint32_t>(preset & ColorPreset::RED) >> 16u);
  const auto green = static_cast<uint8_t>(static_cast<uint32_t>(preset & ColorPreset::GREEN) >> 8u);
  const auto blue  = static_cast<uint8_t>(static_cast<uint32_t>(preset & ColorPreset::BLUE));

  makePlainColored(Vec3b(red, green, blue));
}

Texture::Texture(unsigned int width, unsigned int height, int bindingIndex, ImageColorspace colorspace, bool createMipmaps) : Texture(bindingIndex) {
  m_image.m_colorspace = colorspace;

  if (colorspace == ImageColorspace::DEPTH) {
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::NEAREST);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::NEAREST);
  } else {
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);
  }

  resize(width, height);

  if (createMipmaps)
    Renderer::generateMipmap(TextureType::TEXTURE_2D);
}

Texture::Texture(Texture&& texture) noexcept
  : m_image{ std::move(texture.m_image) } {}

void Texture::load(Image image, bool createMipmaps) {
  m_image = std::move(image);
  load(createMipmaps);
}

void Texture::load(const FilePath& filePath, bool flipVertically, bool createMipmaps) {
  m_image.read(filePath, flipVertically);
  load(createMipmaps);
}

void Texture::resize(unsigned int width, unsigned int height) const {
  if (m_image.m_colorspace == ImageColorspace::DEPTH) {
    Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                              0,
                              TextureInternalFormat::DEPTH32F,
                              width,
                              height,
                              static_cast<TextureFormat>(m_image.m_colorspace),
                              TextureDataType::FLOAT,
                              (m_image.isEmpty() ? nullptr : m_image.getDataPtr()));
  } else {
    Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                              0,
                              static_cast<TextureInternalFormat>(m_image.m_colorspace),
                              width,
                              height,
                              static_cast<TextureFormat>(m_image.m_colorspace),
                              TextureDataType::UBYTE,
                              (m_image.isEmpty() ? nullptr : m_image.getDataPtr()));
  }
}

Texture& Texture::operator=(Texture&& texture) noexcept {
  m_image = std::move(texture.m_image);

  return *this;
}

Texture::~Texture() {

}

void Texture::load(bool createMipmaps) {
  if (m_image.isEmpty()) {
    // Image not found, defaulting texture to pure white
    makePlainColored(Vec3b(255));
    return;
  }

  // Default internal format is the image's own colorspace; modified if the image is a floating point one
  auto colorFormat = static_cast<TextureInternalFormat>(m_image.getColorspace());

  if (m_image.getDataType() == ImageDataType::FLOAT) {
    switch (m_image.getColorspace()) {
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

      case ImageColorspace::DEPTH:
        // Unhandled here
        break;
    }
  }

  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            colorFormat,
                            m_image.getWidth(),
                            m_image.getHeight(),
                            static_cast<TextureFormat>(m_image.m_colorspace),
                            (m_image.getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT : TextureDataType::UBYTE),
                            m_image.getDataPtr());

  if (createMipmaps)
    Renderer::generateMipmap(TextureType::TEXTURE_2D);
}

void Texture::makePlainColored(const Vec3b& color) const {

}

} // namespace Raz
