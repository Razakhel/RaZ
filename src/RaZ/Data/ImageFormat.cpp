#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/PngFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STBI_WINDOWS_UTF8
#include "stb_image.h"

namespace Raz::ImageFormat {

namespace {

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

} // namespace

Image load(const FilePath& filePath, bool flipVertically) {
  Logger::debug("[ImageFormat] Loading image '" + filePath + "'...");

  const std::string fileStr = filePath.toUtf8();
  const bool isHdr = stbi_is_hdr(fileStr.c_str());

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
    throw std::invalid_argument("[ImageFormat] Cannot load image '" + filePath + "': " + stbi_failure_reason());

  const std::size_t valueCount = width * height * channelCount;

  Image img(width, height, recoverColorspace(channelCount), (isHdr ? ImageDataType::FLOAT : ImageDataType::BYTE));

  if (isHdr)
    std::copy_n(static_cast<float*>(data.get()), valueCount, static_cast<float*>(img.getDataPtr()));
  else
    std::copy_n(static_cast<uint8_t*>(data.get()), valueCount, static_cast<uint8_t*>(img.getDataPtr()));

  Logger::debug("[ImageFormat] Loaded image");

  return img;
}

void save(const FilePath& filePath, const Image& image, bool flipVertically) {
  Logger::debug("[ImageFormat] Saving image to '" + filePath + "'...");

  const std::string fileExt = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (fileExt == "png")
    PngFormat::save(filePath, image, flipVertically);
  else
    throw std::invalid_argument("[ImageFormat] Unsupported image file extension '" + fileExt + "' for saving.");

  Logger::debug("[ImageFormat] Saved image");
}

} // namespace Raz::ImageFormat
