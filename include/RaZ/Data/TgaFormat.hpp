#pragma once

#ifndef RAZ_TGAFORMAT_HPP
#define RAZ_TGAFORMAT_HPP

namespace Raz {

class FilePath;
class Image;

namespace TgaFormat {

/// Loads an image from a TGA file.
/// \param filePath File from which to load the image.
/// \param flipVertically Flip vertically the image when loading.
/// \return Loaded image's data.
Image load(const FilePath& filePath, bool flipVertically = false);

} // namespace TgaFormat

} // namespace Raz

#endif // RAZ_TGAFORMAT_HPP
