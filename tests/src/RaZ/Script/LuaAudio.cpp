#include "RaZ/Script/LuaWrapper.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaAudio AudioSystem", "[script][lua][audio]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new()
    audioSystem       = AudioSystem.new("")

    assert(AudioSystem.recoverDevices() ~= nil)
    assert(audioSystem:recoverCurrentDevice() ~= nil)
    audioSystem:openDevice()
    audioSystem:openDevice("invalid device")
    assert(audioSystem:recoverCurrentDevice() == "")

    assert(audioSystem:getAcceptedComponents() ~= nil)
    assert(not audioSystem:containsEntity(Entity.new(0)))
    assert(audioSystem:update(FrameTimeInfo.new()))
    audioSystem:destroy()
  )"));
}

TEST_CASE("LuaAudio Listener", "[script][lua][audio]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new() -- Initializing the audio device & context, needed before all audio action

    local listener = Listener.new()
    listener       = Listener.new(Vec3f.new(1))
    listener       = Listener.new(Vec3f.new(1), Axis.Z, Axis.Y)
    listener       = Listener.new(Vec3f.new(1), Mat3f.identity())

    listener.gain = 0.25
    assert(listener.gain == 0.25)

    listener.position = Vec3f.new(1, 2, 3)
    assert(listener.position == Vec3f.new(1, 2, 3))

    listener.velocity = Vec3f.new(0, 0, 10)
    assert(listener.velocity == Vec3f.new(0, 0, 10))

    listener:setOrientation(Axis.Z, Axis.Y)
    listener:setOrientation(Mat3f.identity())

    local forward, up = listener:recoverOrientation()
    assert(forward == -Axis.Z)
    assert(up == Axis.Y)
    assert(listener:recoverForwardOrientation() == -Axis.Z)
    assert(listener:recoverUpOrientation() == Axis.Y)
  )"));
}

TEST_CASE("LuaAudio Microphone", "[script][lua][audio]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local microphone = Microphone.new(AudioFormat.MONO_U8, 8000, 1)
    microphone       = Microphone.new(AudioFormat.MONO_I16, 8000, 1, "")

    assert(Microphone.recoverDevices() ~= nil)
    assert(microphone:recoverCurrentDevice() ~= nil)
    microphone:openDevice(AudioFormat.STEREO_U8, 8000, 1, "invalid device")
    assert(microphone:recoverCurrentDevice() == "")
    microphone:start()
    microphone:stop()
    assert(microphone:recoverAvailableSampleCount() ~= nil)
    assert(microphone:recoverAvailableDuration() ~= nil)
    assert(#microphone:recoverData() == #microphone:recoverData(0))
    assert(microphone:recoverSound() ~= microphone:recoverSound(0))
  )"));
}

TEST_CASE("LuaAudio Sound", "[script][lua][audio]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new() -- Initializing the audio device & context, needed before all audio action

    local sound = Sound.new()
    sound       = WavFormat.load(FilePath.new(RAZ_TESTS_ROOT .. "assets/sounds/notif_ting.wav"))

    assert(sound:getBufferIndex() >= 0)
    assert(sound:getFormat() == AudioFormat.MONO_I16)
    assert(sound:getFrequency() == 48000)

    sound:init()
    sound:load()

    sound.pitch    = 0.5
    sound.gain     = 0
    sound.position = Vec3f.new(1, 2, 3)
    sound.velocity = Vec3f.new(0, 0, 10)
    assert(sound.pitch == 0.5)
    assert(sound.gain == 0)
    assert(sound.position == Vec3f.new(1, 2, 3))
    assert(sound.velocity == Vec3f.new(0, 0, 10))

    sound:setRepeat(false)
    sound:play()
    assert(sound:isPlaying())
    sound:pause()
    assert(sound:isPaused())
    sound:stop()
    assert(sound:isStopped())
    sound:rewind()
    assert(sound:recoverElapsedTime() == 0)
  )"));

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new() -- Initializing the audio device & context, needed before all audio action

    local sound = Sound.new()
    sound:linkSlot(SoundEffectSlot.new())
    sound:unlinkSlot()
  )"));
#endif
}

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
TEST_CASE("LuaAudio SoundEffect", "[script][lua][audio]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new() -- Initializing the audio device & context, needed before all audio action

    local soundEffect = SoundEffect.new()

    assert(soundEffect:getIndex() >= 0)
    soundEffect:init()

    local reverbParams = ReverberationParams.new()
    reverbParams.density                        = 0
    reverbParams.diffusion                      = 0
    reverbParams.gain                           = 0
    reverbParams.gainHighFrequency              = 0
    reverbParams.gainLowFrequency               = 0
    reverbParams.decayTime                      = 0.1
    reverbParams.decayHighFrequencyRatio        = 0.1
    reverbParams.decayLowFrequencyRatio         = 0.1
    reverbParams.reflectionsGain                = 0
    reverbParams.reflectionsDelay               = 0
    reverbParams.reflectionsPan                 = { 0, 0, 0 }
    reverbParams.lateReverbGain                 = 0
    reverbParams.lateReverbDelay                = 0
    reverbParams.lateReverbPan                  = { 0, 0, 0 }
    reverbParams.echoTime                       = 0.075
    reverbParams.echoDepth                      = 0
    reverbParams.modulationTime                 = 0.04
    reverbParams.modulationDepth                = 0
    reverbParams.airAbsorptionGainHighFrequency = 0.892
    reverbParams.highFrequencyReference         = 1000
    reverbParams.lowFrequencyReference          = 20
    reverbParams.roomRolloffFactor              = 0
    reverbParams.decayHighFrequencyLimit        = false
    soundEffect:load(reverbParams)

    local chorusParams = ChorusParams.new()
    chorusParams.waveform = SoundWaveform.SINUSOID
    chorusParams.phase    = 0
    chorusParams.rate     = 0
    chorusParams.depth    = 0
    chorusParams.feedback = 0
    chorusParams.delay    = 0
    soundEffect:load(chorusParams)

    local distortionParams = DistortionParams.new()
    distortionParams.edge          = 0
    distortionParams.gain          = 0.01
    distortionParams.lowpassCutoff = 80
    distortionParams.eqCenter      = 80
    distortionParams.eqBandwidth   = 80
    soundEffect:load(distortionParams)

    local echoParams = EchoParams.new()
    echoParams.delay          = 0
    echoParams.leftRightDelay = 0
    echoParams.damping        = 0
    echoParams.feedback       = 0
    echoParams.spread         = 0
    soundEffect:load(distortionParams)

    soundEffect:reset()
    soundEffect:destroy()
  )"));
}

TEST_CASE("LuaAudio SoundEffectSlot", "[script][lua][audio]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new() -- Initializing the audio device & context, needed before all audio action

    local soundEffectSlot = SoundEffectSlot.new()

    assert(soundEffectSlot:getIndex() >= 0)
    soundEffectSlot:init()
    soundEffectSlot:loadEffect(SoundEffect.new())
    soundEffectSlot:destroy()
  )"));
}
#endif
