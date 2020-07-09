#include "RaZ/Audio/Sound.hpp"

#include <AL/al.h>
#include <iostream>

namespace Raz {

namespace {

inline constexpr const char* recoverAlErrorStr(int errorCode) {
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

} // namespace

Sound::Sound() {
  alGetError(); // Flushing errors

  alGenBuffers(1, &m_buffer);

  int errorCode = alGetError();
  if (errorCode != AL_NO_ERROR)
    std::cerr << "Error: Failed to create a sound buffer (" << recoverAlErrorStr(errorCode) << ")." << std::endl;

  alGenSources(1, &m_source);

  errorCode = alGetError();
  if (errorCode != AL_NO_ERROR)
    std::cerr << "Error: Failed to create a sound source (" << recoverAlErrorStr(errorCode) << ")." << std::endl;
}

Sound::Sound(Sound&& sound) noexcept
  : m_buffer{ std::exchange(sound.m_buffer, std::numeric_limits<unsigned int>::max()) },
    m_source{ std::exchange(sound.m_source, std::numeric_limits<unsigned int>::max()) },
    m_format{ sound.m_format },
    m_frequency{ sound.m_frequency } {}

Sound& Sound::operator=(Sound&& sound) noexcept {
  std::swap(m_buffer, sound.m_buffer);
  std::swap(m_source, sound.m_source);

  m_format    = sound.m_format;
  m_frequency = sound.m_frequency;

  return *this;
}

Sound::~Sound() {
  if (m_source != std::numeric_limits<unsigned int>::max())
    alDeleteSources(1, &m_source);

  if (m_buffer == std::numeric_limits<unsigned int>::max())
    return;

  alDeleteBuffers(1, &m_buffer);
}

} // namespace Raz
