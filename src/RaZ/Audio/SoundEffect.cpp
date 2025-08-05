#include "RaZ/Audio/SoundEffect.hpp"
#include "RaZ/Utils/CompilerUtils.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

#include <AL/efx.h>
#include <AL/efx-presets.h>

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

// Effect functions
LPALGENEFFECTS alGenEffects;
LPALDELETEEFFECTS alDeleteEffects;
LPALISEFFECT alIsEffect;
LPALEFFECTI alEffecti;
//LPALEFFECTIV alEffectiv;
LPALEFFECTF alEffectf;
LPALEFFECTFV alEffectfv;
//LPALGETEFFECTI alGetEffecti;
//LPALGETEFFECTIV alGetEffectiv;
//LPALGETEFFECTF alGetEffectf;
//LPALGETEFFECTFV alGetEffectfv;

inline bool loadFunctions() noexcept {
  PUSH_WARNINGS_STATE
  DISABLE_WARNING_GCC(-Wconditionally-supported)

  alGenEffects    = reinterpret_cast<LPALGENEFFECTS>(alGetProcAddress("alGenEffects"));
  alDeleteEffects = reinterpret_cast<LPALDELETEEFFECTS>(alGetProcAddress("alDeleteEffects"));
  alIsEffect      = reinterpret_cast<LPALISEFFECT>(alGetProcAddress("alIsEffect"));
  alEffecti       = reinterpret_cast<LPALEFFECTI>(alGetProcAddress("alEffecti"));
  //alEffectiv      = reinterpret_cast<LPALEFFECTIV>(alGetProcAddress("alEffectiv"));
  alEffectf       = reinterpret_cast<LPALEFFECTF>(alGetProcAddress("alEffectf"));
  alEffectfv      = reinterpret_cast<LPALEFFECTFV>(alGetProcAddress("alEffectfv"));
  //alGetEffecti    = reinterpret_cast<LPALGETEFFECTI>(alGetProcAddress("alGetEffecti"));
  //alGetEffectiv   = reinterpret_cast<LPALGETEFFECTIV>(alGetProcAddress("alGetEffectiv"));
  //alGetEffectf    = reinterpret_cast<LPALGETEFFECTF>(alGetProcAddress("alGetEffectf"));
  //alGetEffectfv   = reinterpret_cast<LPALGETEFFECTFV>(alGetProcAddress("alGetEffectfv"));

  POP_WARNINGS_STATE

  return true;
}

} // namespace

void SoundEffect::init() {
  ZoneScopedN("SoundEffect::init");

  if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX")) {
    Logger::error("[SoundEffect] Sound effects are unavailable");
    return;
  }

  [[maybe_unused]] static const bool funcsLoaded = loadFunctions();

  Logger::debug("[SoundEffect] Initializing...");

  alGetError(); // Flushing errors

  destroy();

  alGenEffects(1, &m_index.get());
  checkError("Failed to create a sound effect");

  Logger::debug("[SoundEffect] Initialized (ID: " + std::to_string(m_index) + ')');
}

void SoundEffect::load(const ReverberationParams& params) {
  alGetError(); // Flushing errors

  if (alGetEnumValue("AL_EFFECT_EAXREVERB") != 0) {
    alEffecti(m_index, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

    alEffectf(m_index, AL_EAXREVERB_DENSITY, params.density);
    alEffectf(m_index, AL_EAXREVERB_DIFFUSION, params.diffusion);
    alEffectf(m_index, AL_EAXREVERB_GAIN, params.gain);
    alEffectf(m_index, AL_EAXREVERB_GAINHF, params.gainHighFrequency);
    alEffectf(m_index, AL_EAXREVERB_GAINLF, params.gainLowFrequency);
    alEffectf(m_index, AL_EAXREVERB_DECAY_TIME, params.decayTime);
    alEffectf(m_index, AL_EAXREVERB_DECAY_HFRATIO, params.decayHighFrequencyRatio);
    alEffectf(m_index, AL_EAXREVERB_DECAY_LFRATIO, params.decayLowFrequencyRatio);
    alEffectf(m_index, AL_EAXREVERB_REFLECTIONS_GAIN, params.reflectionsGain);
    alEffectf(m_index, AL_EAXREVERB_REFLECTIONS_DELAY, params.reflectionsDelay);
    alEffectfv(m_index, AL_EAXREVERB_REFLECTIONS_PAN, params.reflectionsPan.data());
    alEffectf(m_index, AL_EAXREVERB_LATE_REVERB_GAIN, params.lateReverbGain);
    alEffectf(m_index, AL_EAXREVERB_LATE_REVERB_DELAY, params.lateReverbDelay);
    alEffectfv(m_index, AL_EAXREVERB_LATE_REVERB_PAN, params.lateReverbPan.data());
    alEffectf(m_index, AL_EAXREVERB_ECHO_TIME, params.echoTime);
    alEffectf(m_index, AL_EAXREVERB_ECHO_DEPTH, params.echoDepth);
    alEffectf(m_index, AL_EAXREVERB_MODULATION_TIME, params.modulationTime);
    alEffectf(m_index, AL_EAXREVERB_MODULATION_DEPTH, params.modulationDepth);
    alEffectf(m_index, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, params.airAbsorptionGainHighFrequency);
    alEffectf(m_index, AL_EAXREVERB_HFREFERENCE, params.highFrequencyReference);
    alEffectf(m_index, AL_EAXREVERB_LFREFERENCE, params.lowFrequencyReference);
    alEffectf(m_index, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, params.roomRolloffFactor);
    alEffecti(m_index, AL_EAXREVERB_DECAY_HFLIMIT, params.decayHighFrequencyLimit);
  } else {
    alEffecti(m_index, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

    alEffectf(m_index, AL_REVERB_DENSITY, params.density);
    alEffectf(m_index, AL_REVERB_DIFFUSION, params.diffusion);
    alEffectf(m_index, AL_REVERB_GAIN, params.gain);
    alEffectf(m_index, AL_REVERB_GAINHF, params.gainHighFrequency);
    alEffectf(m_index, AL_REVERB_DECAY_TIME, params.decayTime);
    alEffectf(m_index, AL_REVERB_DECAY_HFRATIO, params.decayHighFrequencyRatio);
    alEffectf(m_index, AL_REVERB_REFLECTIONS_GAIN, params.reflectionsGain);
    alEffectf(m_index, AL_REVERB_REFLECTIONS_DELAY, params.reflectionsDelay);
    alEffectf(m_index, AL_REVERB_LATE_REVERB_GAIN, params.lateReverbGain);
    alEffectf(m_index, AL_REVERB_LATE_REVERB_DELAY, params.lateReverbDelay);
    alEffectf(m_index, AL_REVERB_AIR_ABSORPTION_GAINHF, params.airAbsorptionGainHighFrequency);
    alEffectf(m_index, AL_REVERB_ROOM_ROLLOFF_FACTOR, params.roomRolloffFactor);
    alEffecti(m_index, AL_REVERB_DECAY_HFLIMIT, params.decayHighFrequencyLimit);
  }

  checkError("Failed to set the reverberation effect");
}

void SoundEffect::load(const ChorusParams& params) {
  alGetError(); // Flushing errors

  alEffecti(m_index, AL_EFFECT_TYPE, AL_EFFECT_CHORUS);

  if (alGetError() != AL_NO_ERROR) {
    Logger::error("[OpenAL] Failed to set the chorus effect");
    return;
  }

  alEffecti(m_index, AL_CHORUS_WAVEFORM, static_cast<int>(params.waveform));
  alEffecti(m_index, AL_CHORUS_PHASE, params.phase);
  alEffectf(m_index, AL_CHORUS_RATE, params.rate);
  alEffectf(m_index, AL_CHORUS_DEPTH, params.depth);
  alEffectf(m_index, AL_CHORUS_FEEDBACK, params.feedback);
  alEffectf(m_index, AL_CHORUS_DELAY, params.delay);

  checkError("Failed to set the chorus effect's parameters");
}

void SoundEffect::load(const DistortionParams& params) {
  alGetError(); // Flushing errors

  alEffecti(m_index, AL_EFFECT_TYPE, AL_EFFECT_DISTORTION);

  if (alGetError() != AL_NO_ERROR) {
    Logger::error("[OpenAL] Failed to set the distortion effect");
    return;
  }

  alEffectf(m_index, AL_DISTORTION_EDGE, params.edge);
  alEffectf(m_index, AL_DISTORTION_GAIN, params.gain);
  alEffectf(m_index, AL_DISTORTION_LOWPASS_CUTOFF, params.lowpassCutoff);
  alEffectf(m_index, AL_DISTORTION_EQCENTER, params.eqCenter);
  alEffectf(m_index, AL_DISTORTION_EQBANDWIDTH, params.eqBandwidth);

  checkError("Failed to set the distortion effect's parameters");
}

void SoundEffect::load(const EchoParams& params) {
  alGetError(); // Flushing errors

  alEffecti(m_index, AL_EFFECT_TYPE, AL_EFFECT_ECHO);

  if (alGetError() != AL_NO_ERROR) {
    Logger::error("[OpenAL] Failed to set the echo effect");
    return;
  }

  alEffectf(m_index, AL_ECHO_DELAY, params.delay);
  alEffectf(m_index, AL_ECHO_LRDELAY, params.leftRightDelay);
  alEffectf(m_index, AL_ECHO_DAMPING, params.damping);
  alEffectf(m_index, AL_ECHO_FEEDBACK, params.feedback);
  alEffectf(m_index, AL_ECHO_SPREAD, params.spread);

  checkError("Failed to set the echo effect's parameters");
}

void SoundEffect::reset() {
  alGetError(); // Flushing errors

  alEffecti(m_index, AL_EFFECT_TYPE, AL_EFFECT_NULL);
  checkError("Failed to reset the effect");
}

void SoundEffect::destroy() {
  ZoneScopedN("SoundEffect::destroy");

  if (!m_index.isValid())
    return;

  Logger::debug("[SoundEffect] Destroying (ID: " + std::to_string(m_index) + ")...");

  if (alIsEffect(m_index)) {
    alDeleteEffects(1, &m_index.get());
    checkError("Failed to delete sound effect");
  }

  m_index.reset();

  Logger::debug("[SoundEffect] Destroyed");
}

} // namespace Raz
