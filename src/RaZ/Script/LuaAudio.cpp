#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerAudioTypes() {
  sol::state& state = getState();

  {
    sol::usertype<AudioSystem> audioSystem = state.new_usertype<AudioSystem>("AudioSystem",
                                                                             sol::constructors<AudioSystem(),
                                                                                               AudioSystem(const char*)>(),
                                                                             sol::base_classes, sol::bases<System>());
    audioSystem["recoverDevices"]       = &AudioSystem::recoverDevices;
    audioSystem["openDevice"]           = &AudioSystem::openDevice;
    audioSystem["recoverCurrentDevice"] = &AudioSystem::recoverCurrentDevice;
  }

  {
    sol::usertype<Listener> listener = state.new_usertype<Listener>("Listener",
                                                                    sol::constructors<Listener(),
                                                                                      Listener(const Vec3f&),
                                                                                      Listener(const Vec3f&, const Vec3f&, const Vec3f&),
                                                                                      Listener(const Vec3f&, const Mat3f&)>(),
                                                                    sol::base_classes, sol::bases<Component>());
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
    sol::usertype<Sound> sound = state.new_usertype<Sound>("Sound",
                                                           sol::constructors<Sound()>(),
                                                           sol::base_classes, sol::bases<Component>());
    sound["getBufferIndex"]     = &Sound::getBufferIndex;
    sound["getFormat"]          = &Sound::getFormat;
    sound["getFrequency"]       = &Sound::getFrequency;
    sound["init"]               = &Sound::init;
    sound["load"]               = &Sound::load;
    sound["pitch"]              = sol::property(&Sound::recoverPitch, &Sound::setPitch);
    sound["gain"]               = sol::property(&Sound::recoverGain, &Sound::setGain);
    sound["position"]           = sol::property(&Sound::recoverPosition, PickOverload<const Vec3f&>(&Sound::setPosition));
    sound["velocity"]           = sol::property(&Sound::recoverVelocity, PickOverload<const Vec3f&>(&Sound::setVelocity));
    sound["setRepeat"]          = &Sound::setRepeat;
    sound["play"]               = &Sound::play;
    sound["pause"]              = &Sound::pause;
    sound["stop"]               = &Sound::stop;
    sound["rewind"]             = &Sound::rewind;
    sound["isPlaying"]          = &Sound::isPlaying;
    sound["isPaused"]           = &Sound::isPaused;
    sound["isStopped"]          = &Sound::isStopped;
    sound["recoverElapsedTime"] = &Sound::recoverElapsedTime;

    state.new_enum<AudioFormat>("AudioFormat", {
      { "MONO_U8", AudioFormat::MONO_U8 },
      { "STEREO_U8", AudioFormat::STEREO_U8 },
      { "MONO_I16", AudioFormat::MONO_I16 },
      { "STEREO_I16", AudioFormat::STEREO_I16 },
      { "MONO_F32", AudioFormat::MONO_F32 },
      { "STEREO_F32", AudioFormat::STEREO_F32 },
      { "MONO_F64", AudioFormat::MONO_F64 },
      { "STEREO_F64", AudioFormat::STEREO_F64 }
    });
  }
}

} // namespace Raz
