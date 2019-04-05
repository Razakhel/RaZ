#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include <memory>
#include <string>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Image.hpp"

namespace Raz {

enum class TexturePreset : uint8_t {
  BLACK = 0,
  WHITE,

  PRESET_COUNT
};

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class Texture {
public:
  Texture();
  explicit Texture(uint8_t value) : Texture() { makePlainColored(Vec3b(value)); }
  Texture(unsigned int width, unsigned int height, ImageColorspace colorspace = ImageColorspace::RGB);
  explicit Texture(const std::string& fileName) : Texture() { load(fileName); }

  unsigned int getIndex() const { return m_index; }

  template <typename... Args>
  static TexturePtr create(Args&&... args) { return std::make_shared<Texture>(std::forward<Args>(args)...); }
  static TexturePtr recoverTexture(TexturePreset preset);
  static void activate(uint8_t index);

  void load(const std::string& fileName);
  void save(const std::string& fileName, bool reverse = false) const { m_image->save(fileName, reverse); }
  void bind() const;
  void unbind() const;

  ~Texture();

private:
  void makePlainColored(const Vec3b& color) const;

  unsigned int m_index {};
  ImagePtr m_image {};
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
