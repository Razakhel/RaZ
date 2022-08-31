#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include <memory>
#include <vector>

namespace Raz {

class FilePath;

struct ImageData;
using ImageDataPtr = std::unique_ptr<ImageData>;

struct ImageDataB;
using ImageDataBPtr = std::unique_ptr<ImageDataB>;

struct ImageDataF;
using ImageDataFPtr = std::unique_ptr<ImageDataF>;

class Image;
using ImagePtr = std::unique_ptr<Image>;

enum class ImageColorspace {
  GRAY = 0,
  GRAY_ALPHA,
  RGB,
  RGBA,
  SRGB,
  SRGBA
};

enum class ImageDataType {
  BYTE,
  FLOAT
};

/// ImageData class, representing data held by an Image.
struct ImageData {
  ImageData() = default;
  ImageData(const ImageData&) = default;
  ImageData(ImageData&&) noexcept = default;

  virtual ImageDataType getDataType() const = 0;
  virtual const void* getDataPtr() const = 0;
  virtual void* getDataPtr() = 0;

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  virtual bool isEmpty() const = 0;
  /// Resizes the data container.
  /// \param size New data size.
  virtual void resize(std::size_t size) = 0;

  ImageData& operator=(const ImageData&) = default;
  ImageData& operator=(ImageData&&) noexcept = default;
  /// Checks if the current image data is equal to another given one.
  /// Datas' types must be the same.
  /// \param imgData Image data to be compared with.
  /// \return True if data are equal, false otherwise.
  virtual bool operator==(const ImageData& imgData) const = 0;
  /// Checks if the current image data is different from another given one.
  /// Datas' types must be the same.
  /// \param imgData Image data to be compared with.
  /// \return True if data are different, false otherwise.
  bool operator!=(const ImageData& imgData) const { return !(*this == imgData); }

  virtual ~ImageData() = default;
};

/// ImageData in bytes.
struct ImageDataB final : public ImageData {
  explicit ImageDataB(std::size_t dataSize) { resize(dataSize); }

  ImageDataType getDataType() const override { return ImageDataType::BYTE; }
  const void* getDataPtr() const override { return data.data(); }
  void* getDataPtr() override { return data.data(); }

  template <typename... Args>
  static ImageDataBPtr create(Args&&... args) { return std::make_unique<ImageDataB>(std::forward<Args>(args)...); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const override { return data.empty(); }
  /// Resizes the data container.
  /// \param size New data size.
  void resize(std::size_t size) override { data.resize(size); }

  /// Checks if the current byte image data is equal to another given one.
  /// The given data must be of byte type as well.
  /// \param imgData Image data to be compared with.
  /// \return True if data are equal, false otherwise.
  bool operator==(const ImageData& imgData) const override;

  std::vector<uint8_t> data;
};

/// ImageData in floating point values (for High Dynamic Range (HDR) images).
struct ImageDataF final : public ImageData {
  explicit ImageDataF(std::size_t dataSize) { resize(dataSize); }

  ImageDataType getDataType() const override { return ImageDataType::FLOAT; }
  const void* getDataPtr() const override { return data.data(); }
  void* getDataPtr() override { return data.data(); }

  template <typename... Args>
  static ImageDataFPtr create(Args&&... args) { return std::make_unique<ImageDataF>(std::forward<Args>(args)...); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const override { return data.empty(); }
  /// Resizes the data container.
  /// \param size New data size.
  void resize(std::size_t size) override { data.resize(size); }

  /// Checks if the current floating point image data is equal to another given one.
  /// The given data must be of floating point type as well.
  /// \param imgData Image data to be compared with.
  /// \return True if data are equal, false otherwise.
  bool operator==(const ImageData& imgData) const override;

  std::vector<float> data;
};

/// Image class, handling images of different formats.
class Image {
public:
  Image() = default;
  explicit Image(ImageColorspace colorspace, ImageDataType dataType = ImageDataType::BYTE);
  Image(unsigned int width, unsigned int height, ImageColorspace colorspace, ImageDataType dataType = ImageDataType::BYTE);
  Image(const Image& image);
  Image(Image&&) noexcept = default;

  unsigned int getWidth() const noexcept { return m_width; }
  unsigned int getHeight() const noexcept { return m_height; }
  ImageColorspace getColorspace() const noexcept { return m_colorspace; }
  ImageDataType getDataType() const noexcept { return m_dataType; }
  uint8_t getChannelCount() const noexcept { return m_channelCount; }
  const void* getDataPtr() const noexcept { return m_data->getDataPtr(); }
  void* getDataPtr() noexcept { return m_data->getDataPtr(); }

  template <typename... Args> static ImagePtr create(Args&&... args) { return std::make_unique<Image>(std::forward<Args>(args)...); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const { return (!m_data || m_data->isEmpty()); }

  Image& operator=(const Image& image);
  Image& operator=(Image&&) noexcept = default;
  /// Checks if the current image is equal to another given one.
  /// Their inner data must be of the same type.
  /// \param img Image to be compared with.
  /// \return True if images are the same, false otherwise.
  bool operator==(const Image& img) const;
  /// Checks if the current image is different from another given one.
  /// Their inner data must be of the same type.
  /// \param img Image to be compared with.
  /// \return True if images are different, false otherwise.
  bool operator!=(const Image& img) const { return !(*this == img); }

private:
  unsigned int m_width {};
  unsigned int m_height {};
  ImageColorspace m_colorspace {};
  ImageDataType m_dataType {};
  uint8_t m_channelCount {};

  ImageDataPtr m_data {};
};

} // namespace Raz

#endif // RAZ_IMAGE_HPP
