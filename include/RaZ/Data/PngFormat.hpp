#pragma once

#ifndef RAZ_PNGFORMAT_HPP
#define RAZ_PNGFORMAT_HPP

namespace Raz {

class FilePath;
class Image;

namespace PngFormat {

/// Loads an image from a PNG file.
/// \param filePath File from which to load the image.
/// \param flipVertically Flip vertically the image when loading.
/// \return Loaded image's data.
Image load(const FilePath& filePath, bool flipVertically = false);

/// Saves an image to a PNG file.
/// \param filePath File to which to save the image.
/// \param flipVertically Flip vertically the image when saving.
/// \param image Image to export data from.
void save(const FilePath& filePath, const Image& image, bool flipVertically = false);

} // namespace PngFormat

} // namespace Raz

#endif // RAZ_PNGFORMAT_HPP
