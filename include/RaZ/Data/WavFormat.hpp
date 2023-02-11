#pragma once

#ifndef RAZ_WAVFORMAT_HPP
#define RAZ_WAVFORMAT_HPP

namespace Raz {

class FilePath;
class Sound;

namespace WavFormat {

/// Loads a [WAV](https://en.wikipedia.org/wiki/WAV) audio from a file.
/// \param filePath File from which to load the audio.
/// \return Imported sound.
Sound load(const FilePath& filePath);

/// Saves a sound to a WAV file.
/// \param filePath File to which to save the sound.
/// \param sound Sound to export data from.
void save(const FilePath& filePath, const Sound& sound);

} // namespace WavFormat

} // namespace Raz

#endif // RAZ_WAVFORMAT_HPP
