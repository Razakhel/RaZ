#pragma once

#ifndef RAZ_SOUNDEFFECT_HPP
#define RAZ_SOUNDEFFECT_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <array>
#include <limits>

namespace Raz {

/// Reverberation sound effect parameters. Some require EAX reverberation to be supported by the implementation.
struct ReverberationParams {
  float density                        = 1.f;               ///< Must be between [0; 1].
  float diffusion                      = 1.f;               ///< Reverberation smoothness; lower is sharper, higher is smoother. Must be between [0; 1].
  float gain                           = 0.32f;             ///< Reverberation global volume. Must be between [0; 1].
  float gainHighFrequency              = 0.89f;             ///< High frequencies volume. Must be between [0; 1].
  float gainLowFrequency               = 1.f;               ///< Low frequencies volume. Must be between [0; 1]. Requires EAX reverb.
  float decayTime                      = 1.49f;             ///< Amount of time for the sound to last, in seconds. Must be between [0.1; 20].
  float decayHighFrequencyRatio        = 0.83f;             ///< Must be between [0.1; 2].
  float decayLowFrequencyRatio         = 1.f;               ///< Must be between [0.1; 2]. Requires EAX reverb.
  float reflectionsGain                = 0.05f;             ///< Initial reverberation volume. Must be between [0; 3.16].
  float reflectionsDelay               = 0.007f;            ///< Pre delay; time to start the initial reverberation from, in seconds. Must be between [0; 0.3].
  std::array<float, 3> reflectionsPan  = { 0.f, 0.f, 0.f }; ///< Initial reverberation position. Requires EAX reverb.
  float lateReverbGain                 = 1.26f;             ///< Late reverberation volume. Must be between [0; 10].
  float lateReverbDelay                = 0.011f;            ///< Amount of time to start the late reverberation from, in seconds. Must be between [0; 0.1].
  std::array<float, 3> lateReverbPan   = { 0.f, 0.f, 0.f }; ///< Late reverberation position. Requires EAX reverb.
  float echoTime                       = 0.25f;             ///< Must be between [0.075; 0.25]. Requires EAX reverb.
  float echoDepth                      = 0.f;               ///< Must be between [0; 1]. Requires EAX reverb.
  float modulationTime                 = 0.25f;             ///< Must be between [0.04; 4]. Requires EAX reverb.
  float modulationDepth                = 0.f;               ///< Must be between [0; 1]. Requires EAX reverb.
  float airAbsorptionGainHighFrequency = 0.994f;            ///< Must be between [0.892; 1].
  float highFrequencyReference         = 5000.f;            ///< Must be between [1000; 20000]. Requires EAX reverb.
  float lowFrequencyReference          = 250.f;             ///< Must be between [20; 1000]. Requires EAX reverb.
  float roomRolloffFactor              = 0.f;               ///< Must be between [0; 10].
  bool decayHighFrequencyLimit         = true;              ///<
};

enum class SoundWaveform {
  SINUSOID = 0, ///< Sinusoid wave, giving smooth modulations.
  TRIANGLE = 1  ///< Triangle wave, giving sharp modulations.
};

struct ChorusParams {
  SoundWaveform waveform = SoundWaveform::TRIANGLE; ///< Waveform of the effect.
  int phase              = 90;                      ///< Must be between [-180: 180].
  float rate             = 1.1f;                    ///< Modulation speed, in Hertz. Must be between [0; 10].
  float depth            = 0.1f;                    ///< Modulation frequency range. Must be between [0; 1].
  float feedback         = 0.25f;                   ///< Must be between [-1; 1].
  float delay            = 0.016f;                  ///< Must be between [0; 0.016].
};

struct DistortionParams {
  float edge          = 0.2f;   ///< Must be between [0; 1].
  float gain          = 0.05f;  ///< Must be between [0.01; 1].
  float lowpassCutoff = 8000.f; ///< Must be between [80; 24000].
  float eqCenter      = 3600.f; ///< Must be between [80; 24000].
  float eqBandwidth   = 3600.f; ///< Must be between [80; 24000].
};

struct EchoParams {
  float delay          = 0.1f; ///< Delay between each echo, in seconds. Must be between [0; 0.207].
  float leftRightDelay = 0.1f; ///< Delay between left & right echoes, in seconds; 0 disables stereo. Must be between [0; 0.404].
  float damping        = 0.5f; ///< Must be between [0; 0.99].
  float feedback       = 0.5f; ///< Falloff ratio of each subsequent echo. Must be between [0; 1].
  float spread         = -1.f; ///< Must be between [-1; 1].
};

class SoundEffect {
public:
  SoundEffect() { init(); }
  SoundEffect(const SoundEffect&) = delete;
  SoundEffect(SoundEffect&&) noexcept = default;

  unsigned int getIndex() const noexcept { return m_index; }

  /// Initializes the sound effect.
  /// \note A SoundEffect must be initialized again after opening an audio device.
  /// \see AudioSystem::open()
  void init();
  /// Loads the given reverberation effect parameters.
  /// \param params Parameters to be loaded.
  void load(const ReverberationParams& params);
  /// Loads the given chorus effect parameters.
  /// \param params Parameters to be loaded.
  void load(const ChorusParams& params);
  /// Loads the given distortion effect parameters.
  /// \param params Parameters to be loaded.
  void load(const DistortionParams& params);
  /// Loads the given echo effect parameters.
  /// \param params Parameters to be loaded.
  void load(const EchoParams& params);
  /// Resets the effect, removing any currently assigned.
  void reset();
  /// Destroys the sound effect.
  void destroy();

  SoundEffect& operator=(const SoundEffect&) = delete;
  SoundEffect& operator=(SoundEffect&&) noexcept = default;

  ~SoundEffect() { destroy(); }

private:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
};

} // namespace Raz

#endif // RAZ_SOUNDEFFECT_HPP
