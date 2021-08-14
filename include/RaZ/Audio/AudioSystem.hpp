#pragma once

#ifndef RAZ_AUDIOSYSTEM_HPP
#define RAZ_AUDIOSYSTEM_HPP

#include "RaZ/System.hpp"

namespace Raz {

class AudioSystem final : public System {
public:
  /// Creates a system handling audio.
  /// \param deviceName Name of the audio device to open; nullptr to use the default one.
  /// \see recoverDevices()
  explicit AudioSystem(const char* deviceName = nullptr);

  /// Opens the audio device corresponding to the given name.
  /// \param deviceName Name of the device to open, or nullptr to open the default one.
  /// \see recoverDevices()
  void openDevice(const char* deviceName);
  /// Recovers the name of the current audio device.
  /// \return Current device's name, or an empty string if the required extension is unsupported.
  std::string recoverCurrentDevice() const;
  bool update(float deltaTime) override;
  void destroy() override;

  /// Recovers the names of all available audio devices.
  /// \return List of the available devices' names, or an empty list if the required extension is unsupported.
  static std::vector<std::string> recoverDevices();

  ~AudioSystem() override { destroy(); }

private:
  void* m_device {};
  void* m_context {};
};

} // namespace Raz

#endif // RAZ_AUDIOSYSTEM_HPP
