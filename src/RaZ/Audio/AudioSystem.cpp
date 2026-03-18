#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/RigidBody.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

#include <AL/al.h>
#include <AL/alc.h>

namespace Raz {

namespace {

constexpr std::string_view recoverAlcErrorStr(int errorCode) {
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

inline void checkError(void* device, std::string_view errorMsg) {
  if (const int errorCode = alcGetError(static_cast<ALCdevice*>(device)); errorCode != ALC_NO_ERROR)
    Logger::error("[OpenAL] {} ({})", errorMsg, recoverAlcErrorStr(errorCode));
}

} // namespace

AudioSystem::AudioSystem(const std::string& deviceName) {
  ZoneScopedN("AudioSystem::AudioSystem");

  registerComponents<Sound, Listener>();
  openDevice(deviceName);

  if (m_device == nullptr || m_context == nullptr)
    return;

  const std::string_view alRenderer = alGetString(AL_RENDERER);

  Logger::debug("[AudioSystem] OpenAL renderer: {}", alRenderer);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN) // Emscripten has its own implementation with some OpenAL Soft extensions
  if (alRenderer != "OpenAL Soft")
    Logger::warn("[OpenAL] Standard OpenAL detected; make sure to use OpenAL Soft to get all possible features");
#endif
}

std::vector<std::string> AudioSystem::recoverDevices() {
  if (!alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT"))
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

void AudioSystem::openDevice(const std::string& deviceName) {
  ZoneScopedN("AudioSystem::openDevice");

  Logger::debug("[AudioSystem] Opening {}...", (!deviceName.empty() ? std::format("device '{}'", deviceName) : "default device"));

  destroy();

  m_device = alcOpenDevice((!deviceName.empty() ? deviceName.c_str() : nullptr));
  if (!m_device) {
    Logger::error("[OpenAL] Failed to open an audio device");
    return;
  }

  m_context = alcCreateContext(static_cast<ALCdevice*>(m_device), nullptr);
  checkError(m_device, "Failed to create context");

  if (!alcMakeContextCurrent(static_cast<ALCcontext*>(m_context))) {
    Logger::error("[OpenAL] Failed to make the audio context current");
    alcGetError(static_cast<ALCdevice*>(m_device)); // Flushing errors, since alcMakeContextCurrent() produces one on failure which we already handled
  }

  Logger::debug("[AudioSystem] Opened device '{}'", recoverCurrentDevice());
}

std::string AudioSystem::recoverCurrentDevice() const {
  if (m_device == nullptr) // The system has failed to initialize; returning an empty device name
    return {};

  if (!alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT"))
    return {};

  return alcGetString(static_cast<ALCdevice*>(m_device), ALC_ALL_DEVICES_SPECIFIER);
}

bool AudioSystem::update(const FrameTimeInfo&) {
  ZoneScopedN("AudioSystem::update");

  for (Entity* entity : m_entities) {
    if (entity->hasComponent<Sound>()) {
      const Sound& sound = entity->getComponent<Sound>();

      if (entity->hasComponent<Transform>()) {
        auto& soundTrans = entity->getComponent<Transform>();

        // TODO: Transform's update status may be reinitialized in the RenderSystem (and should theoretically be reset in every system, including here)
        //  A viable solution must be implemented to check for and reset this status in all systems
        //if (soundTrans.hasUpdated()) {
          sound.setPosition(soundTrans.getPosition());
          //soundTrans.setUpdated(false);
        //}
      }

      // TODO: velocity should be set only if it has been updated since last time
      if (entity->hasComponent<RigidBody>())
        sound.setVelocity(entity->getComponent<RigidBody>().getVelocity());
    }

    if (entity->hasComponent<Listener>()) {
      if (!entity->hasComponent<Transform>())
        throw std::runtime_error("[AudioSystem] A listener must have a transform component");

      auto& listener      = entity->getComponent<Listener>();
      auto& listenerTrans = entity->getComponent<Transform>();

      //if (listenerTrans.hasUpdated()) {
        listener.setPosition(listenerTrans.getPosition());
        listener.setOrientation(Mat3f(listenerTrans.getRotation().computeMatrix()));

        //listenerTrans.setUpdated(false);
      //}

      if (entity->hasComponent<RigidBody>())
        listener.setVelocity(entity->getComponent<RigidBody>().getVelocity());
    }
  }

  return true;
}

void AudioSystem::destroy() {
  ZoneScopedN("AudioSystem::destroy");

  if (m_context == nullptr && m_device == nullptr)
    return;

  Logger::debug("[AudioSystem] Destroying...");

  alcMakeContextCurrent(nullptr);

  if (m_context != nullptr) {
    alcDestroyContext(static_cast<ALCcontext*>(m_context));
    checkError(m_device, "Failed to destroy context");
    m_context = nullptr;
  }

  if (m_device != nullptr) {
    if (!alcCloseDevice(static_cast<ALCdevice*>(m_device)))
      Logger::error("[OpenAL] Failed to close the audio device");

    m_device = nullptr;
  }

  Logger::debug("[AudioSystem] Destroyed");
}

void AudioSystem::linkEntity(const EntityPtr& entity) {
  ZoneScopedN("AudioSystem::linkEntity");

  if (entity->hasComponent<Listener>()
   && std::ranges::find_if(m_entities, [] (const Entity* linkedEntity) { return linkedEntity->hasComponent<Listener>(); }) != m_entities.cend()) {
    throw std::runtime_error("[AudioSystem] Only one listener can exist in an audio system");
  }

  System::linkEntity(entity);
}

} // namespace Raz
