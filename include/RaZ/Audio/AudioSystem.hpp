#pragma once

#ifndef RAZ_AUDIOSYSTEM_HPP
#define RAZ_AUDIOSYSTEM_HPP

#include "RaZ/System.hpp"

#include <string>

namespace Raz {

class AudioSystem final : public System {
public:
  /// Creates a system handling audio.
  /// \param deviceName Name of the audio device to open; if empty, will use the default one.
  /// \see AudioSystem::recoverDevices()
  explicit AudioSystem(const std::string& deviceName = {});

  /// Recovers the names of all available audio devices.
  /// \return List of the available devices' names, or an empty list if the required extension is unsupported.
  static std::vector<std::string> recoverDevices();

  /// Opens the audio device corresponding to the given name.
  /// \param deviceName Name of the audio device to open; if empty, will use the default one.
  /// \see AudioSystem::recoverDevices()
  void openDevice(const std::string& deviceName = {});
  /// Recovers the name of the current audio device.
  /// \return The current device's name, or an empty string if the required extension is unsupported.
  std::string recoverCurrentDevice() const;
  bool update(const FrameTimeInfo& timeInfo) override;
  void destroy() override;

  ~AudioSystem() override { destroy(); }

private:
  void* m_device {};
  void* m_context {};
};

} // namespace Raz

#endif // RAZ_AUDIOSYSTEM_HPP
