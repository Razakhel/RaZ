#include "RaZ/Audio/AudioData.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

#include <array>
#include <fstream>
#include <string>

namespace Raz::WavFormat {

namespace {

constexpr uint32_t fromLittleEndian(const std::array<uint8_t, 4>& bytes) {
  return static_cast<uint32_t>((bytes[0] << 0u) | (bytes[1] << 8u) | (bytes[2] << 16u) | (bytes[3] << 24u));
}

constexpr uint16_t fromLittleEndian(uint8_t byte1, uint8_t byte2) {
  return static_cast<uint16_t>((byte1 << 0u) | (byte2 << 8u));
}

struct WavInfo {
  bool isValid = false;

  uint32_t fileSize {};
  uint32_t formatBlockSize {};
  uint16_t audioFormat {};
  uint16_t channelCount {};
  uint32_t frequency {};
  uint32_t bytesPerSecond {};
  uint16_t bytesPerBlock {};
  uint16_t bitsPerSample {};
  uint32_t dataSize {};
};

inline WavInfo validateWav(std::ifstream& file) {
  WavInfo info {};

  std::array<uint8_t, 4> bytes {};

  ////////////
  // Header //
  ////////////

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // 'RIFF'
  if (bytes[0] != 'R' && bytes[1] != 'I' && bytes[2] != 'F' && bytes[3] != 'F')
    return info;

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // File size - 8
  info.fileSize = fromLittleEndian(bytes); // Values are in little-endian; they must be converted

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // 'WAVE'
  if (bytes[0] != 'W' && bytes[1] != 'A' && bytes[2] != 'V' && bytes[3] != 'E')
    return info;

  //////////////////
  // Audio format //
  //////////////////

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // "fmt "
  if (bytes[0] != 'f' && bytes[1] != 'm' && bytes[2] != 't' && bytes[3] != ' ')
    return info;

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // Format block size - 16
  info.formatBlockSize = fromLittleEndian(bytes);

  file.read(reinterpret_cast<char*>(bytes.data()), 2); // Audio format
  info.audioFormat = fromLittleEndian(bytes[0], bytes[1]);
  // 0: Unknown
  // 1: PCM (uncompressed)
  // 2: Microsoft ADPCM
  // 3: IEEE float
  // 6: 8-bit ITU-T G.711 A-law
  // 7: 8-bit ITU-T G.711 µ-law
  // 17: IMA ADPCM
  // 20: ITU G.723 ADPCM (Yamaha)
  // 49: GSM 6.10
  // 64: ITU G.721 ADPCM
  // 80: MPEG

  if (info.audioFormat != 1)
    Logger::warn("[WavLoad] Only WAV files with a PCM format are supported.");

  file.read(reinterpret_cast<char*>(bytes.data()), 2); // Channel count
  info.channelCount = fromLittleEndian(bytes[0], bytes[1]);
  // 1 channel:  mono
  // 2 channels: stereo
  // 3 channels: left, center & right
  // 4 channels: front left, front right, back left & back right
  // 5 channels: left, center, right & surround
  // 6 channels: left, center left, center, center right, right & surround

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // Sampling frequency
  info.frequency = fromLittleEndian(bytes);

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // Bytes per second (frequency * bytes per block)
  info.bytesPerSecond = fromLittleEndian(bytes);

  file.read(reinterpret_cast<char*>(bytes.data()), 2); // Bytes per block (bits per sample / 8 * channel count)
  info.bytesPerBlock = fromLittleEndian(bytes[0], bytes[1]);

  file.read(reinterpret_cast<char*>(bytes.data()), 2); // Bits per sample (bit depth)
  info.bitsPerSample = fromLittleEndian(bytes[0], bytes[1]);

  ////////////////
  // Data block //
  ////////////////

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // Supposed to be 'data'

  // Additional chunks can be present (such as 'cue ', 'LIST', 'bext' and others), which aren't supported there. They must be skipped
  // See:
  // - https://en.wikipedia.org/wiki/WAV#RIFF_WAVE
  // - https://en.wikipedia.org/wiki/Broadcast_Wave_Format#Details
  // - https://stackoverflow.com/a/76137824/3292304
  while (bytes[0] != 'd' || bytes[1] != 'a' || bytes[2] != 't' || bytes[3] != 'a') {
    file.read(reinterpret_cast<char*>(bytes.data()), 4); // Reading the chunk size

    const uint32_t chunkSize = fromLittleEndian(bytes);
    file.ignore(chunkSize);

    file.read(reinterpret_cast<char*>(bytes.data()), 4);
  }

  file.read(reinterpret_cast<char*>(bytes.data()), 4); // Data size (file size - header size (theoretically 44 bytes))
  info.dataSize = fromLittleEndian(bytes);

  info.isValid = true;
  return info;
}

} // namespace

AudioData load(const FilePath& filePath) {
  ZoneScopedN("WavFormat::load");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  Logger::debug("[WavLoad] Loading WAV file ('" + filePath + "')...");

  std::ifstream file(filePath, std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("[WavLoad] Could not open the WAV file '" + filePath + "'");

  const WavInfo info = validateWav(file);

  if (!info.isValid)
    throw std::runtime_error("[WavLoad] '" + filePath + "' is not a valid WAV audio file");

  AudioData audioData {};

  // Determining the right audio format
  switch (info.bitsPerSample) {
    case 8:
      if (info.channelCount == 1)
        audioData.format = AudioFormat::MONO_U8;
      else if (info.channelCount == 2)
        audioData.format = AudioFormat::STEREO_U8;
      break;

    case 16:
      if (info.channelCount == 1)
        audioData.format = AudioFormat::MONO_I16;
      else if (info.channelCount == 2)
        audioData.format = AudioFormat::STEREO_I16;
      break;

    case 32:
      if (info.channelCount == 1)
        audioData.format = AudioFormat::MONO_F32;
      else if (info.channelCount == 2)
        audioData.format = AudioFormat::STEREO_F32;
      break;

    case 64:
      if (info.channelCount == 1)
        audioData.format = AudioFormat::MONO_F64;
      else if (info.channelCount == 2)
        audioData.format = AudioFormat::STEREO_F64;
      break;

    default:
      throw std::runtime_error("[WavLoad] " + std::to_string(info.bitsPerSample) + " bits WAV files are unsupported");
  }

  // If the format is still unassigned, it is invalid
  if (static_cast<int>(audioData.format) == 0)
    throw std::runtime_error("[WavLoad] Unsupported WAV channel count");

  audioData.frequency = static_cast<int>(info.frequency);

  // Reading the actual audio data from the file
  audioData.buffer.resize(info.dataSize);
  file.read(reinterpret_cast<char*>(audioData.buffer.data()), static_cast<std::streamsize>(audioData.buffer.size()));

  Logger::debug("[WavLoad] Loaded WAV file");

  return audioData;
}

} // namespace Raz::WavFormat
