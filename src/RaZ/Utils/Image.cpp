#include <array>
#include <cassert>
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
  assert(("Error: JPEG reading not yet implemented"));
}

void Image::readPng(std::ifstream& file) {
  const bool valid = file.good() && validatePng(file);
  assert(("Error: Not a valid PNG", valid));

  png_structp pngReadStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  assert(("Error: Couldn't initialize PNG read struct", pngReadStruct));

  png_infop pngInfoStruct = png_create_info_struct(pngReadStruct);
  assert(("Error: Couldn't initialize PNG info struct", pngInfoStruct));

  png_set_read_fn(pngReadStruct, &file, [] (png_structp pngReadPtr, png_bytep data, png_size_t length) {
    png_voidp inPtr = png_get_io_ptr(pngReadPtr);
    static_cast<std::istream*>(inPtr)->read(reinterpret_cast<char*>(data), length);
  });

  // Setting the amount signature bytes we've already read
  png_set_sig_bytes(pngReadStruct, PNG_HEADER_SIZE);

  png_read_info(pngReadStruct, pngInfoStruct);

  const uint32_t width = png_get_image_width(pngReadStruct, pngInfoStruct);
  const uint32_t height = png_get_image_height(pngReadStruct, pngInfoStruct);
  uint8_t bitDepth = png_get_bit_depth(pngReadStruct, pngInfoStruct);
  uint8_t channels = png_get_channels(pngReadStruct, pngInfoStruct);
  const uint8_t colorType = png_get_color_type(pngReadStruct, pngInfoStruct);

  switch (colorType) {
    case PNG_COLOR_TYPE_GRAY:
      if (bitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(pngReadStruct);

      bitDepth = 8;
      break;

    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(pngReadStruct);
      channels = 3;
      break;

    default:
      break;
  }

  png_set_scale_16(pngReadStruct);

  // Adding full alpha channel to the image if it possesses transparency
  if (png_get_valid(pngReadStruct, pngInfoStruct, static_cast<png_uint_32>(PNG_INFO_tRNS))) {
    png_set_tRNS_to_alpha(pngReadStruct);
    ++channels;
  }

  png_read_update_info(pngReadStruct, pngInfoStruct);

  m_data.resize(width * height * channels);

  std::vector<png_bytep> rowPtrs(height);

  // Mapping row's elements to data's
  for (unsigned int i = 0; i < height; ++i)
    rowPtrs[i] = &m_data[width * channels * i];

  png_read_image(pngReadStruct, rowPtrs.data());
  png_read_end(pngReadStruct, pngInfoStruct);
  png_destroy_read_struct(&pngReadStruct, nullptr, &pngInfoStruct);
}

void Image::readTga(std::ifstream& file) {
  assert(("Error: TGA reading not yet implemented"));
}

void Image::readBmp(std::ifstream& file) {
  assert(("Error: BMP reading not yet implemented"));
}

void Image::readBpg(std::ifstream& file) {
  assert(("Error: BPG reading not yet implemented"));
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
      std::cerr << "Error: '" << format << "' format is not supported" << std::endl;
  } else {
    std::cerr << "Error: Couldn't open the file '" << fileName << "'" << std::endl;
  }
}

} // namespace Raz
