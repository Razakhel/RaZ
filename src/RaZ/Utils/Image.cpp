#include <array>
#include <fstream>
#include <iostream>

#include "png/png.h"
#include "RaZ/Utils/Image.hpp"

namespace Raz {

namespace {

const uint8_t PNG_HEADER_SIZE = 8;

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

bool validatePng(std::istream& file) {
  std::array<png_byte, PNG_HEADER_SIZE> header {};
  file.read(reinterpret_cast<char*>(header.data()), PNG_HEADER_SIZE);

  return (png_sig_cmp(header.data(), 0, PNG_HEADER_SIZE) == 0);
}

} // namespace

void Image::readPng(std::ifstream& file, bool reverse) {
  if (!validatePng(file))
    throw std::runtime_error("Error: Not a valid PNG");

  png_structp readStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!readStruct)
    throw std::runtime_error("Error: Couldn't initialize PNG read struct");

  png_infop infoStruct = png_create_info_struct(readStruct);
  if (!infoStruct)
    throw std::runtime_error("Error: Couldn't initialize PNG info struct");

  png_set_read_fn(readStruct, &file, [] (png_structp pngReadPtr, png_bytep data, png_size_t length) {
    png_voidp inPtr = png_get_io_ptr(pngReadPtr);
    static_cast<std::istream*>(inPtr)->read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(length));
  });

  // Setting the amount signature bytes we've already read
  png_set_sig_bytes(readStruct, PNG_HEADER_SIZE);

  png_read_info(readStruct, infoStruct);

  m_width = png_get_image_width(readStruct, infoStruct);
  m_height = png_get_image_height(readStruct, infoStruct);
  uint8_t channels = png_get_channels(readStruct, infoStruct);
  const uint8_t colorType = png_get_color_type(readStruct, infoStruct);

  switch (colorType) {
    case PNG_COLOR_TYPE_GRAY:
      if (png_get_bit_depth(readStruct, infoStruct) < 8)
        png_set_expand_gray_1_2_4_to_8(readStruct);

      m_colorspace = GL_RED;
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      m_colorspace = GL_RG;
      break;

    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(readStruct);
      channels = 3;
      m_colorspace = GL_RGB;
      break;

    case PNG_COLOR_TYPE_RGB:
    default:
      m_colorspace = GL_RGB;
      break;

    case PNG_COLOR_TYPE_RGBA:
      m_colorspace = GL_RGBA;
      break;
  }

  png_set_scale_16(readStruct);

  // Adding full alpha channel to the image if it possesses transparency
  if (png_get_valid(readStruct, infoStruct, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(readStruct);
    m_colorspace = GL_RGBA;
    ++channels;
  }

  png_read_update_info(readStruct, infoStruct);

  m_data.resize(m_width * m_height * channels);

  std::vector<png_bytep> rowPtrs(m_height);

  // Mapping row's elements to data's
  for (std::size_t i = 0; i < m_height; ++i)
    rowPtrs[(reverse ? i : m_height - 1 - i)] = &m_data[m_width * channels * i];

  png_read_image(readStruct, rowPtrs.data());
  png_read_end(readStruct, infoStruct);
  png_destroy_read_struct(&readStruct, nullptr, &infoStruct);
}

void Image::read(const std::string& fileName, bool reverse) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "png" || format == "PNG")
      readPng(file, reverse);
    else
      std::cerr << "Warning: '" + format + "' format is not supported, image ignored";
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + fileName + "'");
  }
}

} // namespace Raz
