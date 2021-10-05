#pragma once

#ifndef RAZ_SOUND_HPP
#define RAZ_SOUND_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Vector.hpp"

#include <cstddef>
#include <limits>
#include <vector>

namespace Raz {

namespace Internal { class SoundAccess; }

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

enum class SoundState : int {
  INITIAL = 4113 /* AL_INITIAL */, ///< Initial state, nothing is happening.
  PLAYING = 4114 /* AL_PLAYING */, ///< Being played.
  PAUSED  = 4115 /* AL_PAUSED  */, ///< Paused.
  STOPPED = 4116 /* AL_STOPPED */  ///< Stopped.
};

class Sound final : public Component {
  friend Internal::SoundAccess;

public:
  Sound() { init(); }
  Sound(const Sound&) = delete;
  Sound(Sound&& sound) noexcept;

  constexpr unsigned int getBufferIndex() const noexcept { return m_buffer; }
  constexpr SoundFormat getFormat() const noexcept { return m_format; }
  constexpr int getFrequency() const noexcept { return m_frequency; }

  /// Initializes the sound.
  /// \note A Sound must be initialized again after opening an audio device.
  /// \see AudioSystem::open()
  void init();
  /// Loads the sound's data into memory.
  void load();
  /// Sets the sound's pitch multiplier.
  /// \param pitch Sound's pitch multiplier; must be positive. 1 is the default.
  void setPitch(float pitch) const noexcept;
  /// Recovers the sound's pitch multiplier.
  /// \return Sound's pitch multiplier.
  float recoverPitch() const noexcept;
  /// Sets the sound's gain (volume).
  /// \param gain Sound's gain; must be positive. 1 is the default.
  void setGain(float gain) const noexcept;
  /// Recovers the source's gain (volume).
  /// \return Source's gain.
  float recoverGain() const noexcept;
  /// Sets the audio source's position.
  /// \param position New source's position.
  void setPosition(const Vec3f& position) const noexcept { setPosition(position[0], position[1], position[2]); }
  /// Sets the audio source's position.
  /// \param x New source's X position.
  /// \param y New source's Y position.
  /// \param z New source's Z position.
  void setPosition(float x, float y, float z) const noexcept;
  /// Recovers the position of the sound emitter.
  /// \return Sound's position.
  Vec3f recoverPosition() const noexcept;
  /// Sets the audio source's velocity.
  /// \param velocity New source's velocity.
  void setVelocity(const Vec3f& velocity) const noexcept { setVelocity(velocity[0], velocity[1], velocity[2]); }
  /// Sets the audio source's velocity.
  /// \param x New source's X velocity.
  /// \param y New source's Y velocity.
  /// \param z New source's Z velocity.
  void setVelocity(float x, float y, float z) const noexcept;
  /// Recovers the velocity of the sound emitter.
  /// \return Sound's velocity.
  Vec3f recoverVelocity() const noexcept;
  /// Sets the sound's repeat state.
  /// \param repeat Repeat state; true if the sound should be repeated, false otherwise.
  void repeat(bool repeat) const noexcept;
  /// Plays the sound.
  void play() const noexcept;
  /// Pauses the sound.
  void pause() const noexcept;
  /// Stops the sound.
  void stop() const noexcept;
  /// Rewinds the sound.
  void rewind() const noexcept;
  /// Recovers the current state of the sound.
  /// \return Sound's state.
  SoundState recoverState() const noexcept;
  /// Checks if the sound is currently being played.
  /// \return True if the sound is being played, false otherwise.
  /// \see recoverState()
  bool isPlaying() const noexcept { return (recoverState() == SoundState::PLAYING); }
  /// Checks if the sound is currently paused.
  /// \return True if the sound is paused, false otherwise.
  /// \see recoverState()
  bool isPaused() const noexcept { return (recoverState() == SoundState::PAUSED); }
  /// Checks if the sound is currently stopped.
  /// \return True if the sound is stopped, false otherwise.
  /// \see recoverState()
  bool isStopped() const noexcept { return (recoverState() == SoundState::STOPPED); }
  /// Recovers the amount of minutes the sound has been played so far.
  /// \return Sound's elapsed time, in minutes.
  float recoverElapsedTime() const noexcept;
  /// Destroys the sound.
  void destroy();

  Sound& operator=(const Sound&) = delete;
  Sound& operator=(Sound&& sound) noexcept;

  ~Sound() override { destroy(); }

private:
  unsigned int m_buffer = std::numeric_limits<unsigned int>::max();
  unsigned int m_source = std::numeric_limits<unsigned int>::max();

  SoundFormat m_format {};
  int m_frequency {};
  std::vector<std::byte> m_data {};
};

namespace Internal {

/// Class giving direct access to a Sound's private members; useful for file importers.
/// \note This class is not meant to be used in user code.
class SoundAccess {
public:
  SoundAccess() = delete;

  static SoundFormat& getFormat(Sound& sound) { return sound.m_format; }
  static int& getFrequency(Sound& sound) { return sound.m_frequency; }
  static std::vector<std::byte>& getData(Sound& sound) { return sound.m_data; }
};

} // namespace Internal

} // namespace Raz

#endif // RAZ_SOUND_HPP
