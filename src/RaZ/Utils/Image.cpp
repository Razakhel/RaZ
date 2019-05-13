#include <fstream>
#include <iostream>

#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/StrUtils.hpp"

namespace Raz {

Image::Image(unsigned int width, unsigned int height, ImageColorspace colorspace) : m_width{ width },
                                                                                    m_height{ height },
                                                                                    m_colorspace{ colorspace } {
  switch (colorspace) {
    case ImageColorspace::DEPTH:
    case ImageColorspace::GRAY:
      m_channelCount = 1;
      break;

    case ImageColorspace::GRAY_ALPHA:
      m_channelCount = 2;
      break;

    case ImageColorspace::RGB:
      m_channelCount = 3;
      break;

    case ImageColorspace::RGBA:
      m_channelCount = 4;
      break;
  }

  m_bitDepth = 8;

  ImageDataBPtr imgData = ImageDataB::create();
  imgData->data.resize(width * height * m_channelCount);

  m_data = std::move(imgData);
}

void Image::read(const std::string& filePath, bool reverse) {
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = StrUtils::toLowercaseCopy(FileUtils::extractFileExtension(filePath));

    if (format == "png")
      readPng(file, reverse);
    else if (format == "tga")
      readTga(file);
    else
      std::cerr << "Warning: '" + format + "' format is not supported, image ignored" << std::endl;
  } else {
    std::cerr << "Error: Couldn't open the file '" + filePath + "'" << std::endl;
  }
}

void Image::save(const std::string& filePath, bool reverse) const {
  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);

  if (file) {
    const std::string format = StrUtils::toLowercaseCopy(FileUtils::extractFileExtension(filePath));

    if (format == "png")
      savePng(file, reverse);
    /*else if (format == "tga")
      saveTga(file);*/
    else
      std::cerr << "Warning: '" + format + "' format is not supported, image ignored" << std::endl;
  } else {
    throw std::runtime_error("Error: Unable to create a file as '" + filePath + "'; path to file must exist");
  }
}

} // namespace Raz
