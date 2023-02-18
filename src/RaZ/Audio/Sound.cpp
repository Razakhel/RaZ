#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Audio/SoundEffectSlot.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <AL/al.h>
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <AL/efx.h>
#endif

namespace Raz {

namespace {

constexpr const char* recoverAlErrorStr(int errorCode) {
  switch (errorCode) {
    case AL_INVALID_NAME:      return "Invalid name";
    case AL_INVALID_ENUM:      return "Invalid enum";
    case AL_INVALID_VALUE:     return "Invalid value";
    case AL_INVALID_OPERATION: return "Invalid operation";
    case AL_OUT_OF_MEMORY:     return "Out of memory";
    case AL_NO_ERROR:          return "No error";
    default:                   return "Unknown error";
  }
}

inline void checkError(const std::string& errorMsg) {
  const int errorCode = alGetError();

  if (errorCode != AL_NO_ERROR)
    Logger::error("[OpenAL] " + errorMsg + " (" + recoverAlErrorStr(errorCode) + ").");
}

} // namespace

void Sound::init() {
  Logger::debug("[Sound] Initializing...");

  alGetError(); // Flushing errors

  destroy();

  Logger::debug("[Sound] Creating buffer...");
  alGenBuffers(1, &m_buffer.get());
  checkError("Failed to create a sound buffer");
  Logger::debug("[Sound] Created buffer (ID: " + std::to_string(m_buffer) + ')');

  Logger::debug("[Sound] Creating source...");
  alGenSources(1, &m_source.get());
  checkError("Failed to create a sound source");
  Logger::debug("[Sound] Created source (ID: " + std::to_string(m_source) + ')');

  Logger::debug("[Sound] Initialized");
}

void Sound::load() {
  stop(); // Making sure the sound isn't paused or currently playing
  alSourcei(m_source, AL_BUFFER, 0); // Detaching the previous buffer (if any) from the source

  if ((m_format == AudioFormat::MONO_F32 || m_format == AudioFormat::STEREO_F32) && !alIsExtensionPresent("AL_EXT_float32")) {
    Logger::error("[Sound] Float sound format is not supported by the audio driver.");
    return;
  }

  if ((m_format == AudioFormat::MONO_F64 || m_format == AudioFormat::STEREO_F64) && !alIsExtensionPresent("AL_EXT_double")) {
    Logger::error("[Sound] Double sound format is not supported by the audio driver.");
    return;
  }

  alBufferData(m_buffer, static_cast<int>(m_format), m_data.data(), static_cast<int>(m_data.size()), m_frequency);
  checkError("Failed to send sound information to the buffer");

  alSourcei(m_source, AL_BUFFER, static_cast<int>(m_buffer));
  checkError("Failed to map the sound buffer to the source");
}

void Sound::setPitch(float pitch) const noexcept {
  assert("Error: The source's pitch must be positive." && pitch >= 0.f);

  alSourcef(m_source, AL_PITCH, pitch);
  checkError("Failed to set the source's pitch");
}

float Sound::recoverPitch() const noexcept {
  float pitch {};

  alGetSourcef(m_source, AL_PITCH, &pitch);
  checkError("Failed to recover the source's pitch");

  return pitch;
}

void Sound::setGain(float gain) const noexcept {
  assert("Error: The source's gain must be positive." && gain >= 0.f);

  alSourcef(m_source, AL_GAIN, gain);
  checkError("Failed to set the source's gain");
}

float Sound::recoverGain() const noexcept {
  float gain {};

  alGetSourcef(m_source, AL_GAIN, &gain);
  checkError("Failed to recover the source's gain");

  return gain;
}

void Sound::setPosition(const Vec3f& position) const noexcept {
  setPosition(position.x(), position.y(), position.z());
}

void Sound::setPosition(float x, float y, float z) const noexcept {
  alSource3f(m_source, AL_POSITION, x, y, z);
  checkError("Failed to set the source's position");
}

Vec3f Sound::recoverPosition() const noexcept {
  Vec3f position;

  alGetSource3f(m_source, AL_POSITION, &position.x(), &position.y(), &position.z());
  checkError("Failed to recover the source's position");

  return position;
}

void Sound::setVelocity(const Vec3f& velocity) const noexcept {
  setVelocity(velocity.x(), velocity.y(), velocity.z());
}

void Sound::setVelocity(float x, float y, float z) const noexcept {
  alSource3f(m_source, AL_VELOCITY, x, y, z);
  checkError("Failed to set the source's velocity");
}

Vec3f Sound::recoverVelocity() const noexcept {
  Vec3f velocity;

  alGetSource3f(m_source, AL_VELOCITY, &velocity.x(), &velocity.y(), &velocity.z());
  checkError("Failed to recover the source's velocity");

  return velocity;
}

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
void Sound::linkSlot(const SoundEffectSlot& slot) const noexcept {
  alSource3i(m_source, AL_AUXILIARY_SEND_FILTER, static_cast<int>(slot.getIndex()), 0, AL_FILTER_NULL);
  checkError("Failed to link the sound effect slot to the sound");
}

void Sound::unlinkSlot() const noexcept {
  alSource3i(m_source, AL_AUXILIARY_SEND_FILTER, 0, 0, AL_FILTER_NULL);
  checkError("Failed to unlink the sound effect slot from the sound");
}
#endif

void Sound::setRepeat(bool repeat) const noexcept {
  alSourcei(m_source, AL_LOOPING, repeat);
  checkError("Failed to change the sound's repeat state");
}

void Sound::play() const noexcept {
  if (isPlaying())
    return;

  alSourcePlay(m_source);
  checkError("Failed to play/resume the sound");
}

void Sound::pause() const noexcept {
  alSourcePause(m_source);
  checkError("Failed to pause the sound");
}

void Sound::stop() const noexcept {
  alSourceStop(m_source);
  checkError("Failed to stop the sound");
}

void Sound::rewind() const noexcept {
  alSourceRewind(m_source);
  checkError("Failed to rewind the sound");
}

SoundState Sound::recoverState() const noexcept {
  int state {};
  alGetSourcei(m_source, AL_SOURCE_STATE, &state);

  return static_cast<SoundState>(state);
}

float Sound::recoverElapsedTime() const noexcept {
  float seconds {};
  alGetSourcef(m_source, AL_SEC_OFFSET, &seconds);

  return (seconds / 60.f);
}

void Sound::destroy() {
  if (!m_source.isValid() && !m_buffer.isValid())
    return;

  Logger::debug("[Sound] Destroying...");

  if (m_source.isValid() && alIsSource(m_source)) {
    Logger::debug("[Sound] Destroying source (ID: " + std::to_string(m_source) + ")...");

    alDeleteSources(1, &m_source.get());
    checkError("Failed to delete source");

    Logger::debug("[Sound] Destroyed source");
  }

  m_source.reset();

  if (m_buffer.isValid() && alIsBuffer(m_buffer)) {
    Logger::debug("[Sound] Destroying buffer (ID: " + std::to_string(m_buffer) + ")...");

    alDeleteBuffers(1, &m_buffer.get());
    checkError("Failed to delete buffer");

    Logger::debug("[Sound] Destroyed buffer");
  }

  m_buffer.reset();

  Logger::debug("[Sound] Destroyed");
}

} // namespace Raz
