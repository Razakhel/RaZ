#include "RaZ/Data/TgaFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Image.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <array>
#include <fstream>
#include <sstream>

namespace Raz::TgaFormat {

Image load(const FilePath& filePath, bool flipVertically) {
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Could not open the PNG file '" + filePath + "'");

  // Declaring a single array of unsigned char, reused everywhere later
  std::array<unsigned char, 2> bytes {};

  // ID length (identifies the number of bytes contained in field 6, the image ID Field. The maximum number
  //  of characters is 255. A value of zero indicates that no image ID field is included within the image) - TODO
  file.read(reinterpret_cast<char*>(bytes.data()), 1);

  // Colormap (0 - no colormap, 1 - colormap)
  file.read(reinterpret_cast<char*>(bytes.data()), 1);
  const bool hasColormap = (bytes[0] == 1);

  // Image type
  file.read(reinterpret_cast<char*>(bytes.data()), 1);

  bool runLengthEncoding = false;
  uint8_t channelCount {};
  ImageColorspace colorspace {};

  switch (bytes[0]) {
    case 0: // No image data available
      throw std::runtime_error("Error: Invalid TGA image, no data available");

    case 9:   // RLE color-mapped
    case 10:  // RLE true-color
      runLengthEncoding = true;
      [[fallthrough]];
    case 1:   // Uncompressed color-mapped
    case 2:   // Uncompressed true-color
      channelCount = 3;
      colorspace = ImageColorspace::RGB;
      break;

    case 11:  // RLE gray
      runLengthEncoding = true;
      [[fallthrough]];
    case 3:   // Uncompressed gray
      channelCount = 1;
      colorspace = ImageColorspace::GRAY;
      break;

    default:
      throw std::runtime_error("Error: TGA invalid image type");
  }

  // TODO: handle colormap
  if (hasColormap) {
    // Colormap specs (size 5)
    std::array<char, 5> colorSpecs {};
    file.read(colorSpecs.data(), 5);

    // First entry index (2 bytes)

    // Colormap length (2 bytes)

    // Colormap entry size (1 byte)
  } else {
    file.ignore(5);
  }

  // Image specs (10 bytes)

  // X- & Y-origin (2 bytes each) - TODO: handle origins
  // It is expected to have 0 for both origins
  uint16_t xOrigin {};
  file.read(reinterpret_cast<char*>(&xOrigin), 2);

  uint16_t yOrigin {};
  file.read(reinterpret_cast<char*>(&yOrigin), 2);

  // Width & height (2 bytes each)
  uint16_t width {};
  file.read(reinterpret_cast<char*>(&width), 2);

  uint16_t height {};
  file.read(reinterpret_cast<char*>(&height), 2);

  // Bit depth (1 byte)
  file.read(reinterpret_cast<char*>(bytes.data()), 1);
  [[maybe_unused]] const uint8_t bitDepth = bytes[0] / channelCount;

  // Image descriptor (1 byte) - TODO: handle image descriptor
  // Bits 3-0 give the alpha channel depth, bits 5-4 give direction
  file.read(reinterpret_cast<char*>(bytes.data()), 1);

  Image image(width, height, colorspace, ImageDataType::BYTE);
  auto* imgData = static_cast<uint8_t*>(image.getDataPtr());

  if (!runLengthEncoding) {
    std::vector<uint8_t> values(width * height * channelCount);
    file.read(reinterpret_cast<char*>(values.data()), static_cast<std::streamsize>(values.size()));

    if (channelCount == 3) { // 3 channels, RGB
      for (std::size_t heightIndex = 0; heightIndex < height; ++heightIndex) {
        const std::size_t finalHeightIndex = (flipVertically ? heightIndex : height - 1 - heightIndex);

        for (std::size_t widthIndex = 0; widthIndex < width; ++widthIndex) {
          const std::size_t inPixelIndex = (heightIndex * width + widthIndex) * channelCount;
          const std::size_t outPixelIndex = (finalHeightIndex * width + widthIndex) * channelCount;

          // Values are laid out as BGR, they need to be reordered to RGB
          imgData[outPixelIndex + 2] = values[inPixelIndex];
          imgData[outPixelIndex + 1] = values[inPixelIndex + 1];
          imgData[outPixelIndex]     = values[inPixelIndex + 2];
        }
      }
    } else { // 1 channel, grayscale
      std::move(values.begin(), values.end(), imgData);
    }
  } else {
    throw std::runtime_error("Error: RLE on TGA images is not handled yet");
  }

  return image;
}

} // namespace Raz::TgaFormat
