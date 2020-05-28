#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Image.hpp"

#include <memory>
#include <string>

namespace Raz {

enum class ColorPreset : uint32_t {
  BLACK   = 0x000000,
  RED     = 0xFF0000,
  GREEN   = 0x00FF00,
  BLUE    = 0x0000FF,
  YELLOW  = RED | GREEN,
  CYAN    = GREEN | BLUE,
  MAGENTA = RED | BLUE,
  WHITE   = RED | GREEN | BLUE
};
MAKE_ENUM_FLAG(ColorPreset)

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

/// Texture class, handling images to be displayed into the scene.
class Texture {
public:
  Texture();
  /// Constructs an 1x1 plain colored texture.
  /// \param value Color value to create the texture with.
  explicit Texture(const Vec3b& value) : Texture() { makePlainColored(value); }
  /// Constructs an 1x1 plain colored texture.
  /// \param preset Color preset to create the texture with.
  explicit Texture(ColorPreset preset);
  Texture(unsigned int width, unsigned int height, ImageColorspace colorspace = ImageColorspace::RGB, bool createMipmaps = true);
  explicit Texture(const std::string& fileName, bool flipVertically = false, bool createMipmaps = true)
    : Texture() { load(fileName, flipVertically, createMipmaps); }
  Texture(const Texture&) = delete;
  Texture(Texture&& texture) noexcept;

  unsigned int getIndex() const { return m_index; }
  const Image& getImage() const { return m_image; }

  template <typename... Args>
  static TexturePtr create(Args&&... args) { return std::make_shared<Texture>(std::forward<Args>(args)...); }

  /// Reads the texture in memory & loads it onto the graphics card.
  /// \param filePath Path to the texture to load.
  /// \param flipVertically Flip vertically the texture when loading.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(const std::string& filePath, bool flipVertically = false, bool createMipmaps = true);
  /// Saves the texture on disk.
  /// \param filePath Path to where to save the texture.
  /// \param flipVertically Flip vertically the texture when saving.
  void save(const std::string& filePath, bool flipVertically = false) const { m_image.save(filePath, flipVertically); }
  /// Binds the texture.
  void bind() const;
  /// Unbinds the texture.
  void unbind() const;

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&& texture) noexcept;

  ~Texture();

private:
  /// Fills the texture with a single pixel (creates a single-colored 1x1 texture).
  /// \note This only allocates & fills memory on the graphics card; the image member's data is left untouched.
  /// \param color Color to fill the texture with.
  void makePlainColored(const Vec3b& color) const;

  unsigned int m_index {};
  Image m_image {};
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
