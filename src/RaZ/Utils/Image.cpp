#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <cassert>
#include <fstream>

namespace Raz {

bool ImageDataB::operator==(const ImageData& imgData) const {
  assert("Error: Image data equality check requires having data of the same type." && imgData.getDataType() == ImageDataType::BYTE);

  return std::equal(data.cbegin(), data.cend(), static_cast<const ImageDataB*>(&imgData)->data.cbegin());
}

bool ImageDataF::operator==(const ImageData& imgData) const {
  assert("Error: Image data equality check requires having data of the same type." && imgData.getDataType() == ImageDataType::FLOAT);

  return std::equal(data.cbegin(), data.cend(), static_cast<const ImageDataF*>(&imgData)->data.cbegin());
}

Image::Image(unsigned int width, unsigned int height, ImageColorspace colorspace, ImageDataType dataType) : m_width{ width }, m_height{ height },
                                                                                                            m_colorspace{ colorspace }, m_dataType{ dataType } {
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

  m_bitDepth = 8; // TODO: the bit depth should most likely differ if using a floating-point data type

  const std::size_t imageDataSize = width * height * m_channelCount;

  if (dataType == ImageDataType::FLOAT || colorspace == ImageColorspace::DEPTH)
    m_data = ImageDataF::create(imageDataSize);
  else
    m_data = ImageDataB::create(imageDataSize);
}

Image::Image(const Image& image) : m_width{ image.m_width },
                                   m_height{ image.m_height },
                                   m_colorspace{ image.m_colorspace },
                                   m_dataType{ image.m_dataType },
                                   m_channelCount{ image.m_channelCount },
                                   m_bitDepth{ image.m_bitDepth } {
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

void Image::read(const FilePath& filePath, bool flipVertically) {
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Couldn't open the image file '" + filePath + "'");

  const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (format == "png")
    readPng(file, flipVertically);
  else if (format == "tga")
    readTga(file, flipVertically);
  else
    throw std::invalid_argument("Error: '" + format + "' image format is not supported");
}

void Image::save(const FilePath& filePath, bool flipVertically) const {
  if (isEmpty()) {
    Logger::error("[Image] Cannot save empty image to '" + filePath + "'.");
    return;
  }

  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Unable to create an image file as '" + filePath + "'; path to file must exist");

  const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (format == "png")
    savePng(file, flipVertically);
  /*else if (format == "tga")
    saveTga(file, flipVertically);*/
  else
    throw std::invalid_argument("Error: '" + format + "' image format is not supported");
}

Image& Image::operator=(const Image& image) {
  m_width        = image.m_width;
  m_height       = image.m_height;
  m_colorspace   = image.m_colorspace;
  m_dataType     = image.m_dataType;
  m_channelCount = image.m_channelCount;
  m_bitDepth     = image.m_bitDepth;

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
  if (m_data == nullptr || img.m_data == nullptr)
    return false;

  assert("Error: Image equality check requires having images of the same type." && m_dataType == img.m_dataType);

  return (*m_data == *img.m_data);
}

} // namespace Raz
