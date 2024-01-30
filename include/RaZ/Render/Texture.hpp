#pragma once

#ifndef RAZ_TEXTURE_HPP
#define RAZ_TEXTURE_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <memory>
#include <vector>

namespace Raz {

class Color;
class Image;
using TexturePtr   = std::shared_ptr<class Texture>;
#if !defined(USE_OPENGL_ES)
using Texture1DPtr = std::shared_ptr<class Texture1D>;
#endif
using Texture2DPtr = std::shared_ptr<class Texture2D>;
using Texture3DPtr = std::shared_ptr<class Texture3D>;
enum class TextureType : unsigned int;

enum class TextureColorspace {
  INVALID = -1,
  GRAY = 0,
  RG,
  RGB,
  RGBA,
  SRGB,
  SRGBA,
  DEPTH
};

enum class TextureDataType {
  BYTE,
  FLOAT16,
  FLOAT32
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
  explicit Texture(TextureType type);

  /// Assigns default parameters after image loading. Must be called after having loaded the images' data in order to properly create the mipmaps.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void setLoadedParameters(bool createMipmaps) const;
  /// Generates mipmaps for the current texture.
  void generateMipmaps() const;
  virtual void load() const = 0;

  OwnerValue<unsigned int> m_index {};
  TextureType m_type {};

  TextureColorspace m_colorspace = TextureColorspace::INVALID;
  TextureDataType m_dataType {};
};

#if !defined(USE_OPENGL_ES)
class Texture1D final : public Texture {
public:
  Texture1D();
  explicit Texture1D(TextureColorspace colorspace) : Texture1D() { setColorspace(colorspace); }
  Texture1D(TextureColorspace colorspace, TextureDataType dataType) : Texture1D() { setColorspace(colorspace, dataType); }
  Texture1D(unsigned int width, TextureColorspace colorspace) : Texture1D(colorspace) { resize(width); }
  Texture1D(unsigned int width, TextureColorspace colorspace, TextureDataType dataType);
  /// Constructs a plain colored texture.
  /// \param color Color to fill the texture with.
  /// \param width Width of the texture to create.
  explicit Texture1D(const Color& color, unsigned int width = 1);

  unsigned int getWidth() const { return m_width; }

  template <typename... Args>
  static Texture1DPtr create(Args&&... args) { return std::make_shared<Texture1D>(std::forward<Args>(args)...); }

  /// Resizes the texture.
  /// \param width New texture width.
  void resize(unsigned int width);
  /// Fills the texture with a single color.
  /// \param color Color to fill the texture with.
  void fill(const Color& color);

private:
  void load() const override;

  unsigned int m_width = 0;
};
#endif

class Texture2D final : public Texture {
public:
  Texture2D();
  explicit Texture2D(TextureColorspace colorspace) : Texture2D() { setColorspace(colorspace); }
  Texture2D(TextureColorspace colorspace, TextureDataType dataType) : Texture2D() { setColorspace(colorspace, dataType); }
  Texture2D(unsigned int width, unsigned int height, TextureColorspace colorspace) : Texture2D(colorspace) { resize(width, height); }
  Texture2D(unsigned int width, unsigned int height, TextureColorspace colorspace, TextureDataType dataType);
  explicit Texture2D(const Image& image, bool createMipmaps = true) : Texture2D() { load(image, createMipmaps); }
  /// Constructs a plain colored texture.
  /// \param color Color to fill the texture with.
  /// \param width Width of the texture to create.
  /// \param height Height of the texture to create.
  explicit Texture2D(const Color& color, unsigned int width = 1, unsigned int height = 1);

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
  /// Fills the texture with a single color.
  /// \param color Color to fill the texture with.
  void fill(const Color& color);
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
  explicit Texture3D(const std::vector<Image>& imageSlices, bool createMipmaps = true) : Texture3D() { load(imageSlices, createMipmaps); }
  /// Constructs a plain colored texture.
  /// \param color Color to fill the texture with.
  /// \param width Width of the texture to create.
  /// \param height Height of the texture to create.
  /// \param depth Depth of the texture to create.
  explicit Texture3D(const Color& color, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1);

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
  /// Loads the images' data onto the graphics card.
  /// \param imageSlices Images along the depth to load the data from. All of them must have the same attributes (width, height, colorspace, ...)
  ///   and the number of images will become the texture's depth.
  /// \param createMipmaps True to generate texture mipmaps, false otherwise.
  void load(const std::vector<Image>& imageSlices, bool createMipmaps = true);
  /// Fills the texture with a single color.
  /// \param color Color to fill the texture with.
  void fill(const Color& color);

private:
  void load() const override;

  unsigned int m_width  = 0;
  unsigned int m_height = 0;
  unsigned int m_depth  = 0;
};

} // namespace Raz

#endif // RAZ_TEXTURE_HPP
