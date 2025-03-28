#include "RaZ/Audio/AudioData.hpp"
#include "RaZ/Audio/AudioUtils.hpp"

#include "tracy/Tracy.hpp"

#include <array>
#include <stdexcept>

namespace Raz::AudioUtils {

namespace {

constexpr int16_t fromLittleEndian(uint8_t byte1, uint8_t byte2) {
  return static_cast<int16_t>((byte1 << 0u) | (byte2 << 8u));
}

constexpr std::array<uint8_t, 2> toLittleEndian16(int16_t val) {
  return { static_cast<uint8_t>(val & 0xFFu), static_cast<uint8_t>(val >> 8u) };
}

constexpr std::array<uint8_t, 2> computeMonoI16(std::array<uint8_t, 2> leftSampleBytes, std::array<uint8_t, 2> rightSampleBytes) {
  const int16_t leftSample  = fromLittleEndian(leftSampleBytes[0], leftSampleBytes[1]);
  const int16_t rightSample = fromLittleEndian(rightSampleBytes[0], rightSampleBytes[1]);
  const auto mergedSample   = static_cast<int32_t>(leftSample) + static_cast<int32_t>(rightSample);

  return toLittleEndian16(static_cast<int16_t>(mergedSample / 2));
}

} // namespace

void convertToMono(AudioData& audioData) {
  ZoneScopedN("AudioUtils::convertToMono");

  const AudioFormat origFormat = audioData.format;

  if (origFormat == AudioFormat::MONO_U8 || origFormat == AudioFormat::MONO_I16 || origFormat == AudioFormat::MONO_F32 || origFormat == AudioFormat::MONO_F64)
    return;

  if (origFormat == AudioFormat::STEREO_F32 || origFormat == AudioFormat::STEREO_F64)
    throw std::invalid_argument("[AudioUtils] Mono conversion currently unsupported for floating-point formats");

  uint8_t bytesPerSample {};

  switch (origFormat) {
    case AudioFormat::STEREO_U8:
      bytesPerSample   = 1;
      audioData.format = AudioFormat::MONO_U8;
      break;

    case AudioFormat::STEREO_I16:
      bytesPerSample   = 2;
      audioData.format = AudioFormat::MONO_I16;
      break;

    //case AudioFormat::STEREO_F32:
    //  bytesPerSample   = 4;
    //  audioData.format = AudioFormat::MONO_F32;
    //  break;

    //case AudioFormat::STEREO_F64:
    //  bytesPerSample   = 8;
    //  audioData.format = AudioFormat::MONO_F64;
    //  break;

    default:
      throw std::invalid_argument("[AudioData] Unexpected format to convert to mono");
  }

  std::vector<uint8_t> monoBuffer(audioData.buffer.size() / 2);

  for (std::size_t valIndex = 0; valIndex < audioData.buffer.size(); valIndex += bytesPerSample * 2) {
    const std::size_t monoIndex = valIndex / 2;

    if (bytesPerSample == 1) {
      monoBuffer[monoIndex] = static_cast<uint8_t>((audioData.buffer[valIndex] + audioData.buffer[valIndex + 1]) / 2);
    } else if (bytesPerSample == 2) {
      const std::array<uint8_t, 2> monoSample = computeMonoI16({ audioData.buffer[valIndex], audioData.buffer[valIndex + 1] },
                                                               { audioData.buffer[valIndex + 2], audioData.buffer[valIndex + 3] });
      monoBuffer[monoIndex]     = monoSample[0];
      monoBuffer[monoIndex + 1] = monoSample[1];
    }
  }

  audioData.buffer = std::move(monoBuffer);
}

} // namespace Raz::AudioUtils
