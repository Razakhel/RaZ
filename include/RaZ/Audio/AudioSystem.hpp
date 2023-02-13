#pragma once

#ifndef RAZ_AUDIOSYSTEM_HPP
#define RAZ_AUDIOSYSTEM_HPP

#include "RaZ/System.hpp"

namespace Raz {

enum class AudioFormat : int {
  MONO_U8    = 4352  /* AL_FORMAT_MONO8             */, ///< Mono format on 8 unsigned bits (0 to 255).
  STEREO_U8  = 4353  /* AL_FORMAT_STEREO8           */, ///< Stereo format on 8 unsigned bits (0 to 255).
  MONO_I16   = 4354  /* AL_FORMAT_MONO16            */, ///< Mono format on 16 signed bits (-32768 to 32767).
  STEREO_I16 = 4355  /* AL_FORMAT_STEREO16          */, ///< Stereo format on 16 signed bits (-32768 to 32767).
  MONO_F32   = 65552 /* AL_FORMAT_MONO_FLOAT32      */, ///< Mono format on 32 floating-point bits (float).
  STEREO_F32 = 65553 /* AL_FORMAT_STEREO_FLOAT32    */, ///< Stereo format on 32 floating-point bits (float).
  MONO_F64   = 65554 /* AL_FORMAT_MONO_DOUBLE_EXT   */, ///< Mono format on 64 floating-point bits (double).
  STEREO_F64 = 65555 /* AL_FORMAT_STEREO_DOUBLE_EXT */  ///< Stereo format on 64 floating-point bits (double).
};

class AudioSystem final : public System {
public:
  /// Creates a system handling audio.
  /// \param deviceName Name of the audio device to open; nullptr to use the default one.
  /// \see recoverDevices()
  explicit AudioSystem(const char* deviceName = nullptr);

  /// Recovers the names of all available audio devices.
  /// \return List of the available devices' names, or an empty list if the required extension is unsupported.
  static std::vector<std::string> recoverDevices();

  /// Opens the audio device corresponding to the given name.
  /// \param deviceName Name of the device to open, or nullptr to open the default one.
  /// \see recoverDevices()
  void openDevice(const char* deviceName);
  /// Recovers the name of the current audio device.
  /// \return Current device's name, or an empty string if the required extension is unsupported.
  std::string recoverCurrentDevice() const;
  bool update(float deltaTime) override;
  void destroy() override;

  ~AudioSystem() override { destroy(); }

private:
  void* m_device {};
  void* m_context {};
};

} // namespace Raz

#endif // RAZ_AUDIOSYSTEM_HPP
