#include "RaZ/Data/Image.hpp"

#include <cassert>

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
  assert("Error: A depth image must have a floating-point data type." && (m_colorspace != ImageColorspace::DEPTH || m_dataType == ImageDataType::FLOAT));

  switch (colorspace) {
    case ImageColorspace::DEPTH:
    case ImageColorspace::GRAY:
      m_channelCount = 1;
      break;

    case ImageColorspace::GRAY_ALPHA:
      m_channelCount = 2;
      break;

    case ImageColorspace::RGB:
    default:
      m_channelCount = 3;
      break;

    case ImageColorspace::RGBA:
      m_channelCount = 4;
      break;
  }
}

Image::Image(unsigned int width, unsigned int height, ImageColorspace colorspace)
  : Image(width, height, colorspace, (colorspace == ImageColorspace::DEPTH ? ImageDataType::FLOAT : ImageDataType::BYTE)) {}

Image::Image(unsigned int width, unsigned int height, ImageColorspace colorspace, ImageDataType dataType) : Image(colorspace, dataType) {
  m_width  = width;
  m_height = height;

  const std::size_t imageDataSize = m_width * m_height * m_channelCount;

  if (m_dataType == ImageDataType::FLOAT || m_colorspace == ImageColorspace::DEPTH)
    m_data = ImageDataF::create(imageDataSize);
  else
    m_data = ImageDataB::create(imageDataSize);
}

Image::Image(const Image& image) : m_width{ image.m_width },
                                   m_height{ image.m_height },
                                   m_colorspace{ image.m_colorspace },
                                   m_dataType{ image.m_dataType },
                                   m_channelCount{ image.m_channelCount } {
  if (image.m_data == nullptr)
    return;

  switch (image.m_dataType) {
    case ImageDataType::BYTE:
      m_data = ImageDataB::create(*static_cast<ImageDataB*>(image.m_data.get()));
      break;

    case ImageDataType::FLOAT:
      m_data = ImageDataF::create(*static_cast<ImageDataF*>(image.m_data.get()));
      break;
  }
}

Image& Image::operator=(const Image& image) {
  m_width        = image.m_width;
  m_height       = image.m_height;
  m_colorspace   = image.m_colorspace;
  m_dataType     = image.m_dataType;
  m_channelCount = image.m_channelCount;

  if (image.m_data) {
    switch (image.m_dataType) {
      case ImageDataType::BYTE:
        m_data = ImageDataB::create(*static_cast<ImageDataB*>(image.m_data.get()));
        break;

      case ImageDataType::FLOAT:
        m_data = ImageDataF::create(*static_cast<ImageDataF*>(image.m_data.get()));
        break;
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
