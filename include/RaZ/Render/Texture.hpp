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

/// Texture class, handling images to be displayed into the scene.
class Texture {
public:
  Texture();
  explicit Texture(uint8_t value) : Texture() { makePlainColored(Vec3b(value)); }
  Texture(unsigned int width, unsigned int height, ImageColorspace colorspace = ImageColorspace::RGB);
  explicit Texture(const std::string& fileName) : Texture() { load(fileName); }
  Texture(const Texture&) = delete;
  Texture(Texture&& texture) noexcept;

  unsigned int getIndex() const { return m_index; }
  const ImagePtr& getImage() const { return m_image; }

  template <typename... Args>
  static TexturePtr create(Args&&... args) { return std::make_shared<Texture>(std::forward<Args>(args)...); }

  /// Gets a texture based on a given preset.
  /// \param preset Preset of the texture to get.
  /// \return Recovered texture.
  static TexturePtr recoverTexture(TexturePreset preset);
  /// Activates the texture at the given index.
  /// \param index Index of the active texture to set.
  static void activate(uint8_t index);
  /// Reads the texture in memory & loads it onto the graphics card.
  /// \param filePath Path to the texture to load.
  void load(const std::string& filePath);
  /// Saves the texture on disk.
  /// \param filePath Path to where to save the texture.
  /// \param reverse Reverse the texture when saving.
  void save(const std::string& filePath, bool reverse = false) const { m_image->save(filePath, reverse); }
  /// Binds the texture.
  void bind() const;
  /// Unbinds the texture.
  void unbind() const;

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&& texture) noexcept;

  ~Texture();

private:
  /// Fills the texture with a single pixel (creates a single-colored 1x1 texture).
  /// \param color Color to fill the texture with.
  void makePlainColored(const Vec3b& color) const;

  unsigned int m_index {};
  ImagePtr m_image {};
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
