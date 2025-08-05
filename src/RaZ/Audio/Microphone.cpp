#include "RaZ/Audio/Microphone.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <stdexcept>
#include <string>

namespace Raz {

namespace {

constexpr const char* recoverAlcErrorStr(int errorCode) {
  switch (errorCode) {
    case ALC_INVALID_DEVICE:  return "Invalid device";
    case ALC_INVALID_CONTEXT: return "Invalid context";
    case ALC_INVALID_ENUM:    return "Invalid enum";
    case ALC_INVALID_VALUE:   return "Invalid value";
    case ALC_OUT_OF_MEMORY:   return "Out of memory";
    case ALC_NO_ERROR:        return "No error";
    default:                  return "Unknown error";
  }
}

inline void checkError(void* device, const std::string& errorMsg) {
  const int errorCode = alcGetError(static_cast<ALCdevice*>(device));

  if (errorCode != ALC_NO_ERROR)
    Logger::error("[OpenAL] " + errorMsg + " (" + recoverAlcErrorStr(errorCode) + ')');
}

constexpr int recoverFrameSize(AudioFormat format) {
  uint8_t channelCount {};
  uint8_t bitCount {};

  switch (format) {
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
      throw std::invalid_argument("[Microphone] Unhandled audio format");
  }

  switch (format) {
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
      throw std::invalid_argument("[Microphone] Unhandled audio format");
  }

  return channelCount * bitCount / 8;
}

} // namespace

Microphone::Microphone(AudioFormat format, unsigned int frequency, float duration, const std::string& deviceName) {
  ZoneScopedN("Microphone::Microphone");
  openDevice(format, frequency, duration, deviceName);
}

std::vector<std::string> Microphone::recoverDevices() {
  if (!alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT")) // If the needed extension is unsupported, return an empty vector
    return {};

  std::vector<std::string> devices;

  // This recovers all devices' names in a single string, each name separated by a null character ('\0'), and ending with two of those
  // For example: "First device\0Second device\0Third device\0\0"
  const char* devicesNames = alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);

  while (devicesNames[0] != '\0') {
    devices.emplace_back(devicesNames); // This automatically fills the string until the first \0
    devicesNames += devices.back().size() + 1;
  }

  return devices;
}

void Microphone::openDevice(AudioFormat format, unsigned int frequency, float duration, const std::string& deviceName) {
  ZoneScopedN("Microphone::openDevice");

  Logger::debug("[Microphone] Opening capture " + (!deviceName.empty() ? + "device '" + deviceName + '\'' : "default device") + "...");

  destroy();

  if ((format == AudioFormat::MONO_F32 || format == AudioFormat::STEREO_F32) && !alIsExtensionPresent("AL_EXT_float32")) {
    Logger::error("[Microphone] Float audio capture format is not supported by the audio driver");
    return;
  }

  if ((format == AudioFormat::MONO_F64 || format == AudioFormat::STEREO_F64) && !alIsExtensionPresent("AL_EXT_double")) {
    Logger::error("[Microphone] Double audio capture format is not supported by the audio driver");
    return;
  }

  m_device = alcCaptureOpenDevice((!deviceName.empty() ? deviceName.c_str() : nullptr), frequency,
                                  static_cast<int>(format), static_cast<int>(duration * static_cast<float>(frequency)));
  if (!m_device) {
    Logger::error("[OpenAL] Failed to open an audio capture device");
    return;
  }

  m_format    = format;
  m_frequency = frequency;

  Logger::debug("[Microphone] Opened capture device '" + recoverCurrentDevice() + '\'');
}

std::string Microphone::recoverCurrentDevice() const {
  if (m_device == nullptr) // The system has failed to initialize; returning an empty device name
    return {};

  if (!alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT")) // If the needed extension is unsupported, return an empty string
    return {};

  return alcGetString(static_cast<ALCdevice*>(m_device), ALC_CAPTURE_DEVICE_SPECIFIER);
}

void Microphone::start() const noexcept {
  alcCaptureStart(static_cast<ALCdevice*>(m_device));
}

void Microphone::stop() const noexcept {
  alcCaptureStop(static_cast<ALCdevice*>(m_device));
}

int Microphone::recoverAvailableSampleCount() const noexcept {
  if (m_device == nullptr)
    return 0;

  int count {};
  alcGetIntegerv(static_cast<ALCdevice*>(m_device), ALC_CAPTURE_SAMPLES, 1, &count);

  return count;
}

float Microphone::recoverAvailableDuration() const noexcept {
  return (static_cast<float>(recoverAvailableSampleCount()) / static_cast<float>(m_frequency));
}

AudioData Microphone::recoverData(float maxDuration) const {
  AudioData data {};
  recoverData(data, maxDuration);

  return data;
}

void Microphone::recoverData(AudioData& data, float maxDuration) const {
  ZoneScopedN("Microphone::recoverData");

  data.format    = m_format;
  data.frequency = m_frequency;
  data.buffer.clear();

  if (maxDuration == 0.f)
    return;

  int sampleCount = recoverAvailableSampleCount();

  if (sampleCount <= 0)
    return;

  if (maxDuration > 0.f)
    sampleCount = std::min(sampleCount, static_cast<int>(maxDuration * static_cast<float>(m_frequency)));

  data.buffer.resize(recoverFrameSize(m_format) * sampleCount);

  alcCaptureSamples(static_cast<ALCdevice*>(m_device), data.buffer.data(), sampleCount);
  checkError(m_device, "Failed to recover captured data");
}

void Microphone::destroy() {
  ZoneScopedN("Microphone::destroy");

  if (m_device == nullptr)
    return;

  Logger::debug("[Microphone] Destroying...");

  if (!alcCaptureCloseDevice(static_cast<ALCdevice*>(m_device)))
    Logger::error("[OpenAL] Failed to close the audio capture device");

  m_device = nullptr;

  Logger::debug("[Microphone] Destroyed");
}

} // namespace Raz
