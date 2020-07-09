#pragma once

#ifndef RAZ_SOUND_HPP
#define RAZ_SOUND_HPP

#include "RaZ/Component.hpp"

namespace Raz {

class FilePath;

enum class SoundFormat : int {
  MONO_U8    = 4352  /* AL_FORMAT_MONO8             */, ///< Mono format on 8 unsigned bits (0 to 255).
  STEREO_U8  = 4353  /* AL_FORMAT_STEREO8           */, ///< Stereo format on 8 unsigned bits (0 to 255).
  MONO_I16   = 4354  /* AL_FORMAT_MONO16            */, ///< Mono format on 16 signed bits (-32768 to 32767).
  STEREO_I16 = 4355  /* AL_FORMAT_STEREO16          */, ///< Stereo format on 16 signed bits (-32768 to 32767).
  MONO_F32   = 65552 /* AL_FORMAT_MONO_FLOAT32      */, ///< Mono format on 32 floating-point bits (float).
  STEREO_F32 = 65553 /* AL_FORMAT_STEREO_FLOAT32    */, ///< Stereo format on 32 floating-point bits (float).
  MONO_F64   = 65554 /* AL_FORMAT_MONO_DOUBLE_EXT   */, ///< Mono format on 64 floating-point bits (double).
  STEREO_F64 = 65555 /* AL_FORMAT_STEREO_DOUBLE_EXT */  ///< Stereo format on 64 floating-point bits (double).
};

class Sound final : public Component {
public:
  Sound();
  Sound(const Sound&) = delete;
  Sound(Sound&& sound) noexcept;

  constexpr unsigned int getBufferIndex() const noexcept { return m_buffer; }

  Sound& operator=(const Sound&) = delete;
  Sound& operator=(Sound&& sound) noexcept;

  ~Sound() override;

private:
  unsigned int m_buffer = std::numeric_limits<unsigned int>::max();
  unsigned int m_source = std::numeric_limits<unsigned int>::max();

  SoundFormat m_format {};
  int m_frequency {};
};

} // namespace Raz

#endif // RAZ_SOUND_HPP
