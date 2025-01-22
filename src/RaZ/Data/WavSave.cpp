#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

#include <array>
#include <fstream>

namespace Raz::WavFormat {

namespace {

constexpr std::array<char, 4> toLittleEndian32(uint32_t val) {
  return { static_cast<char>(val & 0xFFu), static_cast<char>((val >> 8u) & 0xFFu), static_cast<char>((val >> 16u) & 0xFFu), static_cast<char>(val >> 24u) };
}

constexpr std::array<char, 2> toLittleEndian16(uint16_t val) {
  return { static_cast<char>(val & 0xFFu), static_cast<char>(val >> 8u) };
}

} // namespace

void save(const FilePath& filePath, const AudioData& data) {
  ZoneScopedN("WavFormat::save");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  Logger::debug("[WavSave] Saving WAV file ('" + filePath + "')...");

  std::ofstream file(filePath, std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("[WavSave] Unable to create a WAV file as '" + filePath + "'; path to file must exist");

  ////////////
  // Header //
  ////////////

  file << "RIFF";
  file.write(toLittleEndian32(static_cast<uint32_t>(data.buffer.size()) + 36).data(), 4); // File size - 8
  file << "WAVE";

  //////////////////
  // Audio format //
  //////////////////

  file << "fmt ";
  file.write(toLittleEndian32(16).data(), 4); // Format section size

  uint8_t bitCount {};

  switch (data.format) {
    case AudioFormat::MONO_U8:
    case AudioFormat::STEREO_U8:
      bitCount = 8;
      break;

    case AudioFormat::MONO_I16:
    case AudioFormat::STEREO_I16:
      bitCount = 16;
      break;

    case AudioFormat::MONO_F32:
    case AudioFormat::STEREO_F32:
      bitCount = 32;
      break;

    case AudioFormat::MONO_F64:
    case AudioFormat::STEREO_F64:
      bitCount = 64;
      break;

    default:
      throw std::invalid_argument("[WavSave] Unhandled audio format");
  }

  file.write(toLittleEndian16((bitCount >= 32 ? 3 : 1)).data(), 2); // Writing 1 if integer, 3 if floating-point

  uint16_t channelCount {};

  switch (data.format) {
    case AudioFormat::MONO_U8:
    case AudioFormat::MONO_I16:
    case AudioFormat::MONO_F32:
    case AudioFormat::MONO_F64:
      channelCount = 1;
      break;

    case AudioFormat::STEREO_U8:
    case AudioFormat::STEREO_I16:
    case AudioFormat::STEREO_F32:
    case AudioFormat::STEREO_F64:
      channelCount = 2;
      break;

    default:
      throw std::invalid_argument("[WavSave] Unhandled audio format");
  }

  file.write(toLittleEndian16(channelCount).data(), 2);
  file.write(toLittleEndian32(static_cast<uint32_t>(data.frequency)).data(), 4);

  const auto frameSize = static_cast<uint16_t>(bitCount / 8 * channelCount);

  file.write(toLittleEndian32(static_cast<uint32_t>(data.frequency) * frameSize).data(), 4); // Bytes per second
  file.write(toLittleEndian16(frameSize).data(), 2); // Bytes per block (bits per sample / 8 * channel count)
  file.write(toLittleEndian16(bitCount).data(), 2); // Bits per sample (bit depth)

  ////////////////
  // Data block //
  ////////////////

  file << "data";
  file.write(toLittleEndian32(static_cast<uint32_t>(data.buffer.size())).data(), 4);
  file.write(reinterpret_cast<const char*>(data.buffer.data()), static_cast<std::streamsize>(data.buffer.size()));

  Logger::debug("[WavSave] Saved WAV file");
}

} // namespace Raz::WavFormat
