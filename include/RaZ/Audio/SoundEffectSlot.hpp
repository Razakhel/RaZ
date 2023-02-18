#pragma once

#ifndef RAZ_SOUNDEFFECTSLOT_HPP
#define RAZ_SOUNDEFFECTSLOT_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <limits>

namespace Raz {

class SoundEffect;

class SoundEffectSlot {
public:
  SoundEffectSlot() { init(); }
  SoundEffectSlot(const SoundEffectSlot&) = delete;
  SoundEffectSlot(SoundEffectSlot&&) noexcept = default;

  unsigned int getIndex() const noexcept { return m_index; }

  /// Initializes the sound effect slot.
  /// \note A SoundEffectSlot must be initialized again after opening an audio device.
  /// \see AudioSystem::open()
  void init();
  /// Loads a sound effect into the current slot.
  /// \param effect Sound effect to be loaded.
  void loadEffect(const SoundEffect& effect) const noexcept;
  /// Destroys the sound effect slot.
  void destroy();

  SoundEffectSlot& operator=(const SoundEffectSlot&) = delete;
  SoundEffectSlot& operator=(SoundEffectSlot&&) noexcept = default;

  ~SoundEffectSlot() { destroy(); }

private:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
};

} // namespace Raz

#endif // RAZ_SOUNDEFFECTSLOT_HPP
