#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Audio/SoundEffectSlot.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

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
    Logger::error("[OpenAL] " + errorMsg + " (" + recoverAlErrorStr(errorCode) + ')');
}

} // namespace

void Sound::init() {
  ZoneScopedN("Sound::init");

  Logger::debug("[Sound] Initializing...");

  alGetError(); // Flushing errors

  destroy();

  Logger::debug("[Sound] Creating buffer...");
  alGenBuffers(1, &m_bufferIndex.get());
  checkError("Failed to create a sound buffer");
  Logger::debug("[Sound] Created buffer (ID: " + std::to_string(m_bufferIndex) + ')');

  Logger::debug("[Sound] Creating source...");
  alGenSources(1, &m_sourceIndex.get());
  checkError("Failed to create a sound source");
  Logger::debug("[Sound] Created source (ID: " + std::to_string(m_sourceIndex) + ')');

  if (!m_data.buffer.empty())
    load();

  Logger::debug("[Sound] Initialized");
}

void Sound::setPitch(float pitch) const noexcept {
  assert("Error: The source's pitch must be positive." && pitch >= 0.f);

  alSourcef(m_sourceIndex, AL_PITCH, pitch);
  checkError("Failed to set the source's pitch");
}

float Sound::recoverPitch() const noexcept {
  float pitch {};

  alGetSourcef(m_sourceIndex, AL_PITCH, &pitch);
  checkError("Failed to recover the source's pitch");

  return pitch;
}

void Sound::setGain(float gain) const noexcept {
  assert("Error: The source's gain must be positive." && gain >= 0.f);

  alSourcef(m_sourceIndex, AL_GAIN, gain);
  checkError("Failed to set the source's gain");
}

float Sound::recoverGain() const noexcept {
  float gain {};

  alGetSourcef(m_sourceIndex, AL_GAIN, &gain);
  checkError("Failed to recover the source's gain");

  return gain;
}

void Sound::setPosition(const Vec3f& position) const noexcept {
  setPosition(position.x(), position.y(), position.z());
}

void Sound::setPosition(float x, float y, float z) const noexcept {
  alSource3f(m_sourceIndex, AL_POSITION, x, y, z);
  checkError("Failed to set the source's position");
}

Vec3f Sound::recoverPosition() const noexcept {
  Vec3f position;

  alGetSource3f(m_sourceIndex, AL_POSITION, &position.x(), &position.y(), &position.z());
  checkError("Failed to recover the source's position");

  return position;
}

void Sound::setVelocity(const Vec3f& velocity) const noexcept {
  setVelocity(velocity.x(), velocity.y(), velocity.z());
}

void Sound::setVelocity(float x, float y, float z) const noexcept {
  alSource3f(m_sourceIndex, AL_VELOCITY, x, y, z);
  checkError("Failed to set the source's velocity");
}

Vec3f Sound::recoverVelocity() const noexcept {
  Vec3f velocity;

  alGetSource3f(m_sourceIndex, AL_VELOCITY, &velocity.x(), &velocity.y(), &velocity.z());
  checkError("Failed to recover the source's velocity");

  return velocity;
}

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
void Sound::linkSlot(const SoundEffectSlot& slot) const noexcept {
  alSource3i(m_sourceIndex, AL_AUXILIARY_SEND_FILTER, static_cast<int>(slot.getIndex()), 0, AL_FILTER_NULL);
  checkError("Failed to link the sound effect slot to the sound");
}

void Sound::unlinkSlot() const noexcept {
  alSource3i(m_sourceIndex, AL_AUXILIARY_SEND_FILTER, 0, 0, AL_FILTER_NULL);
  checkError("Failed to unlink the sound effect slot from the sound");
}
#endif

void Sound::setRepeat(bool repeat) const noexcept {
  alSourcei(m_sourceIndex, AL_LOOPING, repeat);
  checkError("Failed to change the sound's repeat state");
}

void Sound::play() const noexcept {
  if (isPlaying())
    return;

  alSourcePlay(m_sourceIndex);
  checkError("Failed to play/resume the sound");
}

void Sound::pause() const noexcept {
  alSourcePause(m_sourceIndex);
  checkError("Failed to pause the sound");
}

void Sound::stop() const noexcept {
  alSourceStop(m_sourceIndex);
  checkError("Failed to stop the sound");
}

void Sound::rewind() const noexcept {
  alSourceRewind(m_sourceIndex);
  checkError("Failed to rewind the sound");
}

SoundState Sound::recoverState() const noexcept {
  int state {};
  alGetSourcei(m_sourceIndex, AL_SOURCE_STATE, &state);

  return static_cast<SoundState>(state);
}

float Sound::recoverElapsedTime() const noexcept {
  float seconds {};
  alGetSourcef(m_sourceIndex, AL_SEC_OFFSET, &seconds);

  return (seconds / 60.f);
}

void Sound::destroy() {
  ZoneScopedN("Sound::destroy");

  if (!m_sourceIndex.isValid() && !m_bufferIndex.isValid())
    return;

  Logger::debug("[Sound] Destroying...");

  if (m_sourceIndex.isValid() && alIsSource(m_sourceIndex)) {
    Logger::debug("[Sound] Destroying source (ID: " + std::to_string(m_sourceIndex) + ")...");

    alDeleteSources(1, &m_sourceIndex.get());
    checkError("Failed to delete source");

    Logger::debug("[Sound] Destroyed source");
  }

  m_sourceIndex.reset();

  if (m_bufferIndex.isValid() && alIsBuffer(m_bufferIndex)) {
    Logger::debug("[Sound] Destroying buffer (ID: " + std::to_string(m_bufferIndex) + ")...");

    alDeleteBuffers(1, &m_bufferIndex.get());
    checkError("Failed to delete buffer");

    Logger::debug("[Sound] Destroyed buffer");
  }

  m_bufferIndex.reset();

  Logger::debug("[Sound] Destroyed");
}

void Sound::load() {
  ZoneScopedN("Sound::load");

  stop(); // Making sure the sound isn't paused or currently playing
  alSourcei(m_sourceIndex, AL_BUFFER, 0); // Detaching the previous buffer (if any) from the source

  if ((m_data.format == AudioFormat::MONO_F32 || m_data.format == AudioFormat::STEREO_F32) && !alIsExtensionPresent("AL_EXT_float32")) {
    Logger::error("[Sound] Float audio format is not supported by the audio driver");
    return;
  }

  if ((m_data.format == AudioFormat::MONO_F64 || m_data.format == AudioFormat::STEREO_F64) && !alIsExtensionPresent("AL_EXT_double")) {
    Logger::error("[Sound] Double audio format is not supported by the audio driver");
    return;
  }

  alBufferData(m_bufferIndex, static_cast<int>(m_data.format), m_data.buffer.data(),
               static_cast<int>(m_data.buffer.size()), static_cast<int>(m_data.frequency));
  checkError("Failed to send audio data to the buffer");

  alSourcei(m_sourceIndex, AL_BUFFER, static_cast<int>(m_bufferIndex));
  checkError("Failed to map the sound buffer to the source");
}

} // namespace Raz
