#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include <cassert>
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

template <typename T, std::size_t Size>
class Vector;

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
  Image(const Image& img);
  Image(Image&&) noexcept = default;

  unsigned int getWidth() const noexcept { return m_width; }
  unsigned int getHeight() const noexcept { return m_height; }
  ImageColorspace getColorspace() const noexcept { return m_colorspace; }
  ImageDataType getDataType() const noexcept { return m_dataType; }
  uint8_t getChannelCount() const noexcept { return m_channelCount; }
  const void* getDataPtr() const noexcept { return m_data->getDataPtr(); }
  void* getDataPtr() noexcept { return m_data->getDataPtr(); }

  /// Checks if the image doesn't contain data.
  /// \return True if the image has no data, false otherwise.
  bool isEmpty() const { return (!m_data || m_data->isEmpty()); }
  /// Gets a byte value from the image.
  /// \note The image must have a byte data type for this function to execute properly.
  /// \param widthIndex Width index of the value to be fetched.
  /// \param heightIndex Height index of the value to be fetched.
  /// \param channelIndex Channel index of the value to be fetched.
  /// \return Value at the given location.
  uint8_t recoverByteValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex) const;
  /// Gets a float value from the image.
  /// \note The image must have a float data type for this function to execute properly.
  /// \param widthIndex Width index of the value to be fetched.
  /// \param heightIndex Height index of the value to be fetched.
  /// \param channelIndex Channel index of the value to be fetched.
  /// \return Value at the given location.
  float recoverFloatValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex) const;
  /// Gets a single-component pixel from the image.
  /// \note This gets a single value, hence requires an image with a single channel.
  /// \tparam T Type of the pixel to be fetched.
  /// \param widthIndex Width index of the pixel to be fetched.
  /// \param heightIndex Height index of the pixel to be fetched.
  /// \return Pixel at the given location.
  template <typename T>
  T recoverPixel(std::size_t widthIndex, std::size_t heightIndex) const;
  /// Gets a pixel from the image with multiple components.
  /// \note The image requires having a channel count equal to the given value count.
  /// \tparam T Type of the pixel to be fetched.
  /// \tparam N Number of values to be fetched. Must be equal to the image's channel count.
  /// \param widthIndex Width index of the pixel to be fetched.
  /// \param heightIndex Height index of the pixel to be fetched.
  /// \return Pixel at the given location.
  template <typename T, std::size_t N>
  Vector<T, N> recoverPixel(std::size_t widthIndex, std::size_t heightIndex) const;
  /// Sets a byte value in the image.
  /// \note The image must have a byte data type for this function to execute properly.
  /// \param widthIndex Width index of the value to be set.
  /// \param heightIndex Height index of the value to be set.
  /// \param channelIndex Channel index of the value to be set.
  /// \param val Value to be set.
  void setByteValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex, uint8_t val);
  /// Sets a float value in the image.
  /// \note The image must have a float data type for this function to execute properly.
  /// \param widthIndex Width index of the value to be set.
  /// \param heightIndex Height index of the value to be set.
  /// \param channelIndex Channel index of the value to be set.
  /// \param val Value to be set.
  void setFloatValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex, float val);
  /// Sets a pixel in the image.
  /// \tparam T Type of the pixel to be set.
  /// \param widthIndex Width index of the pixel to be set.
  /// \param heightIndex Height index of the pixel to be set.
  /// \param val Value to be set.
  template <typename T>
  void setPixel(std::size_t widthIndex, std::size_t heightIndex, T val);
  /// Sets a pixel in the image with multiple components.
  /// \note The image requires having a channel count equal to the given value count.
  /// \tparam T Type of the pixel to be set.
  /// \tparam N Number of values to be set. Must be equal to the image's channel count.
  /// \param widthIndex Width index of the pixel to be set.
  /// \param heightIndex Height index of the pixel to be set.
  /// \param values Values to be set.
  template <typename T, std::size_t N>
  void setPixel(std::size_t widthIndex, std::size_t heightIndex, const Vector<T, N>& values);

  Image& operator=(const Image& img);
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
  constexpr std::size_t computeIndex(std::size_t widthIndex, std::size_t heightIndex) const noexcept {
    assert("Error: The given width index is invalid." && widthIndex < m_width);
    assert("Error: The given height index is invalid." && heightIndex < m_height);
    return heightIndex * m_width * m_channelCount + widthIndex * m_channelCount;
  }

  constexpr std::size_t computeIndex(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex) const noexcept {
    assert("Error: The given channel index is invalid." && channelIndex < m_channelCount);
    return computeIndex(widthIndex, heightIndex) + channelIndex;
  }

  template <typename T>
  T recoverValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex) const {
    const std::size_t valIndex = computeIndex(widthIndex, heightIndex, channelIndex);
    return static_cast<const T*>(m_data->getDataPtr())[valIndex];
  }

  template <typename T>
  void setValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex, T val) {
    const std::size_t valIndex = computeIndex(widthIndex, heightIndex, channelIndex);
    static_cast<T*>(m_data->getDataPtr())[valIndex] = val;
  }

  unsigned int m_width {};
  unsigned int m_height {};
  ImageColorspace m_colorspace {};
  ImageDataType m_dataType {};
  uint8_t m_channelCount {};

  ImageDataPtr m_data {};
};

} // namespace Raz

#include "RaZ/Data/Image.inl"

#endif // RAZ_IMAGE_HPP
