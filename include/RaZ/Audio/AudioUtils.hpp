#pragma once

#ifndef RAZ_AUDIOUTILS_HPP
#define RAZ_AUDIOUTILS_HPP

namespace Raz {

struct AudioData;

namespace AudioUtils {

/// Transforms audio data to be mono by averaging channels. Does nothing if the input is already mono.
/// \param audioData Data to be converted.
void convertToMono(AudioData& audioData);

} // namespace AudioUtils
} // namespace Raz

#endif // RAZ_AUDIOUTILS_HPP
