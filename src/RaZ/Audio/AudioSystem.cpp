#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <iostream>

namespace Raz {

AudioSystem::AudioSystem(const char* deviceName) {
  m_acceptedComponents.setBit(Component::getId<Sound>());

  m_device = alcOpenDevice(deviceName);
  if (!m_device)
    std::cerr << "Error: Failed to open an audio device." << std::endl;

  m_context = alcCreateContext(m_device, nullptr);
  if (!alcMakeContextCurrent(m_context))
    std::cerr << "Error: Failed to make the audio context current." << std::endl;
}

bool AudioSystem::update(float /* deltaTime */) {
  for (const Entity* entity : m_entities) {
    if (entity->hasComponent<Sound>()) {
      const Sound& sound = entity->getComponent<Sound>();

      if (!sound.isPaused() && !sound.isStopped())
        sound.play();
    }
  }

  return true;
}

std::string AudioSystem::recoverCurrentDevice() const {
  if (m_device == nullptr) // The system has failed to initialize; returning an empty device name
    return {};

  return alcGetString(m_device, ALC_DEVICE_SPECIFIER);
}

std::vector<std::string> AudioSystem::recoverDevices() {
  std::vector<std::string> devices;

  // This recovers all devices' names in a single string, each name separated by a null character ('\0'), and ending with two of those
  // For example: "First device\0Second device\0Third device\0\0"
  const char* devicesNames = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

  while (devicesNames[0] != '\0') {
    devices.emplace_back(devicesNames); // This automatically fills the string until the first \0
    devicesNames += devices.back().size() + 1;
  }

  return devices;
}

AudioSystem::~AudioSystem() {
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_context);
  alcCloseDevice(m_device);
}

} // namespace Raz
