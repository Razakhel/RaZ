#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

Texture::Texture() {
  Renderer::generateTexture(m_index);
}

Texture::Texture(ColorPreset preset, int bindingIndex) : Texture(bindingIndex) {
  const auto red   = static_cast<uint8_t>(static_cast<uint32_t>(preset & ColorPreset::RED) >> 16u);
  const auto green = static_cast<uint8_t>(static_cast<uint32_t>(preset & ColorPreset::GREEN) >> 8u);
  const auto blue  = static_cast<uint8_t>(static_cast<uint32_t>(preset & ColorPreset::BLUE));

  makePlainColored(Vec3b(red, green, blue));
}

Texture::Texture(unsigned int width, unsigned int height, int bindingIndex, ImageColorspace colorspace, bool createMipmaps) : Texture(bindingIndex) {
  m_image.m_colorspace = colorspace;

  bind();

  if (colorspace != ImageColorspace::DEPTH) {
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);

    Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                              0,
                              static_cast<TextureInternalFormat>(colorspace),
                              width,
                              height,
                              static_cast<TextureFormat>(colorspace),
                              TextureDataType::UBYTE,
                              nullptr);
  } else {
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::NEAREST);
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::NEAREST);

    Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                              0,
                              TextureInternalFormat::DEPTH32F,
                              width,
                              height,
                              static_cast<TextureFormat>(colorspace),
                              TextureDataType::FLOAT,
                              nullptr);
  }

  if (createMipmaps)
    Renderer::generateMipmap(TextureType::TEXTURE_2D);

  unbind();
}

Texture::Texture(Texture&& texture) noexcept
  : m_index{ std::exchange(texture.m_index, std::numeric_limits<unsigned int>::max()) },
    m_bindingIndex{ std::exchange(texture.m_bindingIndex, std::numeric_limits<int>::max()) },
    m_image{ std::move(texture.m_image) } {}

void Texture::load(const FilePath& filePath, bool flipVertically, bool createMipmaps) {
  m_image.read(filePath, flipVertically);

  if (m_image.isEmpty()) {
    // Image not found, defaulting texture to pure white
    makePlainColored(Vec3b(255));
    return;
  }

  bind();
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_S, TextureParamValue::REPEAT);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::WRAP_T, TextureParamValue::REPEAT);

  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR_MIPMAP_LINEAR);
  Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);

  if (m_image.getColorspace() == ImageColorspace::GRAY || m_image.getColorspace() == ImageColorspace::GRAY_ALPHA) {
    const std::array<int, 4> swizzle = { GL_RED,
                                         GL_RED,
                                         GL_RED,
                                         (m_image.getColorspace() == ImageColorspace::GRAY ? GL_ONE : GL_GREEN) };
    Renderer::setTextureParameter(TextureType::TEXTURE_2D, TextureParam::SWIZZLE_RGBA, swizzle.data());
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

      case ImageColorspace::DEPTH: // Unhandled here
        break;
    }
  }

  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            colorFormat,
                            m_image.getWidth(),
                            m_image.getHeight(),
                            static_cast<TextureFormat>(m_image.getColorspace()),
                            (m_image.getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT : TextureDataType::UBYTE),
                            m_image.getDataPtr());

  if (createMipmaps)
    Renderer::generateMipmap(TextureType::TEXTURE_2D);

  unbind();
}

void Texture::activate() const {
  assert("Error: The texture trying to be activated has an invalid binding index." && m_bindingIndex != std::numeric_limits<int>::max());

  Renderer::activateTexture(static_cast<unsigned int>(m_bindingIndex));
}

void Texture::bind() const {
  Renderer::bindTexture(TextureType::TEXTURE_2D, m_index);
}

void Texture::unbind() const {
  Renderer::unbindTexture(TextureType::TEXTURE_2D);
}

Texture& Texture::operator=(Texture&& texture) noexcept {
  std::swap(m_index, texture.m_index);
  std::swap(m_bindingIndex, texture.m_bindingIndex);
  m_image = std::move(texture.m_image);

  return *this;
}

Texture::~Texture() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteTexture(m_index);
}

void Texture::makePlainColored(const Vec3b& color) const {
  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D, 0, TextureInternalFormat::RGB, 1, 1, TextureFormat::RGB, TextureDataType::UBYTE, color.getDataPtr());
  unbind();
}

} // namespace Raz
