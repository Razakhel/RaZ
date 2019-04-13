#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

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

  std::vector<float> data;
};

enum class ImageColorspace {
  GRAY       = 6403,  // GL_RED
  GRAY_ALPHA = 33319, // GL_RG
  RGB        = 6407,  // GL_RGB
  RGBA       = 6408,  // GL_RGBA
  DEPTH      = 6402   // GL_DEPTH_COMPONENT
};

/// Image class, handling images of different formats.
class Image {
public:
  Image() = default;
  explicit Image(const std::string& fileName, bool reverse = false) { read(fileName, reverse); }

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  ImageColorspace getColorspace() const { return m_colorspace; }
  ImageDataType getDataType() const { return m_data->getDataType(); }
  const void* getDataPtr() const { return m_data->getDataPtr(); }

  template <typename... Args> static ImagePtr create(Args&&... args) { return std::make_unique<Image>(std::forward<Args>(args)...); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const { return (!m_data || m_data->isEmpty()); }
  /// Reads the image to memory.
  /// \param filePath Path to the image to read.
  /// \param reverse Reverse the image when reading.
  void read(const std::string& filePath, bool reverse = false);
  /// Saves the image on disk.
  /// \param filePath Path to where to save the image.
  /// \param reverse Reverse the image when saving.
  void save(const std::string& filePath, bool reverse = false) const;

private:
  /// Reads a PNG image to memory.
  /// \param file File to read.
  /// \param reverse Reverse the image when reading.
  void readPng(std::ifstream& file, bool reverse);
  /// Saves the image on disk in PNG.
  /// \param file File to save.
  /// \param reverse Reverse the image when saving.
  void savePng(std::ofstream& file, bool reverse) const;
  /// Reads a TGA image to memory.
  /// \param file File to read.
  void readTga(std::ifstream& file);
  /*
  /// Saves the image on disk in TGA.
  /// \param file File to save.
  void saveTga(std::ofstream& file) const;
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
