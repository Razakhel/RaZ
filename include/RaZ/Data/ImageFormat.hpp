#pragma once

#ifndef RAZ_IMAGEFORMAT_HPP
#define RAZ_IMAGEFORMAT_HPP

#include <vector>

namespace Raz {

class FilePath;
class Image;

namespace ImageFormat {

/// Loads an image from a file.
/// \param filePath File from which to load the image.
/// \param flipVertically Flip vertically the image when loading.
/// \return Loaded image's data.
Image load(const FilePath& filePath, bool flipVertically = false);

/// Loads an image from a byte array.
/// \param imgData Data to be loaded as image.
/// \param flipVertically Flip vertically the image when loading.
/// \return Loaded image's data.
Image loadFromData(const std::vector<unsigned char>& imgData, bool flipVertically = false);

/// Loads an image from a byte array.
/// \param imgData Data to be loaded as image.
/// \param dataSize Size of the data to be loaded.
/// \param flipVertically Flip vertically the image when loading.
/// \return Loaded image's data.
Image loadFromData(const unsigned char* imgData, std::size_t dataSize, bool flipVertically = false);

/// Saves an image to a file.
/// \param filePath File to which to save the image.
/// \param flipVertically Flip vertically the image when saving.
/// \param image Image to export data from.
void save(const FilePath& filePath, const Image& image, bool flipVertically = false);

} // namespace ImageFormat

} // namespace Raz

#endif // RAZ_IMAGEFORMAT_HPP
