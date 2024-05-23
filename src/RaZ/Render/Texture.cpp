#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Image.hpp"
#if defined(USE_OPENGL_ES)
#include "RaZ/Render/Framebuffer.hpp"
#endif
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

namespace {

inline TextureFormat recoverFormat(TextureColorspace colorspace) {
  switch (colorspace) {
    case TextureColorspace::INVALID:
      break;

    case TextureColorspace::GRAY:
      return TextureFormat::RED;

    case TextureColorspace::RG:
      return TextureFormat::RG;

    case TextureColorspace::RGB:
    case TextureColorspace::SRGB:
      return TextureFormat::RGB;

    case TextureColorspace::RGBA:
    case TextureColorspace::SRGBA:
      return TextureFormat::RGBA;

    case TextureColorspace::DEPTH:
      return TextureFormat::DEPTH;
  }

  throw std::invalid_argument("Error: Invalid texture colorspace");
}

inline TextureInternalFormat recoverInternalFormat(TextureColorspace colorspace, TextureDataType dataType) {
  // If the texture is of a byte data type and not an sRGB colorspace, its internal format is the same as its format
  if (dataType == TextureDataType::BYTE && colorspace != TextureColorspace::SRGB && colorspace != TextureColorspace::SRGBA)
    return static_cast<TextureInternalFormat>(recoverFormat(colorspace));

  switch (colorspace) {
    case TextureColorspace::INVALID:
      break;

    case TextureColorspace::GRAY:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::R16F : TextureInternalFormat::R32F);

    case TextureColorspace::RG:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::RG16F : TextureInternalFormat::RG32F);

    case TextureColorspace::RGB:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::RGB16F : TextureInternalFormat::RGB32F);

    case TextureColorspace::RGBA:
      return (dataType == TextureDataType::FLOAT16 ? TextureInternalFormat::RGBA16F : TextureInternalFormat::RGBA32F);

    case TextureColorspace::SRGB:
      return TextureInternalFormat::SRGB8;

    case TextureColorspace::SRGBA:
      return TextureInternalFormat::SRGBA8;

    case TextureColorspace::DEPTH:
      return TextureInternalFormat::DEPTH32F;
  }

  throw std::invalid_argument("Error: Invalid texture colorspace");
}

inline TextureParamValue recoverParam(TextureFilter filter) {
  switch (filter) {
    case TextureFilter::NEAREST:
      return TextureParamValue::NEAREST;

    case TextureFilter::LINEAR:
      return TextureParamValue::LINEAR;
  }

  throw std::invalid_argument("Error: Invalid texture filter");
}

inline TextureParamValue recoverParam(TextureFilter filter, TextureFilter mipmapFilter) {
  switch (filter) {
    case TextureFilter::NEAREST:
      return (mipmapFilter == TextureFilter::NEAREST ? TextureParamValue::NEAREST_MIPMAP_NEAREST : TextureParamValue::NEAREST_MIPMAP_LINEAR);

    case TextureFilter::LINEAR:
      return (mipmapFilter == TextureFilter::NEAREST ? TextureParamValue::LINEAR_MIPMAP_NEAREST : TextureParamValue::LINEAR_MIPMAP_LINEAR);
  }

  throw std::invalid_argument("Error: Invalid texture filter");
}

inline TextureParamValue recoverParam(TextureWrapping wrapping) {
  switch (wrapping) {
    case TextureWrapping::REPEAT:
      return TextureParamValue::REPEAT;

    case TextureWrapping::CLAMP:
      return TextureParamValue::CLAMP_TO_EDGE;
  }

  throw std::invalid_argument("Error: Invalid texture wrapping");
}

inline TextureColorspace recoverColorspace(ImageColorspace imgColorspace, bool shouldUseSrgb) {
  auto texColorspace = static_cast<TextureColorspace>(imgColorspace);

  if (shouldUseSrgb) {
    if (texColorspace == TextureColorspace::RGB)
      texColorspace = TextureColorspace::SRGB;
    else if (texColorspace == TextureColorspace::RGBA)
      texColorspace = TextureColorspace::SRGBA;
  }

  return texColorspace;
}

} // namespace

void Texture::bind() const {
  Renderer::bindTexture(m_type, m_index);
}

void Texture::unbind() const {
  Renderer::unbindTexture(m_type);
}

void Texture::setFilter(TextureFilter minify, TextureFilter magnify) const {
  ZoneScopedN("Texture::setFilter");

  bind();
  Renderer::setTextureParameter(m_type, TextureParam::MINIFY_FILTER, recoverParam(minify));
  Renderer::setTextureParameter(m_type, TextureParam::MAGNIFY_FILTER, recoverParam(magnify));
  unbind();
}

void Texture::setFilter(TextureFilter minify, TextureFilter mipmapMinify, TextureFilter magnify) const {
  ZoneScopedN("Texture::setFilter");

  bind();
  Renderer::setTextureParameter(m_type, TextureParam::MINIFY_FILTER, recoverParam(minify, mipmapMinify));
  Renderer::setTextureParameter(m_type, TextureParam::MAGNIFY_FILTER, recoverParam(magnify));
  unbind();
}

void Texture::setWrapping(TextureWrapping wrapping) const {
  ZoneScopedN("Texture::setWrapping");

  const TextureParamValue value = recoverParam(wrapping);

  bind();
  Renderer::setTextureParameter(m_type, TextureParam::WRAP_S, value);
  Renderer::setTextureParameter(m_type, TextureParam::WRAP_T, value);
  Renderer::setTextureParameter(m_type, TextureParam::WRAP_R, value);
  unbind();
}

void Texture::setColorspace(TextureColorspace colorspace) {
  setColorspace(colorspace, (colorspace == TextureColorspace::DEPTH ? TextureDataType::FLOAT32 : TextureDataType::BYTE));
}

void Texture::setColorspace(TextureColorspace colorspace, TextureDataType dataType) {
  assert("Error: A depth texture must have a 32-bit floating-point data type."
      && (colorspace != TextureColorspace::DEPTH || dataType == TextureDataType::FLOAT32));
  assert("Error: A depth texture cannot be three-dimensional." && (colorspace != TextureColorspace::DEPTH || m_type != TextureType::TEXTURE_3D));

  ZoneScopedN("Texture::setColorspace");

  m_colorspace = colorspace;
  m_dataType   = dataType;

  load();

  if (m_colorspace == TextureColorspace::DEPTH)
    setFilter(TextureFilter::NEAREST);
}

Texture::~Texture() {
  ZoneScopedN("Texture::~Texture");

  if (!m_index.isValid())
    return;

  Logger::debug("[Texture] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteTexture(m_index);
  Logger::debug("[Texture] Destroyed");
}

Texture::Texture(TextureType type) : m_type{ type } {
  ZoneScopedN("Texture::Texture");

  Logger::debug("[Texture] Creating...");
  Renderer::generateTexture(m_index);
  Logger::debug("[Texture] Created (ID: " + std::to_string(m_index) + ')');

  setFilter(TextureFilter::LINEAR);
  setWrapping(TextureWrapping::CLAMP);
}

void Texture::setLoadedParameters(bool createMipmaps) const {
  ZoneScopedN("Texture::setLoadedParameters");

  if (m_colorspace == TextureColorspace::GRAY || m_colorspace == TextureColorspace::RG) {
    Renderer::setTextureParameter(m_type, TextureParam::SWIZZLE_R, static_cast<int>(TextureFormat::RED));
    Renderer::setTextureParameter(m_type, TextureParam::SWIZZLE_G, static_cast<int>(TextureFormat::RED));
    Renderer::setTextureParameter(m_type, TextureParam::SWIZZLE_B, static_cast<int>(TextureFormat::RED));
    Renderer::setTextureParameter(m_type, TextureParam::SWIZZLE_A, (m_colorspace == TextureColorspace::RG ? static_cast<int>(TextureFormat::GREEN) : 1));
  }

  if (createMipmaps) {
    generateMipmaps();
    setFilter(TextureFilter::LINEAR, TextureFilter::LINEAR, TextureFilter::LINEAR);
  } else {
    setFilter(TextureFilter::LINEAR);
  }

  setWrapping(TextureWrapping::REPEAT);
}

void Texture::generateMipmaps() const {
  ZoneScopedN("Texture::generateMipmaps");

  bind();
  Renderer::generateMipmap(m_type);
  unbind();
}

#if !defined(USE_OPENGL_ES)
Texture1D::Texture1D()
  : Texture(TextureType::TEXTURE_1D) {}

Texture1D::Texture1D(unsigned int width, TextureColorspace colorspace, TextureDataType dataType)
  : Texture1D(colorspace, dataType) { resize(width); }

Texture1D::Texture1D(const Color& color, unsigned int width) : Texture1D() {
  m_width = width;

  fill(color);
}

void Texture1D::resize(unsigned int width) {
  m_width = width;

  load();
}

void Texture1D::fill(const Color& color) {
  ZoneScopedN("Texture1D::fill");

  m_colorspace = TextureColorspace::RGB;
  m_dataType   = TextureDataType::BYTE;

  const std::vector<Vec3b> values(m_width * 3, Vec3b(color));

  bind();
  Renderer::sendImageData1D(TextureType::TEXTURE_1D,
                            0,
                            TextureInternalFormat::RGB,
                            m_width,
                            TextureFormat::RGB,
                            PixelDataType::UBYTE,
                            values.data());
  unbind();
}

void Texture1D::load() const {
  ZoneScopedN("Texture1D::load");

  if (m_colorspace == TextureColorspace::INVALID)
    return; // No colorspace has been set yet, the texture can't be loaded

  bind();
  Renderer::sendImageData1D(TextureType::TEXTURE_1D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            nullptr);
  unbind();
}
#endif

Texture2D::Texture2D()
  : Texture(TextureType::TEXTURE_2D) {}

Texture2D::Texture2D(unsigned int width, unsigned int height, TextureColorspace colorspace, TextureDataType dataType)
  : Texture2D(colorspace, dataType) { resize(width, height); }

Texture2D::Texture2D(const Color& color, unsigned int width, unsigned int height) : Texture2D() {
  m_width  = width;
  m_height = height;

  fill(color);
}

void Texture2D::resize(unsigned int width, unsigned int height) {
  m_width  = width;
  m_height = height;

  load();
}

void Texture2D::load(const Image& image, bool createMipmaps, bool shouldUseSrgb) {
  ZoneScopedN("Texture2D::load(Image)");

  if (image.isEmpty()) {
    // Image not found, defaulting texture to pure white
    fill(ColorPreset::White);
    return;
  }

  m_width      = image.getWidth();
  m_height     = image.getHeight();
  m_colorspace = recoverColorspace(image.getColorspace(), shouldUseSrgb);
  m_dataType   = (image.getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT16 : TextureDataType::BYTE);

#if defined(USE_OPENGL_ES)
  if ((m_width & (m_width - 1)) != 0 || (m_height & (m_height - 1)) != 0) {
    Logger::warn("[Texture] The given image's dimensions (" + std::to_string(m_width) + 'x' + std::to_string(m_height)
               + ") are not powers of two; this can give unexpected results.");
  }
#endif

  int unpackAlignment = 4;

  if (image.getChannelCount() == 1) {
    Renderer::getParameter(StateParameter::UNPACK_ALIGNMENT, &unpackAlignment);
    Renderer::setPixelStorage(PixelStorage::UNPACK_ALIGNMENT, 1);
  }

  bind();

  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            m_height,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            image.getDataPtr());

  if (image.getChannelCount() == 1)
    Renderer::setPixelStorage(PixelStorage::UNPACK_ALIGNMENT, unpackAlignment);

  setLoadedParameters(createMipmaps);
}

void Texture2D::fill(const Color& color) {
  ZoneScopedN("Texture2D::fill");

  m_colorspace = TextureColorspace::RGB;
  m_dataType   = TextureDataType::BYTE;

  const std::vector<Vec3b> values(m_width * m_height * 3, Vec3b(color));

  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            TextureInternalFormat::RGB,
                            m_width,
                            m_height,
                            TextureFormat::RGB,
                            PixelDataType::UBYTE,
                            values.data());
  unbind();
}

Image Texture2D::recoverImage() const {
  ZoneScopedN("Texture2D::recoverImage");

  Image img(m_width, m_height, static_cast<ImageColorspace>(m_colorspace), (m_dataType == TextureDataType::BYTE ? ImageDataType::BYTE : ImageDataType::FLOAT));

  const PixelDataType pixelDataType = (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT);

#if !defined(USE_OPENGL_ES)
  bind();
  Renderer::recoverTextureData(TextureType::TEXTURE_2D, 0, recoverFormat(m_colorspace), pixelDataType, img.getDataPtr());
  unbind();
#else
  // Recovering an image directly from a texture (glGetTexImage()) is not possible with OpenGL ES; a framebuffer must be used to read the texture from instead
  // See: https://stackoverflow.com/a/53993894/3292304

  const Framebuffer dummyFramebuffer;
  Renderer::bindFramebuffer(dummyFramebuffer.getIndex(), FramebufferType::READ_FRAMEBUFFER);

  Renderer::setFramebufferTexture2D(FramebufferAttachment::COLOR0, m_index, 0, TextureType::TEXTURE_2D, FramebufferType::READ_FRAMEBUFFER);
  Renderer::recoverFrame(m_width, m_height, recoverFormat(m_colorspace), pixelDataType, img.getDataPtr());

  Renderer::unbindFramebuffer(FramebufferType::READ_FRAMEBUFFER);
#endif

  return img;
}

void Texture2D::load() const {
  ZoneScopedN("Texture2D::load");

  if (m_colorspace == TextureColorspace::INVALID)
    return; // No colorspace has been set yet, the texture can't be loaded

  bind();
  Renderer::sendImageData2D(TextureType::TEXTURE_2D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            m_height,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            nullptr);
  unbind();
}

Texture3D::Texture3D()
  : Texture(TextureType::TEXTURE_3D) {}

Texture3D::Texture3D(unsigned int width, unsigned int height, unsigned int depth, TextureColorspace colorspace, TextureDataType dataType)
  : Texture3D(colorspace, dataType) { resize(width, height, depth); }

Texture3D::Texture3D(const Color& color, unsigned int width, unsigned int height, unsigned int depth) : Texture3D() {
  m_width  = width;
  m_height = height;
  m_depth  = depth;

  fill(color);
}

void Texture3D::resize(unsigned int width, unsigned int height, unsigned int depth) {
  m_width  = width;
  m_height = height;
  m_depth  = depth;

  load();
}

void Texture3D::load(const std::vector<Image>& imageSlices, bool createMipmaps, bool shouldUseSrgb) {
  ZoneScopedN("Texture3D::load(std::vector<Image>)");

  if (imageSlices.empty() || imageSlices.front().isEmpty()) {
    // Images not found, defaulting texture to pure white
    fill(ColorPreset::White);
    return;
  }

  const ImageColorspace firstImgColorspace = imageSlices.front().getColorspace();
  const ImageDataType firstImgDataType     = imageSlices.front().getDataType();

  m_width      = imageSlices.front().getWidth();
  m_height     = imageSlices.front().getHeight();
  m_depth      = static_cast<unsigned int>(imageSlices.size());
  m_colorspace = recoverColorspace(firstImgColorspace, shouldUseSrgb);
  m_dataType   = (firstImgDataType == ImageDataType::FLOAT ? TextureDataType::FLOAT16 : TextureDataType::BYTE);

#if defined(USE_OPENGL_ES)
  if ((m_width & (m_width - 1)) != 0 || (m_height & (m_height - 1)) != 0 || (m_depth & (m_depth - 1)) != 0) {
    Logger::warn("[Texture] The given image's dimensions (" + std::to_string(m_width) + 'x' + std::to_string(m_height) + 'x' + std::to_string(m_depth)
               + ") are not powers of two; this can give unexpected results.");
  }
#endif

  load();

  const TextureFormat textureFormat = recoverFormat(m_colorspace);
  const PixelDataType pixelDataType = (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT);

  bind();

  for (std::size_t imgIndex = 0; imgIndex < imageSlices.size(); ++imgIndex) {
    const Image& img = imageSlices[imgIndex];

    if (img.getWidth() != m_width || img.getHeight() != m_height || img.getColorspace() != firstImgColorspace || img.getDataType() != firstImgDataType)
      throw std::invalid_argument("[Texture3D] The given images have different attributes.");

    Renderer::sendImageSubData3D(TextureType::TEXTURE_3D,
                                 0,
                                 0,
                                 0,
                                 static_cast<unsigned int>(imgIndex),
                                 m_width,
                                 m_height,
                                 1,
                                 textureFormat,
                                 pixelDataType,
                                 img.getDataPtr());
  }

  setLoadedParameters(createMipmaps);
}

void Texture3D::fill(const Color& color) {
  ZoneScopedN("Texture3D::fill");

  m_colorspace = TextureColorspace::RGB;
  m_dataType   = TextureDataType::BYTE;

  const std::vector<Vec3b> values(m_width * m_height * m_depth * 3, Vec3b(color));

  bind();
  Renderer::sendImageData3D(TextureType::TEXTURE_3D,
                            0,
                            TextureInternalFormat::RGB,
                            m_width,
                            m_height,
                            m_depth,
                            TextureFormat::RGB,
                            PixelDataType::UBYTE,
                            values.data());
  unbind();
}

void Texture3D::load() const {
  ZoneScopedN("Texture3D::load");

  if (m_colorspace == TextureColorspace::INVALID)
    return; // No colorspace has been set yet, the texture can't be loaded

  bind();
  Renderer::sendImageData3D(TextureType::TEXTURE_3D,
                            0,
                            recoverInternalFormat(m_colorspace, m_dataType),
                            m_width,
                            m_height,
                            m_depth,
                            recoverFormat(m_colorspace),
                            (m_dataType == TextureDataType::BYTE ? PixelDataType::UBYTE : PixelDataType::FLOAT),
                            nullptr);
  unbind();
}

} // namespace Raz
