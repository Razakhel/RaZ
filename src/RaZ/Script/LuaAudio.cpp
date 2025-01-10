#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Microphone.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Audio/SoundEffect.hpp"
#include "RaZ/Audio/SoundEffectSlot.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerAudioTypes() {
  sol::state& state = getState();

  {
    sol::usertype<AudioData> audioData = state.new_usertype<AudioData>("AudioData",
                                                                       sol::constructors<AudioData()>());
    audioData["format"]    = &AudioData::format;
    audioData["frequency"] = &AudioData::frequency;
    audioData["buffer"]    = &AudioData::buffer;

    state.new_enum<AudioFormat>("AudioFormat", {
      { "MONO_U8",    AudioFormat::MONO_U8 },
      { "STEREO_U8",  AudioFormat::STEREO_U8 },
      { "MONO_I16",   AudioFormat::MONO_I16 },
      { "STEREO_I16", AudioFormat::STEREO_I16 },
      { "MONO_F32",   AudioFormat::MONO_F32 },
      { "STEREO_F32", AudioFormat::STEREO_F32 },
      { "MONO_F64",   AudioFormat::MONO_F64 },
      { "STEREO_F64", AudioFormat::STEREO_F64 }
    });
  }

  {
    sol::usertype<AudioSystem> audioSystem = state.new_usertype<AudioSystem>("AudioSystem",
                                                                             sol::constructors<AudioSystem(),
                                                                                               AudioSystem(const std::string&)>(),
                                                                             sol::base_classes, sol::bases<System>());
    audioSystem["recoverDevices"]       = &AudioSystem::recoverDevices;
    audioSystem["openDevice"]           = sol::overload([] (AudioSystem& s) { s.openDevice(); },
                                                        PickOverload<const std::string&>(&AudioSystem::openDevice));
    audioSystem["recoverCurrentDevice"] = &AudioSystem::recoverCurrentDevice;
  }

  {
    sol::usertype<Listener> listener = state.new_usertype<Listener>("Listener",
                                                                    sol::constructors<Listener(),
                                                                                      Listener(const Vec3f&),
                                                                                      Listener(const Vec3f&, const Vec3f&, const Vec3f&),
                                                                                      Listener(const Vec3f&, const Mat3f&)>(),
                                                                    sol::base_classes, sol::bases<Component>());
    listener["gain"]                      = sol::property(&Listener::recoverGain, &Listener::setGain);
    listener["position"]                  = sol::property(&Listener::recoverPosition, &Listener::setPosition);
    listener["velocity"]                  = sol::property(&Listener::recoverVelocity, &Listener::setVelocity);
    listener["setOrientation"]            = sol::overload(PickOverload<const Vec3f&>(&Listener::setOrientation),
                                                          PickOverload<const Vec3f&, const Vec3f&>(&Listener::setOrientation),
                                                          PickOverload<const Mat3f&>(&Listener::setOrientation));
    listener["recoverOrientation"]        = &Listener::recoverOrientation;
    listener["recoverForwardOrientation"] = &Listener::recoverForwardOrientation;
    listener["recoverUpOrientation"]      = &Listener::recoverUpOrientation;
  }

  {
    sol::usertype<Microphone> microphone = state.new_usertype<Microphone>("Microphone",
                                                                          sol::constructors<Microphone(AudioFormat, unsigned int, float),
                                                                                            Microphone(AudioFormat, unsigned int, float,
                                                                                                       const std::string&)>());
    microphone["recoverDevices"]              = &Microphone::recoverDevices;
    microphone["openDevice"]                  = sol::overload([] (Microphone& m, AudioFormat fmt, unsigned int freq, float d) { m.openDevice(fmt, freq, d); },
                                                              PickOverload<AudioFormat, unsigned int, float, const std::string&>(&Microphone::openDevice));
    microphone["recoverCurrentDevice"]        = &Microphone::recoverCurrentDevice;
    microphone["start"]                       = &Microphone::start;
    microphone["stop"]                        = &Microphone::stop;
    microphone["recoverAvailableSampleCount"] = &Microphone::recoverAvailableSampleCount;
    microphone["recoverAvailableDuration"]    = &Microphone::recoverAvailableDuration;
    microphone["recoverData"]                 = sol::overload([] (const Microphone& m) { return m.recoverData(); },
                                                              PickOverload<float>(&Microphone::recoverData));
  }

  {
    sol::usertype<Sound> sound = state.new_usertype<Sound>("Sound",
                                                           sol::constructors<Sound(),
                                                                             Sound(AudioData)>(),
                                                           sol::base_classes, sol::bases<Component>());
    sound["getBufferIndex"]     = &Sound::getBufferIndex;
    sound["getData"]            = &Sound::getData;
    sound["init"]               = &Sound::init;
    sound["load"]               = PickOverload<AudioData>(&Sound::load);
    sound["pitch"]              = sol::property(&Sound::recoverPitch, &Sound::setPitch);
    sound["gain"]               = sol::property(&Sound::recoverGain, &Sound::setGain);
    sound["position"]           = sol::property(&Sound::recoverPosition, PickOverload<const Vec3f&>(&Sound::setPosition));
    sound["velocity"]           = sol::property(&Sound::recoverVelocity, PickOverload<const Vec3f&>(&Sound::setVelocity));
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    sound["linkSlot"]           = &Sound::linkSlot;
    sound["unlinkSlot"]         = &Sound::unlinkSlot;
#endif
    sound["setRepeat"]          = &Sound::setRepeat;
    sound["play"]               = &Sound::play;
    sound["pause"]              = &Sound::pause;
    sound["stop"]               = &Sound::stop;
    sound["rewind"]             = &Sound::rewind;
    sound["isPlaying"]          = &Sound::isPlaying;
    sound["isPaused"]           = &Sound::isPaused;
    sound["isStopped"]          = &Sound::isStopped;
    sound["recoverElapsedTime"] = &Sound::recoverElapsedTime;
  }

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  {
    sol::usertype<SoundEffect> soundEffect = state.new_usertype<SoundEffect>("SoundEffect",
                                                                             sol::constructors<SoundEffect()>());
    soundEffect["getIndex"] = &SoundEffect::getIndex;
    soundEffect["init"]     = &SoundEffect::init;
    soundEffect["load"]     = sol::overload(PickOverload<const ReverberationParams&>(&SoundEffect::load),
                                            PickOverload<const ChorusParams&>(&SoundEffect::load),
                                            PickOverload<const DistortionParams&>(&SoundEffect::load),
                                            PickOverload<const EchoParams&>(&SoundEffect::load));
    soundEffect["reset"]    = &SoundEffect::reset;
    soundEffect["destroy"]  = &SoundEffect::destroy;

    // Effects
    {
      sol::usertype<ReverberationParams> reverbParams = state.new_usertype<ReverberationParams>("ReverberationParams",
                                                                                                sol::constructors<ReverberationParams()>());
      reverbParams["density"]                        = &ReverberationParams::density;
      reverbParams["diffusion"]                      = &ReverberationParams::diffusion;
      reverbParams["gain"]                           = &ReverberationParams::gain;
      reverbParams["gainHighFrequency"]              = &ReverberationParams::gainHighFrequency;
      reverbParams["gainLowFrequency"]               = &ReverberationParams::gainLowFrequency;
      reverbParams["decayTime"]                      = &ReverberationParams::decayTime;
      reverbParams["decayHighFrequencyRatio"]        = &ReverberationParams::decayHighFrequencyRatio;
      reverbParams["decayLowFrequencyRatio"]         = &ReverberationParams::decayLowFrequencyRatio;
      reverbParams["reflectionsGain"]                = &ReverberationParams::reflectionsGain;
      reverbParams["reflectionsDelay"]               = &ReverberationParams::reflectionsDelay;
      reverbParams["reflectionsPan"]                 = &ReverberationParams::reflectionsPan;
      reverbParams["lateReverbGain"]                 = &ReverberationParams::lateReverbGain;
      reverbParams["lateReverbDelay"]                = &ReverberationParams::lateReverbDelay;
      reverbParams["lateReverbPan"]                  = &ReverberationParams::lateReverbPan;
      reverbParams["echoTime"]                       = &ReverberationParams::echoTime;
      reverbParams["echoDepth"]                      = &ReverberationParams::echoDepth;
      reverbParams["modulationTime"]                 = &ReverberationParams::modulationTime;
      reverbParams["modulationDepth"]                = &ReverberationParams::modulationDepth;
      reverbParams["airAbsorptionGainHighFrequency"] = &ReverberationParams::airAbsorptionGainHighFrequency;
      reverbParams["highFrequencyReference"]         = &ReverberationParams::highFrequencyReference;
      reverbParams["lowFrequencyReference"]          = &ReverberationParams::lowFrequencyReference;
      reverbParams["roomRolloffFactor"]              = &ReverberationParams::roomRolloffFactor;
      reverbParams["decayHighFrequencyLimit"]        = &ReverberationParams::decayHighFrequencyLimit;

      state.new_enum<SoundWaveform>("SoundWaveform", {
        { "SINUSOID", SoundWaveform::SINUSOID },
        { "TRIANGLE", SoundWaveform::TRIANGLE }
      });

      sol::usertype<ChorusParams> chorusParams = state.new_usertype<ChorusParams>("ChorusParams",
                                                                                  sol::constructors<ChorusParams()>());
      chorusParams["waveform"] = &ChorusParams::waveform;
      chorusParams["phase"]    = &ChorusParams::phase;
      chorusParams["rate"]     = &ChorusParams::rate;
      chorusParams["depth"]    = &ChorusParams::depth;
      chorusParams["feedback"] = &ChorusParams::feedback;
      chorusParams["delay"]    = &ChorusParams::delay;

      sol::usertype<DistortionParams> distortionParams = state.new_usertype<DistortionParams>("DistortionParams",
                                                                                              sol::constructors<DistortionParams()>());
      distortionParams["edge"]          = &DistortionParams::edge;
      distortionParams["gain"]          = &DistortionParams::gain;
      distortionParams["lowpassCutoff"] = &DistortionParams::lowpassCutoff;
      distortionParams["eqCenter"]      = &DistortionParams::eqCenter;
      distortionParams["eqBandwidth"]   = &DistortionParams::eqBandwidth;

      sol::usertype<EchoParams> echoParams = state.new_usertype<EchoParams>("EchoParams",
                                                                            sol::constructors<EchoParams()>());
      echoParams["delay"]          = &EchoParams::delay;
      echoParams["leftRightDelay"] = &EchoParams::leftRightDelay;
      echoParams["damping"]        = &EchoParams::damping;
      echoParams["feedback"]       = &EchoParams::feedback;
      echoParams["spread"]         = &EchoParams::spread;
    }
  }

  {
    sol::usertype<SoundEffectSlot> soundEffectSlot = state.new_usertype<SoundEffectSlot>("SoundEffectSlot",
                                                                                         sol::constructors<SoundEffectSlot()>());
    soundEffectSlot["getIndex"]   = &SoundEffectSlot::getIndex;
    soundEffectSlot["init"]       = &SoundEffectSlot::init;
    soundEffectSlot["loadEffect"] = &SoundEffectSlot::loadEffect;
    soundEffectSlot["destroy"]    = &SoundEffectSlot::destroy;
  }
#endif
}

} // namespace Raz
