#include <array>
#include <unordered_map>

#include "RaZ/Render/Texture.hpp"

namespace Raz {

Texture::Texture(uint8_t value) : Texture() {
  const std::array<uint8_t, 3> pixel = { value, value, value };

  bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel.data());
  unbind();
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

TexturePtr Texture::recoverTexture(TexturePreset texturePreset) {
  static const std::unordered_map<TexturePreset, TexturePtr> texturePresets = {
    { TexturePreset::BLACK, std::make_shared<Texture>(0) },
    { TexturePreset::WHITE, std::make_shared<Texture>(255) }
  };

  const auto materialParamsIter = texturePresets.find(texturePreset);

  if (materialParamsIter == texturePresets.end())
    throw std::invalid_argument("Error: Couldn't find the given texture preset");

  return materialParamsIter->second;
}

void Texture::load(const std::string& fileName) {
  const Image img(fileName);

  bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (img.getColorspace() == ImageColorspace::GRAY || img.getColorspace() == ImageColorspace::GRAY_ALPHA) {
    const std::array<int, 4> swizzle = { GL_RED, GL_RED, GL_RED, (img.getColorspace() == ImageColorspace::GRAY ? GL_ONE : GL_GREEN) };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle.data());
  }

  glTexImage2D(GL_TEXTURE_2D,
               0,
               static_cast<int>(img.getColorspace()),
               static_cast<int>(img.getWidth()),
               static_cast<int>(img.getHeight()),
               0,
               static_cast<unsigned int>(img.getColorspace()),
               GL_UNSIGNED_BYTE,
               img.getDataPtr());
  glGenerateMipmap(GL_TEXTURE_2D);
  unbind();
}

} // namespace Raz
