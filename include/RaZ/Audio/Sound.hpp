#pragma once

#ifndef RAZ_SOUND_HPP
#define RAZ_SOUND_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Audio/AudioData.hpp"
#include "RaZ/Data/OwnerValue.hpp"

#include <limits>

namespace Raz {

class SoundEffectSlot;

template <typename T, std::size_t Size>
class Vector;
using Vec3f = Vector<float, 3>;

enum class SoundState : int {
  INITIAL = 4113 /* AL_INITIAL */, ///< Initial state, nothing is happening.
  PLAYING = 4114 /* AL_PLAYING */, ///< Being played.
  PAUSED  = 4115 /* AL_PAUSED  */, ///< Paused.
  STOPPED = 4116 /* AL_STOPPED */  ///< Stopped.
};

class Sound final : public Component {
public:
  Sound() { init(); }
  explicit Sound(AudioData data) : Sound() { load(std::move(data)); }
  Sound(const Sound&) = delete;
  Sound(Sound&&) noexcept = default;

  constexpr unsigned int getBufferIndex() const noexcept { return m_bufferIndex; }
  constexpr const AudioData& getData() const noexcept { return m_data; }

  /// Initializes the sound. If there is audio data, also loads it into memory.
  /// \note A Sound must be initialized again after opening an audio device.
  /// \see AudioSystem::open()
  void init();
  /// Loads the given audio data into memory.
  /// \param data Data to be loaded.
  void load(AudioData data) {
    m_data = std::move(data);
    load();
  }
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
  /// \note Note that positional audio will only be effective with sounds having a mono format.
  /// \param position New source's position.
  void setPosition(const Vec3f& position) const noexcept;
  /// Sets the audio source's position.
  /// \note Note that positional audio will only be effective with sounds having a mono format.
  /// \param x New source's X position.
  /// \param y New source's Y position.
  /// \param z New source's Z position.
  void setPosition(float x, float y, float z) const noexcept;
  /// Recovers the position of the sound emitter.
  /// \return Sound's position.
  Vec3f recoverPosition() const noexcept;
  /// Sets the audio source's velocity.
  /// \param velocity New source's velocity.
  void setVelocity(const Vec3f& velocity) const noexcept;
  /// Sets the audio source's velocity.
  /// \param x New source's X velocity.
  /// \param y New source's Y velocity.
  /// \param z New source's Z velocity.
  void setVelocity(float x, float y, float z) const noexcept;
  /// Recovers the velocity of the sound emitter.
  /// \return Sound's velocity.
  Vec3f recoverVelocity() const noexcept;
#if !defined(__EMSCRIPTEN__)
  /// Links a sound effect slot to the current sound.
  /// \param slot Slot to be linked.
  void linkSlot(const SoundEffectSlot& slot) const noexcept;
  /// Unlinks any sound effect slot from the current sound.
  void unlinkSlot() const noexcept;
#endif
  /// Sets the sound's repeat state.
  /// \param repeat Repeat state; true if the sound should be repeated, false otherwise.
  void setRepeat(bool repeat) const noexcept;
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
  Sound& operator=(Sound&&) noexcept = default;

  ~Sound() override { destroy(); }

private:
  /// Loads the audio data into memory.
  void load();

  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_bufferIndex {};
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_sourceIndex {};

  AudioData m_data {};
};

} // namespace Raz

#endif // RAZ_SOUND_HPP
