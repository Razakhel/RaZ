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
  std::array<png_byte, PNG_HEADER_SIZE> header;
  file.read(reinterpret_cast<char*>(header.data()), PNG_HEADER_SIZE);

  return (png_sig_cmp(header.data(), 0, PNG_HEADER_SIZE) == 0);
}

} // namespace

void Image::readJpeg(std::ifstream& file) {
  throw std::runtime_error("Error: JPEG reading not yet implemented");
}

void Image::readPng(std::ifstream& file) {
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
    static_cast<std::istream*>(inPtr)->read(reinterpret_cast<char*>(data), length);
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
      break;

    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(readStruct);
      channels = 3;
      break;

    default:
      break;
  }

  png_set_scale_16(readStruct);

  // Adding full alpha channel to the image if it possesses transparency
  if (png_get_valid(readStruct, infoStruct, static_cast<png_uint_32>(PNG_INFO_tRNS))) {
    png_set_tRNS_to_alpha(readStruct);
    ++channels;
  }

  png_read_update_info(readStruct, infoStruct);

  m_data.resize(m_width * m_height * channels);

  std::vector<png_bytep> rowPtrs(m_height);

  // Mapping row's elements to data's
  for (unsigned int i = 0; i < m_height; ++i)
    rowPtrs[i] = &m_data[m_width * channels * i];

  png_read_image(readStruct, rowPtrs.data());
  png_read_end(readStruct, infoStruct);
  png_destroy_read_struct(&readStruct, nullptr, &infoStruct);
}

void Image::readTga(std::ifstream& file) {
  throw std::runtime_error("Error: TGA reading not yet implemented");
}

void Image::readBmp(std::ifstream& file) {
  throw std::runtime_error("Error: BMP reading not yet implemented");
}

void Image::readBpg(std::ifstream& file) {
  throw std::runtime_error("Error: BPG reading not yet implemented");
}

void Image::read(const std::string& fileName) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "jpg" || format == "jpeg" || format == "JPG" || format == "JPEG")
      readJpeg(file);
    else if (format == "png" || format == "PNG")
      readPng(file);
    else if (format == "tga" || format == "TGA")
      readTga(file);
    else if (format == "bmp" || format == "BMP")
      readBmp(file);
    else if (format == "bpg" || format == "BPG")
      readBpg(file);
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + fileName + "'");
  }
}

} // namespace Raz
