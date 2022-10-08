#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <limits>
#include <memory>

namespace Raz {

class Color;
class Image;
class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;

enum class TextureColorspace {
  INVALID = -1,
  GRAY = 0,
  GRAY_ALPHA,
  RGB,
  RGBA,
  SRGB,
  SRGBA,
  DEPTH
};

enum class TextureDataType {
  BYTE,
  FLOAT
};

enum class TextureFilter {
  NEAREST,
  LINEAR
};

enum class TextureWrapping {
  REPEAT,
  CLAMP
};

/// 2D texture class, representing images or buffers to be used in the rendering process.
class Texture2D {
public:
  Texture2D();
  explicit Texture2D(TextureColorspace colorspace) : Texture2D() { setColorspace(colorspace); }
  Texture2D(TextureColorspace colorspace, TextureDataType dataType) : Texture2D() { setColorspace(colorspace, dataType); }
  Texture2D(unsigned int width, unsigned int height, TextureColorspace colorspace) : Texture2D(colorspace) { resize(width, height); }
  Texture2D(unsigned int width, unsigned int height, TextureColorspace colorspace, TextureDataType dataType);
  explicit Texture2D(const Image& image, bool createMipmaps = true) : Texture2D() { load(image, createMipmaps); }
  /// Constructs an 1x1 plain colored texture.
  /// \param value Color to create the texture with.
  explicit Texture2D(const Color& color) : Texture2D() { makePlainColored(color); }
  Texture2D(const Texture2D&) = delete;
  Texture2D(Texture2D&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }
  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  TextureColorspace getColorspace() const { return m_colorspace; }
  TextureDataType getDataType() const { return m_dataType; }

  template <typename... Args>
  static Texture2DPtr create(Args&&... args) { return std::make_shared<Texture2D>(std::forward<Args>(args)...); }

  /// Binds the current texture.
  void bind() const;
  /// Unbinds the current texture.
  void unbind() const;
  void setFilter(TextureFilter filter) const { setFilter(filter, filter); }
  void setFilter(TextureFilter minify, TextureFilter magnify) const;
  void setFilter(TextureFilter minify, TextureFilter mipmapMinify, TextureFilter magnify) const;
  void setWrapping(TextureWrapping wrapping) const;
  /// Sets the texture's parameters; the data type will be deduced from the colorspace (float if depth, byte otherwise).
  /// \param width New texture width.
  /// \param height New texture height.
  /// \param colorspace New colorspace.
  /// \see resize(), setColorspace()
  void setParameters(unsigned int width, unsigned int height, TextureColorspace colorspace);
  /// Sets the texture's parameters.
  /// \param width New texture width.
  /// \param height New texture height.
  /// \param colorspace New colorspace.
  /// \param dataType New data type.
  /// \see resize(), setColorspace()
  void setParameters(unsigned int width, unsigned int height, TextureColorspace colorspace, TextureDataType dataType);
  /// Sets the texture's colorspace & data type; the latter will be deduced from the former (float if depth colorspace, byte otherwise).
  /// \param colorspace New colorspace.
  void setColorspace(TextureColorspace colorspace);
  /// Sets the texture's colorspace & data type.
  /// \param colorspace New colorspace.
  /// \param dataType New data type.
  void setColorspace(TextureColorspace colorspace, TextureDataType dataType);
  /// Resizes the texture.
  /// \param width New texture width.
  /// \param height New texture height.
  void resize(unsigned int width, unsigned int height);
  /// Loads the image's data onto the graphics card.
  /// \param image Image to load the data from.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(const Image& image, bool createMipmaps = true);
#if !defined(USE_OPENGL_ES)
  /// Retrieves the texture's data from the GPU.
  /// \warning The pixel storage pack & unpack alignments should be set to 1 in order to recover actual pixels.
  /// \see Renderer::setPixelStorage()
  /// \warning Retrieving an image from the GPU is slow; use this function with caution.
  /// \return Recovered image.
  Image recoverImage() const;
#endif

  Texture2D& operator=(const Texture2D&) = delete;
  Texture2D& operator=(Texture2D&&) noexcept = default;

  ~Texture2D();

private:
  /// Fills the texture with a single pixel (creates a single-colored 1x1 texture).
  /// \param color Color to fill the texture with.
  void makePlainColored(const Color& color);

  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};

  unsigned int m_width {};
  unsigned int m_height {};
  TextureColorspace m_colorspace = TextureColorspace::INVALID;
  TextureDataType m_dataType {};
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
