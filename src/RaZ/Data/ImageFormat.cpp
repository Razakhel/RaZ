#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/PngFormat.hpp"
#include "RaZ/Data/TgaFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

namespace Raz::ImageFormat {

Image load(const FilePath& filePath, bool flipVertically) {
  const std::string fileExt = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (fileExt == "png")
    return PngFormat::load(filePath, flipVertically);
  else if (fileExt == "tga")
    return TgaFormat::load(filePath, flipVertically);

  throw std::invalid_argument("[ImageFormat] Unsupported image file extension '" + fileExt + "' for loading.");
}

void save(const FilePath& filePath, const Image& image, bool flipVertically) {
  const std::string fileExt = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (fileExt == "png")
    PngFormat::save(filePath, image, flipVertically);
  else
    throw std::invalid_argument("[ImageFormat] Unsupported image file extension '" + fileExt + "' for saving.");
}

} // namespace Raz::ImageFormat
