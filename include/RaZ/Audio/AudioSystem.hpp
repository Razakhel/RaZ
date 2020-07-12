#pragma once

#ifndef RAZ_AUDIOSYSTEM_HPP
#define RAZ_AUDIOSYSTEM_HPP

#include "RaZ/System.hpp"

#include <AL/alc.h>

namespace Raz {

class AudioSystem final : public System {
public:
  /// Creates a system handling audio.
  /// \param deviceName Name of the audio device to open; nullptr to use the default one.
  /// \see recoverDevices()
  explicit AudioSystem(const char* deviceName = nullptr);

  std::string recoverCurrentDevice() const;
  bool update(float deltaTime) override;

  /// Recover the names of the available audio devices.
  /// \return List of the available devices' names.
  static std::vector<std::string> recoverDevices();

  ~AudioSystem() override;

private:
  ALCdevice* m_device;
  ALCcontext* m_context;
};

} // namespace Raz

#endif // RAZ_AUDIOSYSTEM_HPP
