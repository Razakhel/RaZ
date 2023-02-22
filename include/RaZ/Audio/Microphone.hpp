#pragma once

#ifndef RAZ_MICROPHONE_HPP
#define RAZ_MICROPHONE_HPP

#include "RaZ/Audio/AudioSystem.hpp"

namespace Raz {

class Sound;

class Microphone {
public:
  /// Creates a microphone object handling audio capture.
  /// \param format Format in which to record.
  /// \param frequency Sampling frequency in which to record, in Hertz.
  /// \param duration Amount of time to record, in seconds. This is a minimum; the actual available duration may exceed this value.
  /// \param deviceName Name of the audio capture device to open, or nullptr to use the default one.
  /// \see Microphone::recoverDevices()
  Microphone(AudioFormat format, unsigned int frequency, float duration, const char* deviceName = nullptr) { openDevice(format, frequency,
                                                                                                                        duration, deviceName); }
  Microphone(const Microphone&) = delete;
  Microphone(Microphone&&) = delete;

  /// Recovers the names of all available audio capture devices.
  /// \return List of the available devices' names, or an empty list if the required extension is unsupported.
  static std::vector<std::string> recoverDevices();

  /// Opens the audio capture device corresponding to the given name.
  /// \param format Format in which to record.
  /// \param frequency Sampling frequency in which to record, in Hertz.
  /// \param duration Amount of time to record, in seconds. This is a minimum; the actual available duration may exceed this value.
  /// \param deviceName Name of the audio capture device to open, or nullptr to open the default one.
  /// \see Microphone::recoverDevices()
  void openDevice(AudioFormat format, unsigned int frequency, float duration, const char* deviceName = nullptr);
  /// Recovers the name of the current audio capture device.
  /// \return The current device's name, or an empty string if the required extension is unsupported.
  std::string recoverCurrentDevice() const;
  /// Starts capturing audio.
  void start() const noexcept;
  /// Stops capturing audio.
  void stop() const noexcept;
  /// Recovers the amount of currently captured samples.
  /// \return Available captured sample count.
  int recoverAvailableSampleCount() const noexcept;
  /// Recovers the amount of currently captured time.
  /// \return Available captured duration, in seconds.
  float recoverAvailableDuration() const noexcept;
  /// Recovers captured samples.
  /// \note This flushes the recovered captured data; if recovering something, the available sample count right after this call will be less than it was before.
  /// \param maxDuration Maximum amount of time to recover, in seconds. Giving a negative value will result in recovering all available samples.
  /// \return Captured samples.
  std::vector<uint8_t> recoverData(float maxDuration = -1.f) const;
  /// Recovers captured samples as a Sound object.
  /// \note This flushes the recovered captured data; if recovering something, the available sample count right after this call will be less than it was before.
  /// \param maxDuration Maximum amount of time to recover, in seconds. Giving a negative value will result in recovering all available samples.
  /// \return Captured sound.
  Sound recoverSound(float maxDuration = -1.f) const;

  Microphone& operator=(const Microphone&) = delete;
  Microphone& operator=(Microphone&&) = delete;

  ~Microphone() { destroy(); }

private:
  /// Destroys the microphone.
  void destroy();

  void* m_device {};

  AudioFormat m_format {};
  unsigned int m_frequency {};
};

} // namespace Raz

#endif // RAZ_MICROPHONE_HPP
