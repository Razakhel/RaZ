#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include "RaZ/Render/Renderer.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Raz {

struct ImageData;
using ImageDataPtr = std::unique_ptr<ImageData>;

struct ImageDataB;
using ImageDataBPtr = std::unique_ptr<ImageDataB>;

struct ImageDataF;
using ImageDataFPtr = std::unique_ptr<ImageDataF>;

class Image;
using ImagePtr = std::unique_ptr<Image>;

enum class ImageDataType : uint8_t {
  BYTE = 0,
  FLOAT
};

/// ImageData class, representing data held by an Image.
struct ImageData {
  virtual ImageDataType getDataType() const = 0;
  virtual const void* getDataPtr() const = 0;
  virtual void* getDataPtr() = 0;

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  virtual bool isEmpty() const = 0;

  /// Checks if the current image data is equal to another given one.
  /// Datas' types must be the same.
  /// \param imgData Image data to be compared with.
  /// \return True if datas are equal, false otherwise.
  virtual bool operator==(const ImageData& imgData) const = 0;
  /// Checks if the current image data is different from another given one.
  /// Datas' types must be the same.
  /// \param imgData Image data to be compared with.
  /// \return True if datas are different, false otherwise.
  bool operator!=(const ImageData& imgData) const { return !(*this == imgData); }

  virtual ~ImageData() = default;
};

/// ImageData in bytes.
struct ImageDataB : public ImageData {
  ImageDataType getDataType() const override { return ImageDataType::BYTE; }
  const void* getDataPtr() const override { return data.data(); }
  void* getDataPtr() override { return data.data(); }

  template <typename... Args>
  static ImageDataBPtr create(Args&&... args) { return std::make_unique<ImageDataB>(std::forward<Args>(args)...); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const override { return data.empty(); }

  /// Checks if the current byte image data is equal to another given one.
  /// The given data must be of byte type as well.
  /// \param imgData Image data to be compared with.
  /// \return True if datas are equal, false otherwise.
  bool operator==(const ImageData& imgData) const override;

  std::vector<uint8_t> data;
};

/// ImageData in floating point values (for High Dynamic Range (HDR) images).
struct ImageDataF : public ImageData {
  ImageDataType getDataType() const override { return ImageDataType::FLOAT; }
  const void* getDataPtr() const override { return data.data(); }
  void* getDataPtr() override { return data.data(); }

  template <typename... Args>
  static ImageDataFPtr create(Args&&... args) { return std::make_unique<ImageDataF>(std::forward<Args>(args)...); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const override { return data.empty(); }

  /// Checks if the current floating point image data is equal to another given one.
  /// The given data must be of floating point type as well.
  /// \param imgData Image data to be compared with.
  /// \return True if datas are equal, false otherwise.
  bool operator==(const ImageData& imgData) const override;

  std::vector<float> data;
};

enum class ImageColorspace : unsigned int {
  GRAY       = static_cast<unsigned int>(TextureFormat::RED),
  GRAY_ALPHA = static_cast<unsigned int>(TextureFormat::RG),
  RGB        = static_cast<unsigned int>(TextureFormat::RGB),
  RGBA       = static_cast<unsigned int>(TextureFormat::RGBA),
  DEPTH      = static_cast<unsigned int>(TextureFormat::DEPTH)
};

/// Image class, handling images of different formats.
class Image {
  friend class Texture;

public:
  Image() = default;
  Image(unsigned int width, unsigned int height, ImageColorspace colorspace = ImageColorspace::RGB);
  explicit Image(const std::string& fileName, bool flipVertically = false) { read(fileName, flipVertically); }

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  ImageColorspace getColorspace() const { return m_colorspace; }
  ImageDataType getDataType() const { return m_data->getDataType(); }
  const void* getDataPtr() const { return m_data->getDataPtr(); }
  void* getDataPtr() { return m_data->getDataPtr(); }

  template <typename... Args> static ImagePtr create(Args&&... args) { return std::make_unique<Image>(std::forward<Args>(args)...); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const { return (!m_data || m_data->isEmpty()); }
  /// Reads the image to memory.
  /// \param filePath Path to the image to read.
  /// \param flipVertically Flip vertically the image when reading.
  void read(const std::string& filePath, bool flipVertically = false);
  /// Saves the image on disk.
  /// \param filePath Path to where to save the image.
  /// \param flipVertically Flip vertically the image when saving.
  void save(const std::string& filePath, bool flipVertically = false) const;

  /// Checks if the current image is equal to another given one.
  /// Their inner datas must be of the same type.
  /// \param img Image to be compared with.
  /// \return True if images are the same, false otherwise.
  bool operator==(const Image& img) const;
  /// Checks if the current image is different from another given one.
  /// Their inner datas must be of the same type.
  /// \param img Image to be compared with.
  /// \return True if images are different, false otherwise.
  bool operator!=(const Image& img) const { return !(*this == img); }

private:
  /// Reads a PNG image to memory.
  /// \param file File to read.
  /// \param flipVertically Flip vertically the image when reading.
  void readPng(std::ifstream& file, bool flipVertically);
  /// Saves the image on disk in PNG format.
  /// \param file File to save.
  /// \param flipVertically Flip vertically the image when saving.
  void savePng(std::ofstream& file, bool flipVertically) const;
  /// Reads a TGA image to memory.
  /// \param file File to read.
  /// \param flipVertically Flip vertically the image when reading.
  void readTga(std::ifstream& file, bool flipVertically);
  /*
  /// Saves the image on disk in TGA format.
  /// \param file File to save.
  /// \param flipVertically Flip vertically the image when saving.
  void saveTga(std::ofstream& file, bool flipVertically) const;
  */

  unsigned int m_width {};
  unsigned int m_height {};
  ImageColorspace m_colorspace {};
  uint8_t m_channelCount {};
  uint8_t m_bitDepth {};

  ImageDataPtr m_data {};
};

} // namespace Raz

#endif // RAZ_IMAGE_HPP
