#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Math/Vector.hpp"

#include <memory>

namespace Raz {

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

/// Texture class, handling images to be displayed into the scene.
class Texture {
public:
  Texture();
  /// Constructs an 1x1 plain colored texture.
  /// \param value Color to create the texture with.
  explicit Texture(const Color& color) : Texture() { makePlainColored(color); }
  Texture(unsigned int width, unsigned int height, ImageColorspace colorspace);
  Texture(unsigned int width, unsigned int height, ImageColorspace colorspace, ImageDataType dataType);
  explicit Texture(Image image, bool createMipmaps = true) : Texture() { load(std::move(image), createMipmaps); }
  Texture(const Texture&) = delete;
  Texture(Texture&& texture) noexcept;

  unsigned int getIndex() const { return m_index; }
  const Image& getImage() const { return m_image; }

  template <typename... Args>
  static TexturePtr create(Args&&... args) { return std::make_shared<Texture>(std::forward<Args>(args)...); }

  /// Sets the image & loads it onto the graphics card.
  /// \param image Image to be set as a texture.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(Image&& image, bool createMipmaps = true);
  /// Loads the image's data onto the graphics card.
  /// \param image Image to load the data from.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(const Image& image, bool createMipmaps = true);
  /// Binds the current texture.
  void bind() const;
  /// Unbinds the current texture.
  void unbind() const;
  /// Resizes the texture.
  /// \warning This does NOT resize the contained image.
  /// \param width New texture width.
  /// \param height New texture height.
  void resize(unsigned int width, unsigned int height) const;

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&& texture) noexcept;

  ~Texture();

private:
  /// Fills the texture with a single pixel (creates a single-colored 1x1 texture).
  /// \note This only allocates & fills memory on the graphics card; the image member's data is left untouched.
  /// \param color Color to fill the texture with.
  void makePlainColored(const Color& color) const;

  unsigned int m_index = std::numeric_limits<unsigned int>::max();
  Image m_image {};
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
