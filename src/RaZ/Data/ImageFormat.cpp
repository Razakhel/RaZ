#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STBI_WINDOWS_UTF8
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_WINDOWS_UTF8
#include "stb_image_write.h"

#include "tracy/Tracy.hpp"

namespace Raz::ImageFormat {

namespace {

enum class FileFormat {
  UNKNOWN,
  BMP,
  PNG,
  JPG,
  TGA,
  HDR
};

struct ImageDataDeleter {
  void operator()(void* data) noexcept { stbi_image_free(data); }
};

ImageColorspace recoverColorspace(int channelCount) {
  switch (channelCount) {
    case 1: return ImageColorspace::GRAY;
    case 2: return ImageColorspace::GRAY_ALPHA;
    case 3: return ImageColorspace::RGB;
    case 4: return ImageColorspace::RGBA;
    default:
      throw std::invalid_argument("Error: Unsupported number of channels.");
  }
}

FileFormat recoverFileFormat(const std::string& fileExt) {
  if (fileExt == "bmp")
    return FileFormat::BMP;
  if (fileExt == "hdr")
    return FileFormat::HDR;
  if (fileExt == "jpg" || fileExt == "jpeg")
    return FileFormat::JPG;
  if (fileExt == "png")
    return FileFormat::PNG;
  if (fileExt == "tga")
    return FileFormat::TGA;

  return FileFormat::UNKNOWN;
}

Image createImageFromData(int width, int height, int channelCount, bool isHdr, const std::unique_ptr<void, ImageDataDeleter>& data) {
  const std::size_t valueCount = width * height * channelCount;

  Image img(width, height, recoverColorspace(channelCount), (isHdr ? ImageDataType::FLOAT : ImageDataType::BYTE));

  if (isHdr)
    std::copy_n(static_cast<float*>(data.get()), valueCount, static_cast<float*>(img.getDataPtr()));
  else
    std::copy_n(static_cast<uint8_t*>(data.get()), valueCount, static_cast<uint8_t*>(img.getDataPtr()));

  return img;
}

} // namespace

Image load(const FilePath& filePath, bool flipVertically) {
  ZoneScopedN("ImageFormat::load");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  Logger::debug("[ImageFormat] Loading image '{}'...", filePath);

  const std::string fileStr = filePath.toUtf8();
  const bool isHdr = (stbi_is_hdr(fileStr.c_str()) != 0);

  stbi_set_flip_vertically_on_load(flipVertically);

  int width {};
  int height {};
  int channelCount {};
  std::unique_ptr<void, ImageDataDeleter> data;

  if (isHdr)
    data.reset(stbi_loadf(fileStr.c_str(), &width, &height, &channelCount, 0));
  else
    data.reset(stbi_load(fileStr.c_str(), &width, &height, &channelCount, 0));

  if (data == nullptr)
    throw std::invalid_argument(std::format("[ImageFormat] Cannot load image '{}': {}", filePath, stbi_failure_reason()));

  Image img = createImageFromData(width, height, channelCount, isHdr, data);

  Logger::debug("[ImageFormat] Loaded image");

  return img;
}

Image loadFromData(const std::vector<unsigned char>& imgData, bool flipVertically) {
  return loadFromData(imgData.data(), imgData.size(), flipVertically);
}

Image loadFromData(const unsigned char* imgData, std::size_t dataSize, bool flipVertically) {
  ZoneScopedN("ImageFormat::loadFromData");

  Logger::debug("[ImageFormat] Loading image from data...");

  stbi_set_flip_vertically_on_load(flipVertically);

  const bool isHdr = (stbi_is_hdr_from_memory(imgData, static_cast<int>(dataSize)) != 0);

  int width {};
  int height {};
  int channelCount {};
  std::unique_ptr<void, ImageDataDeleter> data;

  if (isHdr)
    data.reset(stbi_loadf_from_memory(imgData, static_cast<int>(dataSize), &width, &height, &channelCount, 0));
  else
    data.reset(stbi_load_from_memory(imgData, static_cast<int>(dataSize), &width, &height, &channelCount, 0));

  if (data == nullptr)
    throw std::invalid_argument(std::format("[ImageFormat] Cannot load image from data: {}", stbi_failure_reason()));

  Image img = createImageFromData(width, height, static_cast<uint8_t>(channelCount), isHdr, data);

  Logger::debug("[ImageFormat] Loaded image from data");

  return img;
}

void save(const FilePath& filePath, const Image& image, bool flipVertically) {
  ZoneScopedN("ImageFormat::save");

  Logger::debug("[ImageFormat] Saving image to '{}'...", filePath);

  const std::string fileStr = filePath.toUtf8();
  const std::string fileExt = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  stbi_flip_vertically_on_write(flipVertically);

  const auto imgWidth        = static_cast<int>(image.getWidth());
  const auto imgHeight       = static_cast<int>(image.getHeight());
  const auto imgChannelCount = static_cast<int>(image.getChannelCount());

  const FileFormat format = recoverFileFormat(fileExt);

  switch (format) {
    case FileFormat::BMP:
    case FileFormat::JPG:
    case FileFormat::PNG:
    case FileFormat::TGA:
      if (image.getDataType() != ImageDataType::BYTE)
        throw std::invalid_argument("[ImageFormat] Saving a non-HDR image requires a byte data type.");
      break;

    case FileFormat::HDR:
      if (image.getDataType() != ImageDataType::FLOAT)
        throw std::invalid_argument("[ImageFormat] Saving an HDR image requires a floating-point data type.");
      break;

    case FileFormat::UNKNOWN:
    default:
      break;
  }

  int result {};

  switch (format) {
    case FileFormat::BMP:
      result = stbi_write_bmp(fileStr.c_str(), imgWidth, imgHeight, imgChannelCount, image.getDataPtr());
      break;

    case FileFormat::HDR:
      result = stbi_write_hdr(fileStr.c_str(), imgWidth, imgHeight, imgChannelCount, static_cast<const float*>(image.getDataPtr()));
      break;

    case FileFormat::JPG:
      result = stbi_write_jpg(fileStr.c_str(), imgWidth, imgHeight, imgChannelCount, image.getDataPtr(), 90);
      break;

    case FileFormat::PNG:
      result = stbi_write_png(fileStr.c_str(), imgWidth, imgHeight, imgChannelCount, image.getDataPtr(), imgWidth * imgChannelCount);
      break;

    case FileFormat::TGA:
      result = stbi_write_tga(fileStr.c_str(), imgWidth, imgHeight, imgChannelCount, image.getDataPtr());
      break;

    case FileFormat::UNKNOWN:
    default:
      throw std::invalid_argument(std::format("[ImageFormat] Unsupported image file extension '{}' for saving", fileExt));
  }

  if (result == 0)
    throw std::invalid_argument("[ImageFormat] Failed to save image.");

  Logger::debug("[ImageFormat] Saved image");
}

} // namespace Raz::ImageFormat
