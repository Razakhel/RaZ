#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include <memory>
#include <string>

#include "glew/include/GL/glew.h"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Image.hpp"

namespace Raz {

enum class TexturePreset : uint8_t { BLACK = 0,
                                     WHITE = 255 };

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class Texture {
public:
  Texture() { glGenTextures(1, &m_index); }
  explicit Texture(uint8_t value) : Texture() { makePlainColored(Vec3b(value)); }
  Texture(unsigned int width, unsigned int height, ImageColorspace colorspace = ImageColorspace::RGB);
  explicit Texture(const std::string& fileName) : Texture() { load(fileName); }

  GLuint getIndex() const { return m_index; }

  static TexturePtr recoverTexture(TexturePreset texturePreset);
  static void activate(uint8_t index) { glActiveTexture(GL_TEXTURE0 + index); }
  void load(const std::string& fileName);
  void save(const std::string& fileName, bool reverse = false) const { m_image->save(fileName, reverse); }
  void bind() const { glBindTexture(GL_TEXTURE_2D, m_index); }
  void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

  ~Texture() { glDeleteTextures(1, &m_index); }

private:
  void makePlainColored(const Vec3b& color) const;

  GLuint m_index {};
  ImagePtr m_image {};
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
