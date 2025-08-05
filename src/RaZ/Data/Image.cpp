#include "RaZ/Data/Image.hpp"

#include <stdexcept>

namespace Raz {

bool ImageDataB::operator==(const ImageData& imgData) const {
  assert("Error: Image data equality check requires having data of the same type." && imgData.getDataType() == ImageDataType::BYTE);

  const auto& imgDataB = static_cast<const ImageDataB&>(imgData);

  if (data.size() != imgDataB.data.size())
    return false;

  return std::equal(data.cbegin(), data.cend(), imgDataB.data.cbegin());
}

bool ImageDataF::operator==(const ImageData& imgData) const {
  assert("Error: Image data equality check requires having data of the same type." && imgData.getDataType() == ImageDataType::FLOAT);

  const auto& imgDataF = static_cast<const ImageDataF&>(imgData);

  if (data.size() != imgDataF.data.size())
    return false;

  return std::equal(data.cbegin(), data.cend(), imgDataF.data.cbegin());
}

Image::Image(ImageColorspace colorspace, ImageDataType dataType) : m_colorspace{ colorspace }, m_dataType{ dataType } {
  assert("Error: An sRGB(A) image must have a byte data type."
      && (m_colorspace != ImageColorspace::SRGB || m_colorspace != ImageColorspace::SRGBA || m_dataType == ImageDataType::BYTE));

  switch (colorspace) {
    case ImageColorspace::GRAY:
      m_channelCount = 1;
      break;

    case ImageColorspace::GRAY_ALPHA:
      m_channelCount = 2;
      break;

    case ImageColorspace::RGB:
    case ImageColorspace::SRGB:
      m_channelCount = 3;
      break;

    case ImageColorspace::RGBA:
    case ImageColorspace::SRGBA:
      m_channelCount = 4;
      break;

    default:
      throw std::invalid_argument("[Image] Invalid colorspace to create an image with");
  }
}

Image::Image(unsigned int width, unsigned int height, ImageColorspace colorspace, ImageDataType dataType) : Image(colorspace, dataType) {
  m_width  = width;
  m_height = height;

  const std::size_t imgDataSize = m_width * m_height * m_channelCount;

  if (m_dataType == ImageDataType::FLOAT)
    m_data = ImageDataF::create(imgDataSize);
  else
    m_data = ImageDataB::create(imgDataSize);
}

Image::Image(const Image& img) : m_width{ img.m_width },
                                 m_height{ img.m_height },
                                 m_colorspace{ img.m_colorspace },
                                 m_dataType{ img.m_dataType },
                                 m_channelCount{ img.m_channelCount } {
  if (img.m_data == nullptr)
    return;

  switch (img.m_dataType) {
    case ImageDataType::BYTE:
      m_data = ImageDataB::create(*static_cast<ImageDataB*>(img.m_data.get()));
      break;

    case ImageDataType::FLOAT:
      m_data = ImageDataF::create(*static_cast<ImageDataF*>(img.m_data.get()));
      break;

    default:
      throw std::invalid_argument("[Image] Invalid data type");
  }
}

uint8_t Image::recoverByteValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex) const {
  assert("Error: Getting a byte value requires the image to be of a byte type." && m_dataType == ImageDataType::BYTE);
  return recoverValue<uint8_t>(widthIndex, heightIndex, channelIndex);
}

float Image::recoverFloatValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex) const {
  assert("Error: Getting a float value requires the image to be of a float type." && m_dataType == ImageDataType::FLOAT);
  return recoverValue<float>(widthIndex, heightIndex, channelIndex);
}

void Image::setByteValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex, uint8_t val) {
  assert("Error: Setting a byte value requires the image to be of a byte type." && m_dataType == ImageDataType::BYTE);
  setValue(widthIndex, heightIndex, channelIndex, val);
}

void Image::setFloatValue(std::size_t widthIndex, std::size_t heightIndex, uint8_t channelIndex, float val) {
  assert("Error: Setting a float value requires the image to be of a float type." && m_dataType == ImageDataType::FLOAT);
  setValue(widthIndex, heightIndex, channelIndex, val);
}

Image& Image::operator=(const Image& img) {
  m_width        = img.m_width;
  m_height       = img.m_height;
  m_colorspace   = img.m_colorspace;
  m_dataType     = img.m_dataType;
  m_channelCount = img.m_channelCount;

  if (img.m_data) {
    switch (img.m_dataType) {
      case ImageDataType::BYTE:
        m_data = ImageDataB::create(*static_cast<ImageDataB*>(img.m_data.get()));
        break;

      case ImageDataType::FLOAT:
        m_data = ImageDataF::create(*static_cast<ImageDataF*>(img.m_data.get()));
        break;

      default:
        throw std::invalid_argument("[Image] Invalid data type");
    }
  } else {
    m_data.reset();
  }

  return *this;
}

bool Image::operator==(const Image& img) const {
  if (m_width != img.m_width || m_height != img.m_height || m_channelCount != img.m_channelCount || m_colorspace != img.m_colorspace
      || m_dataType != img.m_dataType || m_data == nullptr || img.m_data == nullptr)
    return false;

  return (*m_data == *img.m_data);
}

} // namespace Raz
