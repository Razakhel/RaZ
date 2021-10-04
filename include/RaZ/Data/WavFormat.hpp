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

} // namespace WavFormat

} // namespace Raz

#endif // RAZ_WAVFORMAT_HPP
