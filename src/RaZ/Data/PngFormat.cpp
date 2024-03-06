#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/PngFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "png.h"
#include "zlib.h"

#include "tracy/Tracy.hpp"

#include <array>
#include <fstream>

namespace Raz::PngFormat {

namespace {

constexpr uint8_t PNG_HEADER_SIZE = 8;

inline bool validatePng(std::istream& file) {
  std::array<png_byte, PNG_HEADER_SIZE> header {};
  file.read(reinterpret_cast<char*>(header.data()), PNG_HEADER_SIZE);

  return (png_sig_cmp(header.data(), 0, PNG_HEADER_SIZE) == 0);
}

} // namespace

Image load(const FilePath& filePath, bool flipVertically) {
  ZoneScopedN("PngFormat::load");

  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Could not open the PNG file '" + filePath + "'");

  if (!validatePng(file))
    throw std::runtime_error("Error: Not a valid PNG file");

  png_structp readStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (readStruct == nullptr)
    throw std::runtime_error("Error: Could not initialize PNG read struct");

  png_infop infoStruct = png_create_info_struct(readStruct);
  if (infoStruct == nullptr)
    throw std::runtime_error("Error: Could not initialize PNG info struct");

  png_set_read_fn(readStruct, &file, [] (png_structp pngReadPtr, png_bytep data, png_size_t length) {
    png_voidp inPtr = png_get_io_ptr(pngReadPtr);
    static_cast<std::istream*>(inPtr)->read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(length));
  });

  // Setting the amount signature bytes we've already read
  png_set_sig_bytes(readStruct, PNG_HEADER_SIZE);

  png_read_info(readStruct, infoStruct);

  const unsigned int width  = png_get_image_width(readStruct, infoStruct);
  const unsigned int height = png_get_image_height(readStruct, infoStruct);
  const uint8_t colorType   = png_get_color_type(readStruct, infoStruct);
  uint8_t bitDepth          = png_get_bit_depth(readStruct, infoStruct);
  uint8_t channelCount      = png_get_channels(readStruct, infoStruct);

  ImageColorspace colorspace {};

  switch (colorType) {
    case PNG_COLOR_TYPE_GRAY:
      if (bitDepth < 8) {
        png_set_expand_gray_1_2_4_to_8(readStruct);
        bitDepth = 8;
      }

      colorspace = ImageColorspace::GRAY;
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      colorspace = ImageColorspace::GRAY_ALPHA;
      break;

    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(readStruct);
      channelCount = 3;
      [[fallthrough]];

    case PNG_COLOR_TYPE_RGB:
    default:
      colorspace = ImageColorspace::RGB;
      break;

    case PNG_COLOR_TYPE_RGBA:
      colorspace = ImageColorspace::RGBA;
      break;
  }

  if (!png_set_interlace_handling(readStruct))
    Logger::error("[PngLoad] Could not set PNG interlace handling.");

  png_set_scale_16(readStruct);

  // Adding full alpha channel to the image if it possesses transparency
  if (png_get_valid(readStruct, infoStruct, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(readStruct);
    colorspace = ImageColorspace::RGBA;
    ++channelCount;
  }

  png_read_update_info(readStruct, infoStruct);

  Image image(width, height, colorspace, ImageDataType::BYTE);
  auto* imgData = static_cast<uint8_t*>(image.getDataPtr());

  std::vector<png_bytep> rowPtrs(height);

  // Mapping row's elements to data's
  for (std::size_t heightIndex = 0; heightIndex < height; ++heightIndex)
    rowPtrs[(flipVertically ? height - 1 - heightIndex : heightIndex)] = &imgData[width * channelCount * heightIndex];

  png_read_image(readStruct, rowPtrs.data());
  png_read_end(readStruct, infoStruct);
  png_destroy_read_struct(&readStruct, nullptr, &infoStruct);

  return image;
}

void save(const FilePath& filePath, const Image& image, bool flipVertically) {
  ZoneScopedN("PngFormat::save");

  if (image.isEmpty()) {
    Logger::error("[PngSave] Cannot save empty image to '" + filePath + "'.");
    return;
  }

  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Unable to create a PNG file as '" + filePath + "'; path to file must exist");

  png_structp writeStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!writeStruct)
    throw std::runtime_error("Error: Could not initialize PNG write struct");

  png_infop infoStruct = png_create_info_struct(writeStruct);
  if (!infoStruct)
    throw std::runtime_error("Error: Could not initialize PNG info struct");

  int colorType {};

  switch (image.getColorspace()) {
    case ImageColorspace::GRAY:
      colorType = PNG_COLOR_TYPE_GRAY;
      break;

    case ImageColorspace::GRAY_ALPHA:
      colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
      break;

    case ImageColorspace::RGB:
    case ImageColorspace::SRGB:
    default:
      colorType = PNG_COLOR_TYPE_RGB;
      break;

    case ImageColorspace::RGBA:
    case ImageColorspace::SRGBA:
      colorType = PNG_COLOR_TYPE_RGBA;
      break;
  }

  png_set_compression_level(writeStruct, 6);

  if (image.getChannelCount() >= 2) {
    png_set_compression_strategy(writeStruct, Z_FILTERED);
    png_set_filter(writeStruct, 0, PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_PAETH);
  } else {
    png_set_compression_strategy(writeStruct, Z_DEFAULT_STRATEGY);
  }

  png_set_IHDR(writeStruct,
               infoStruct,
               image.getWidth(),
               image.getHeight(),
               8, // Only 8-bit images will be written
               colorType,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_set_write_fn(writeStruct, &file, [] (png_structp pngWritePtr, png_bytep data, png_size_t length) {
    png_voidp outPtr = png_get_io_ptr(pngWritePtr);
    static_cast<std::ostream*>(outPtr)->write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(length));
  }, nullptr);
  png_write_info(writeStruct, infoStruct);

  const std::size_t pixelIndexBase = image.getWidth() * image.getChannelCount();

  if (image.getDataType() == ImageDataType::FLOAT) {
    // Manually converting floating-point pixels to standard byte ones
    const auto* pixels = static_cast<const float*>(image.getDataPtr());
    std::vector<uint8_t> rgbPixels(image.getWidth() * image.getHeight() * image.getChannelCount());

    for (std::size_t i = 0; i < rgbPixels.size(); ++i)
      rgbPixels[i] = static_cast<uint8_t>(pixels[i] * 255);

    const uint8_t* dataPtr = rgbPixels.data();

    for (std::size_t heightIndex = 0; heightIndex < image.getHeight(); ++heightIndex)
      png_write_row(writeStruct, &dataPtr[pixelIndexBase * (flipVertically ? image.getHeight() - 1 - heightIndex : heightIndex)]);
  } else {
    const auto* dataPtr = static_cast<const uint8_t*>(image.getDataPtr());

    for (std::size_t heightIndex = 0; heightIndex < image.getHeight(); ++heightIndex)
      png_write_row(writeStruct, &dataPtr[pixelIndexBase * (flipVertically ? image.getHeight() - 1 - heightIndex : heightIndex)]);
  }

  png_write_end(writeStruct, infoStruct);
  png_destroy_write_struct(&writeStruct, &infoStruct);
}

} // namespace Raz::PngFormat
