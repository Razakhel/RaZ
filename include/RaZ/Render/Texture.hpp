#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <limits>
#include <memory>

namespace Raz {

class Color;
class Image;
using TexturePtr   = std::shared_ptr<class Texture>;
using Texture2DPtr = std::shared_ptr<class Texture2D>;
using Texture3DPtr = std::shared_ptr<class Texture3D>;
enum class TextureType : unsigned int;

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

/// Texture class, representing images or buffers to be used in the rendering process.
class Texture {
public:
  Texture(const Texture&) = delete;
  Texture(Texture&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }
  TextureColorspace getColorspace() const { return m_colorspace; }
  TextureDataType getDataType() const { return m_dataType; }

  /// Binds the current texture.
  void bind() const;
  /// Unbinds the current texture.
  void unbind() const;
  void setFilter(TextureFilter filter) const { setFilter(filter, filter); }
  void setFilter(TextureFilter minify, TextureFilter magnify) const;
  void setFilter(TextureFilter minify, TextureFilter mipmapMinify, TextureFilter magnify) const;
  void setWrapping(TextureWrapping wrapping) const;
  /// Sets the texture's colorspace & data type; the latter will be deduced from the former (float if depth colorspace, byte otherwise).
  /// \param colorspace New colorspace.
  void setColorspace(TextureColorspace colorspace);
  /// Sets the texture's colorspace & data type.
  /// \param colorspace New colorspace.
  /// \param dataType New data type.
  void setColorspace(TextureColorspace colorspace, TextureDataType dataType);

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&&) noexcept = default;

  virtual ~Texture();

protected:
  Texture(TextureType type);

  virtual void load() const = 0;

  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
  TextureType m_type {};

  TextureColorspace m_colorspace = TextureColorspace::INVALID;
  TextureDataType m_dataType {};
};

class Texture2D final : public Texture {
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

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }

  template <typename... Args>
  static Texture2DPtr create(Args&&... args) { return std::make_shared<Texture2D>(std::forward<Args>(args)...); }

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

private:
  void load() const override;
  /// Fills the texture with a single pixel (creates a single-colored 1x1 texture).
  /// \param color Color to fill the texture with.
  void makePlainColored(const Color& color);

  unsigned int m_width  = 0;
  unsigned int m_height = 0;
};

class Texture3D final : public Texture {
public:
  Texture3D();
  explicit Texture3D(TextureColorspace colorspace) : Texture3D() { setColorspace(colorspace); }
  Texture3D(TextureColorspace colorspace, TextureDataType dataType) : Texture3D() { setColorspace(colorspace, dataType); }
  Texture3D(unsigned int width, unsigned int height, unsigned int depth, TextureColorspace colorspace) : Texture3D(colorspace) { resize(width, height, depth); }
  Texture3D(unsigned int width, unsigned int height, unsigned int depth, TextureColorspace colorspace, TextureDataType dataType);

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  unsigned int getDepth() const { return m_depth; }

  template <typename... Args>
  static Texture3DPtr create(Args&&... args) { return std::make_shared<Texture3D>(std::forward<Args>(args)...); }

  /// Resizes the texture.
  /// \param width New texture width.
  /// \param height New texture height.
  /// \param depth New texture depth.
  void resize(unsigned int width, unsigned int height, unsigned int depth);

private:
  void load() const override;

  unsigned int m_width  = 0;
  unsigned int m_height = 0;
  unsigned int m_depth  = 0;
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
