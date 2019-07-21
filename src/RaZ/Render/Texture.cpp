#include <array>
#include <unordered_map>

#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

Texture::Texture() {
  Renderer::generateTextures(1, &m_index);
}

Texture::Texture(unsigned int width, unsigned int height, ImageColorspace colorspace) : Texture() {
  bind();

  if (colorspace != ImageColorspace::DEPTH) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 static_cast<unsigned int>(colorspace),
                 static_cast<int>(width),
                 static_cast<int>(height),
                 0,
                 static_cast<unsigned int>(colorspace),
                 GL_UNSIGNED_BYTE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT32F,
                 static_cast<int>(width),
                 static_cast<int>(height),
                 0,
                 static_cast<unsigned int>(colorspace),
                 GL_FLOAT,
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (m_image->getColorspace() == ImageColorspace::GRAY || m_image->getColorspace() == ImageColorspace::GRAY_ALPHA) {
      const std::array<int, 4> swizzle = { GL_RED,
                                           GL_RED,
                                           GL_RED,
                                           (m_image->getColorspace() == ImageColorspace::GRAY ? GL_ONE : GL_GREEN) };
      glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle.data());
    }

    // Default internal format is the image's own colorspace; modified if the image is a floating point one
    auto colorFormat = static_cast<int>(m_image->getColorspace());

    if (m_image->getDataType() == ImageDataType::FLOAT) {
      switch (m_image->getColorspace()) {
        case ImageColorspace::GRAY:
          colorFormat = GL_R16F;
          break;

        case ImageColorspace::GRAY_ALPHA:
          colorFormat = GL_RG16F;
          break;

        case ImageColorspace::RGB:
          colorFormat = GL_RGB16F;
          break;

        case ImageColorspace::RGBA:
          colorFormat = GL_RGBA16F;
          break;

        case ImageColorspace::DEPTH: // Unhandled here
          break;
      }
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 colorFormat,
                 static_cast<int>(m_image->getWidth()),
                 static_cast<int>(m_image->getHeight()),
                 0,
                 static_cast<unsigned int>(m_image->getColorspace()),
                 (m_image->getDataType() == ImageDataType::FLOAT ? GL_FLOAT : GL_UNSIGNED_BYTE),
                 m_image->getDataPtr());
    glGenerateMipmap(GL_TEXTURE_2D);
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
  //Renderer::deleteTextures(1, &m_index);
}

void Texture::makePlainColored(const Vec3b& color) const {
  bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, color.getDataPtr());
  unbind();
}

} // namespace Raz
