#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <iostream>
#include <string_view>

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

inline void checkError(ALCdevice* device, const std::string_view& errorMsg) {
  const int errorCode = alcGetError(device);

  if (errorCode != ALC_NO_ERROR)
    std::cerr << "[OpenAL] Error: " << errorMsg << " (" << recoverAlcErrorStr(errorCode) << ")." << std::endl;
}

} // namespace

AudioSystem::AudioSystem(const char* deviceName) {
  m_acceptedComponents.setBit(Component::getId<Sound>());
  m_acceptedComponents.setBit(Component::getId<Listener>());

  openDevice(deviceName);
}

void AudioSystem::openDevice(const char* deviceName) {
  destroy();

  m_device = alcOpenDevice(deviceName);
  if (!m_device)
    std::cerr << "[OpenAL] Error: Failed to open an audio device." << std::endl;

  m_context = alcCreateContext(m_device, nullptr);
  checkError(m_device, "Failed to create context");

  if (!alcMakeContextCurrent(m_context)) {
    std::cerr << "[OpenAL] Error: Failed to make the audio context current." << std::endl;
    alcGetError(m_device); // Flushing errors, since alcMakeContextCurrent() produces one on failure, which we already handled
  }
}

std::string AudioSystem::recoverCurrentDevice() const {
  if (m_device == nullptr) // The system has failed to initialize; returning an empty device name
    return {};

  if (!alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT")) // If the needed extension is unsupported, return an empty string
    return {};

  return alcGetString(m_device, ALC_ALL_DEVICES_SPECIFIER);
}

bool AudioSystem::update(float /* deltaTime */) {
#if defined(RAZ_CONFIG_DEBUG)
  // Checking that only one Listener exists
  bool hasOneListener = false;
#endif

  for (Entity* entity : m_entities) {
    if (entity->hasComponent<Sound>()) {
      const Sound& sound = entity->getComponent<Sound>();

      if (entity->hasComponent<Transform>()) {
        auto& soundTrans = entity->getComponent<Transform>();

        // TODO: Transform's update status may be reinitialized in the RenderSystem (and should theoretically be reset in every system, including here)
        //  A viable solution must be implemented to check for and reset this status in all systems
        if (soundTrans.hasUpdated()) {
          sound.setPosition(soundTrans.getPosition());
          //soundTrans.setUpdated(false);
        }
      }
    }

    if (entity->hasComponent<Listener>()) {
#if defined(RAZ_CONFIG_DEBUG)
      assert("Error: Only one Listener component must exist in an AudioSystem." && !hasOneListener);
      hasOneListener = true;
#endif

      assert("Error: A Listener entity must have a Transform component." && entity->hasComponent<Transform>());

      auto& listener      = entity->getComponent<Listener>();
      auto& listenerTrans = entity->getComponent<Transform>();

      if (listenerTrans.hasUpdated()) {
        listener.setPosition(listenerTrans.getPosition());
        listener.setOrientation(Mat3f(listenerTrans.computeTransformMatrix()));

        //listenerTrans.setUpdated(false);
      }
    }
  }

  return true;
}

void AudioSystem::destroy() {
  for (Entity* entity : m_entities) {
    if (entity->hasComponent<Sound>())
      entity->getComponent<Sound>().destroy();
  }

  alcMakeContextCurrent(nullptr);

  if (m_context != nullptr) {
    alcDestroyContext(m_context);
    checkError(m_device, "Failed to destroy context");
    m_context = nullptr;
  }

  if (m_device != nullptr) {
    if (!alcCloseDevice(m_device))
      std::cerr << "[OpenAL] Error: Failed to close the audio device." << std::endl;
    m_device = nullptr;
  }
}

std::vector<std::string> AudioSystem::recoverDevices() {
  if (!alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT")) // If the needed extension is unsupported, return an empty vector
    return {};

  std::vector<std::string> devices;

  // This recovers all devices' names in a single string, each name separated by a null character ('\0'), and ending with two of those
  // For example: "First device\0Second device\0Third device\0\0"
  const char* devicesNames = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);

  while (devicesNames[0] != '\0') {
    devices.emplace_back(devicesNames); // This automatically fills the string until the first \0
    devicesNames += devices.back().size() + 1;
  }

  return devices;
}

} // namespace Raz
