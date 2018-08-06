#include <array>
#include <fstream>
#include <sstream>

#include "RaZ/Utils/Image.hpp"

namespace Raz {

void Image::readTga(std::ifstream& file) {
  // Declaring a single array of unsigned char, reused everywhere later
  std::array<unsigned char, 2> bytes {};

  // ID length (identifies the number of bytes contained in field 6, the image ID Field. The maximum number
  //  of characters is 255. A value of zero indicates that no image ID field is included within the image) - TODO
  file.read(reinterpret_cast<char*>(bytes.data()), 1);

  // Colormap (0 - no colormap, 1 - colormap)
  file.read(reinterpret_cast<char*>(bytes.data()), 1);
  const bool hasColormap = bytes[0];

  // Image type
  bool runLengthEncoding = false;
  file.read(reinterpret_cast<char*>(bytes.data()), 1);

  switch (bytes[0]) {
    case 0: // No image data available
      return;

    case 1:   // Uncompressed color-mapped
    case 2:   // Uncompressed true-color
      m_colorspace = ImageColorspace::RGB;
      m_channelCount = 3;
      break;

    case 3:   // Uncompressed gray
      m_colorspace = ImageColorspace::GRAY;
      m_channelCount = 1;
      break;

    case 9:   // RLE color-mapped
    case 10:  // RLE true-color
      m_colorspace = ImageColorspace::RGB;
      m_channelCount = 3;
      runLengthEncoding = true;
      break;

    case 11:  // RLE gray
      m_colorspace = ImageColorspace::GRAY;
      m_channelCount = 1;
      runLengthEncoding = true;
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
  uint16_t xOrigin = 0, yOrigin = 0;
  file.read(reinterpret_cast<char*>(&xOrigin), 2);
  file.read(reinterpret_cast<char*>(&yOrigin), 2);

  // Width & height (2 bytes each)
  uint16_t width, height;
  file.read(reinterpret_cast<char*>(&width), 2);
  m_width = width;
  file.read(reinterpret_cast<char*>(&height), 2);
  m_height = height;

  // Bit depth (1 byte)
  file.read(reinterpret_cast<char*>(bytes.data()), 1);
  m_bitDepth = bytes[0] / m_channelCount;

  // Image descriptor (1 byte) - TODO: handle image descriptor
  // Bits 3-0 give the alpha channel depth, bits 5-4 give direction
  file.read(reinterpret_cast<char*>(bytes.data()), 1);

  auto imgData = std::make_unique<ImageDataB>();
  imgData->data.resize(m_width * m_height * m_channelCount);

  if (!runLengthEncoding) {
    std::vector<uint8_t> values(imgData->data.size());
    file.read(reinterpret_cast<char*>(values.data()), values.size());

    if (m_channelCount == 3) { // 3 channels, RGB
      for (std::size_t i = 0; i < values.size(); i += 3) { // Values are laid out as BGR
        imgData->data[i + 2] = values[i];
        imgData->data[i + 1] = values[i + 1];
        imgData->data[i]     = values[i + 2];
      }
    } else { // 1 channel, grayscale
      std::move(values.begin(), values.end(), imgData->data.begin());
    }
  } else {
    throw std::runtime_error("Error: RLE on TGA images not yet handled");
  }

  m_data = std::move(imgData);
}

/*void Image::saveTga(std::ofstream& file) const {

}*/

} // namespace Raz
