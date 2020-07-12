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
    if (entity->hasComponent<Sound>())
      entity->getComponent<Sound>().play();
  }

  return true;
}

std::string AudioSystem::recoverCurrentDevice() const {
  return alcGetString(m_device, ALC_DEVICE_SPECIFIER);
}

std::vector<std::string> AudioSystem::recoverDevices() {
  const char* devicesNames = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
  std::vector<std::string> devices;

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
