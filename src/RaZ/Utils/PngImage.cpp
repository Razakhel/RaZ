#include <array>
#include <fstream>
#include <iostream>

#include "png/png.h"
#include "png/zlib.h"
#include "RaZ/Utils/Image.hpp"

namespace Raz {

namespace {

const uint8_t PNG_HEADER_SIZE = 8;

bool validatePng(std::istream& file) {
  std::array<png_byte, PNG_HEADER_SIZE> header {};
  file.read(reinterpret_cast<char*>(header.data()), PNG_HEADER_SIZE);

  return (png_sig_cmp(header.data(), 0, PNG_HEADER_SIZE) == 0);
}

} // namespace

void Image::readPng(std::ifstream& file, bool flipVertically) {
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

  m_width        = png_get_image_width(readStruct, infoStruct);
  m_height       = png_get_image_height(readStruct, infoStruct);
  m_channelCount = png_get_channels(readStruct, infoStruct);
  m_bitDepth     = png_get_bit_depth(readStruct, infoStruct);

  const uint8_t colorType = png_get_color_type(readStruct, infoStruct);

  switch (colorType) {
    case PNG_COLOR_TYPE_GRAY:
      if (m_bitDepth < 8) {
        png_set_expand_gray_1_2_4_to_8(readStruct);
        m_bitDepth = 8;
      }

      m_colorspace = ImageColorspace::GRAY;
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      m_colorspace = ImageColorspace::GRAY_ALPHA;
      break;

    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(readStruct);
      m_channelCount = 3;
      m_colorspace = ImageColorspace::RGB;
      break;

    case PNG_COLOR_TYPE_RGB:
    default:
      m_colorspace = ImageColorspace::RGB;
      break;

    case PNG_COLOR_TYPE_RGBA:
      m_colorspace = ImageColorspace::RGBA;
      break;
  }

  if (!png_set_interlace_handling(readStruct))
    std::cerr << "Error: Couldn't set PNG interlace handling" << std::endl;

  png_set_scale_16(readStruct);

  // Adding full alpha channel to the image if it possesses transparency
  if (png_get_valid(readStruct, infoStruct, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(readStruct);
    m_colorspace = ImageColorspace::RGBA;
    ++m_channelCount;
  }

  png_read_update_info(readStruct, infoStruct);

  ImageDataBPtr imgData = ImageDataB::create();
  imgData->data.resize(m_width * m_height * m_channelCount);

  std::vector<png_bytep> rowPtrs(m_height);

  // Mapping row's elements to data's
  for (std::size_t heightIndex = 0; heightIndex < m_height; ++heightIndex)
    rowPtrs[(flipVertically ? m_height - 1 - heightIndex : heightIndex)] = &imgData->data[m_width * m_channelCount * heightIndex];

  m_data = std::move(imgData);

  png_read_image(readStruct, rowPtrs.data());
  png_read_end(readStruct, infoStruct);
  png_destroy_read_struct(&readStruct, nullptr, &infoStruct);
}

void Image::savePng(std::ofstream& file, bool flipVertically) const {
  png_structp writeStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!writeStruct)
    throw std::runtime_error("Error: Couldn't initialize PNG write struct");

  png_infop infoStruct = png_create_info_struct(writeStruct);
  if (!infoStruct)
    throw std::runtime_error("Error: Couldn't initialize PNG info struct");

  uint32_t colorType {};
  switch (m_colorspace) {
    case ImageColorspace::GRAY:
    case ImageColorspace::DEPTH:
      colorType = PNG_COLOR_TYPE_GRAY;
      break;

    case ImageColorspace::GRAY_ALPHA:
      colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
      break;

    case ImageColorspace::RGB:
      colorType = PNG_COLOR_TYPE_RGB;
      break;

    case ImageColorspace::RGBA:
      colorType = PNG_COLOR_TYPE_RGBA;
      break;
  }

  png_set_compression_level(writeStruct, 6);

  if (m_channelCount * m_bitDepth >= 16) {
    png_set_compression_strategy(writeStruct, Z_FILTERED);
    png_set_filter(writeStruct, 0, PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_PAETH);
  } else {
    png_set_compression_strategy(writeStruct, Z_DEFAULT_STRATEGY);
  }

  png_set_IHDR(writeStruct,
               infoStruct,
               static_cast<png_uint_32>(m_width),
               static_cast<png_uint_32>(m_height),
               m_bitDepth,
               colorType,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_set_write_fn(writeStruct, &file, [] (png_structp pngWritePtr, png_bytep data, png_size_t length) {
    png_voidp outPtr = png_get_io_ptr(pngWritePtr);
    static_cast<std::ostream*>(outPtr)->write(reinterpret_cast<const char*>(data), length);
  }, nullptr);
  png_write_info(writeStruct, infoStruct);

  const auto dataPtr = static_cast<const uint8_t*>(m_data->getDataPtr());

  for (std::size_t heightIndex = 0; heightIndex < m_height; ++heightIndex)
    png_write_row(writeStruct, &dataPtr[m_width * m_channelCount * (flipVertically ? m_height - 1 - heightIndex : heightIndex)]);

  png_write_end(writeStruct, infoStruct);
  png_destroy_write_struct(&writeStruct, &infoStruct);
}

} // namespace Raz
