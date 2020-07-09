#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

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
    m_frequency{ sound.m_frequency },
    m_data{ std::move(sound.m_data) } {}

void Sound::load(const FilePath& filePath) {
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

    if (format == "wav")
      loadWav(file);
    else
      std::cerr << "Warning: '" + format + "' format is not supported, image ignored" << std::endl;
  } else {
    std::cerr << "Error: Couldn't open the file '" + filePath + "'" << std::endl;
  }

  alBufferData(m_buffer, static_cast<int>(m_format), m_data.data(), static_cast<int>(m_data.size()), m_frequency);

  if (alGetError() != AL_NO_ERROR)
    std::cerr << "Error: Failed to send sound information to the buffer." << std::endl;

  alSourcei(m_source, AL_BUFFER, static_cast<int>(m_buffer));

  if (alGetError() != AL_NO_ERROR)
    std::cerr << "Error: Failed to map the sound buffer to the source." << std::endl;
}

Sound& Sound::operator=(Sound&& sound) noexcept {
  std::swap(m_buffer, sound.m_buffer);
  std::swap(m_source, sound.m_source);

  m_format    = sound.m_format;
  m_frequency = sound.m_frequency;
  m_data      = std::move(sound.m_data);

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
