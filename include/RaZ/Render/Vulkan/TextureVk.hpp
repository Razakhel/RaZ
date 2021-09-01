#pragma once

#ifndef RAZ_TEXTUREVK_HPP
#define RAZ_TEXTUREVK_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Image.hpp"

namespace Raz {

/// Texture class, handling images to be displayed into the scene.
class Texture {
public:
  Texture();
  explicit Texture(int bindingIndex) : Texture() { setBindingIndex(bindingIndex); }
  /// Constructs an 1x1 plain colored texture.
  /// \param value Color value to create the texture with.
  explicit Texture(const Vec3b& value, int bindingIndex = std::numeric_limits<int>::max()) : Texture(bindingIndex) { makePlainColored(value); }
  /// Constructs an 1x1 plain colored texture.
  /// \param preset Color preset to create the texture with.
  /// \param bindingIndex Index of the texture's binding point.
  explicit Texture(ColorPreset preset, int bindingIndex = std::numeric_limits<int>::max());
  Texture(unsigned int width, unsigned int height, int bindingIndex, ImageColorspace colorspace = ImageColorspace::RGB, bool createMipmaps = true);
  explicit Texture(Image image, int bindingIndex, bool createMipmaps = true) : Texture(bindingIndex) { load(std::move(image), createMipmaps); }
  explicit Texture(const FilePath& filePath, int bindingIndex, bool flipVertically = false, bool createMipmaps = true)
    : Texture(bindingIndex) { load(filePath, flipVertically, createMipmaps); }
  Texture(const Texture&) = delete;
  Texture(Texture&& texture) noexcept;

  int getBindingIndex() const { return m_bindingIndex; }
  const Image& getImage() const { return m_image; }

  void setBindingIndex(int bindingIndex) { m_bindingIndex = bindingIndex; }

  template <typename... Args>
  static TexturePtr create(Args&&... args) { return std::make_shared<Texture>(std::forward<Args>(args)...); }

  /// Sets the image & loads it onto the graphics card.
  /// \param image Image to be set as a texture.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(Image image, bool createMipmaps = true);
  /// Reads the texture in memory & loads it onto the graphics card.
  /// \param filePath Path to the texture to load.
  /// \param flipVertically Flip vertically the texture when loading.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(const FilePath& filePath, bool flipVertically = false, bool createMipmaps = true);
  /// Saves the texture on disk.
  /// \param filePath Path to where to save the texture.
  /// \param flipVertically Flip vertically the texture when saving.
  void save(const FilePath& filePath, bool flipVertically = false) const { m_image.save(filePath, flipVertically); }
  void activate() const {}
  void bind() const {}
  void unbind() const {}
  /// Resizes the texture.
  /// \warning This does NOT resize the contained image.
  /// \param width New texture width.
  /// \param height New texture height.
  void resize(unsigned int width, unsigned int height) const;

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&& texture) noexcept;

  ~Texture();

private:
  /// Loads it onto the graphics card.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(bool createMipmaps = true);
  /// Fills the texture with a single pixel (creates a single-colored 1x1 texture).
  /// \note This only allocates & fills memory on the graphics card; the image member's data is left untouched.
  /// \param color Color to fill the texture with.
  void makePlainColored(const Vec3b& color) const;

  int m_bindingIndex = std::numeric_limits<int>::max();
  Image m_image {};
};

} // namespace Raz

#endif // RAZ_TEXTUREVK_HPP
