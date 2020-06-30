#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

namespace Raz {

bool ImageDataB::operator==(const ImageData& imgData) const {
  assert("Error: Image data equality check requires having datas of the same type." && imgData.getDataType() == ImageDataType::BYTE);

  return std::equal(data.cbegin(), data.cend(), static_cast<const ImageDataB*>(&imgData)->data.cbegin());
}

bool ImageDataF::operator==(const ImageData& imgData) const {
  assert("Error: Image data equality check requires having datas of the same type." && imgData.getDataType() == ImageDataType::FLOAT);

  return std::equal(data.cbegin(), data.cend(), static_cast<const ImageDataF*>(&imgData)->data.cbegin());
}

Image::Image(unsigned int width, unsigned int height, ImageColorspace colorspace) : m_width{ width }, m_height{ height }, m_colorspace{ colorspace } {
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

  m_bitDepth = 8;

  const std::size_t imageDataSize = width * height * m_channelCount;

  if (colorspace == ImageColorspace::DEPTH) {
    ImageDataFPtr imgData = ImageDataF::create();
    imgData->data.resize(imageDataSize);

    m_data = std::move(imgData);
  } else {
    ImageDataBPtr imgData = ImageDataB::create();
    imgData->data.resize(imageDataSize);

    m_data = std::move(imgData);
  }
}

void Image::read(const FilePath& filePath, bool flipVertically) {
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

    if (format == "png")
      readPng(file, flipVertically);
    else if (format == "tga")
      readTga(file, flipVertically);
    else
      std::cerr << "Warning: '" + format + "' format is not supported, image ignored" << std::endl;
  } else {
    std::cerr << "Error: Couldn't open the file '" + filePath + "'" << std::endl;
  }
}

void Image::save(const FilePath& filePath, bool flipVertically) const {
  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);

  if (file) {
    const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

    if (format == "png")
      savePng(file, flipVertically);
    /*else if (format == "tga")
      saveTga(file, flipVertically);*/
    else
      std::cerr << "Warning: '" + format + "' format is not supported, image ignored" << std::endl;
  } else {
    throw std::runtime_error("Error: Unable to create a file as '" + filePath + "'; path to file must exist");
  }
}

bool Image::operator==(const Image& img) const {
  assert("Error: Image equality check requires having images of the same type." && getDataType() == img.getDataType());

  return (*m_data == *img.m_data);
}

} // namespace Raz
