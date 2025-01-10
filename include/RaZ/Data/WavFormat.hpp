#pragma once

#ifndef RAZ_WAVFORMAT_HPP
#define RAZ_WAVFORMAT_HPP

namespace Raz {

struct AudioData;
class FilePath;

namespace WavFormat {

/// Loads audio data from a [WAV](https://en.wikipedia.org/wiki/WAV) file.
/// \param filePath File from which to load the audio.
/// \return Imported audio data.
AudioData load(const FilePath& filePath);

/// Saves audio data to a [WAV](https://en.wikipedia.org/wiki/WAV) file.
/// \param filePath File to which to save the sound.
/// \param data Audio data to export.
void save(const FilePath& filePath, const AudioData& data);

} // namespace WavFormat

} // namespace Raz

#endif // RAZ_WAVFORMAT_HPP
