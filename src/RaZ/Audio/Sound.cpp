#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <AL/al.h>
#include <iostream>

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

inline void checkError(const std::string_view& errorMsg) {
  const int errorCode = alGetError();

  if (errorCode != AL_NO_ERROR)
    std::cerr << "[OpenAL] Error: " << errorMsg << " (" << recoverAlErrorStr(errorCode) << ")." << std::endl;
}

} // namespace

Sound::Sound(Sound&& sound) noexcept
  : m_buffer{ std::exchange(sound.m_buffer, std::numeric_limits<unsigned int>::max()) },
    m_source{ std::exchange(sound.m_source, std::numeric_limits<unsigned int>::max()) },
    m_format{ sound.m_format },
    m_frequency{ sound.m_frequency },
    m_data{ std::move(sound.m_data) } {}

void Sound::init() {
  alGetError(); // Flushing errors

  destroy();

  alGenBuffers(1, &m_buffer);
  checkError("Failed to create a sound buffer");

  alGenSources(1, &m_source);
  checkError("Failed to create a sound source");
}

void Sound::load(const FilePath& filePath) {
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Couldn't open the sound file '" + filePath + "'");

  m_data.clear();

  const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (format == "wav")
    loadWav(file);
  else
    throw std::invalid_argument("Error: '" + format + "' sound format is not supported");

  stop(); // Making sure the sound isn't paused or currently playing
  alSourcei(m_source, AL_BUFFER, 0); // Detaching the previous buffer (if any) from the source

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

void Sound::setGain(float gain) const noexcept {
  assert("Error: The source's gain must be positive." && gain >= 0.f);

  alSourcef(m_source, AL_GAIN, gain);
  checkError("Failed to set the source's gain");
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

void Sound::repeat(bool repeat) const noexcept {
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
  if (m_source != std::numeric_limits<unsigned int>::max()) {
    alDeleteSources(1, &m_source);
    checkError("Failed to delete source");
    m_source = std::numeric_limits<unsigned int>::max();
  }

  if (m_buffer != std::numeric_limits<unsigned int>::max()) {
    alDeleteBuffers(1, &m_buffer);
    checkError("Failed to delete buffer");
    m_buffer = std::numeric_limits<unsigned int>::max();
  }
}

Sound& Sound::operator=(Sound&& sound) noexcept {
  std::swap(m_buffer, sound.m_buffer);
  std::swap(m_source, sound.m_source);

  m_format    = sound.m_format;
  m_frequency = sound.m_frequency;
  m_data      = std::move(sound.m_data);

  return *this;
}

} // namespace Raz
