#include "Catch.hpp"

#include "RaZ/Script/LuaWrapper.hpp"

TEST_CASE("LuaAudio AudioSystem") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new()
    audioSystem       = AudioSystem.new(AudioSystem.recoverDevices()[1])

    assert(audioSystem:recoverCurrentDevice() ~= "")
    audioSystem:openDevice("invalid device")
    assert(audioSystem:recoverCurrentDevice() == "")

    assert(audioSystem:getAcceptedComponents() ~= nil)
    assert(not audioSystem:containsEntity(Entity.new(0)))
    assert(audioSystem:update(0))
    assert(audioSystem:step(0))
    audioSystem:destroy()
  )"));
}

TEST_CASE("LuaAudio Listener") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new() -- Initializing the audio device & context, needed before all audio action

    local listener = Listener.new()
    listener       = Listener.new(Vec3f.new(1))
    listener       = Listener.new(Vec3f.new(1), Axis.Z, Axis.Y)
    listener       = Listener.new(Vec3f.new(1), Mat3f.identity())

    listener.position = Vec3f.new(1, 2, 3)
    assert(listener.position == Vec3f.new(1, 2, 3))

    listener.velocity = Vec3f.new(0, 0, 10)
    assert(listener.velocity == Vec3f.new(0, 0, 10))

    listener:setOrientation(Axis.Z, Axis.Y)
    listener:setOrientation(Mat3f.identity())

    local forward, up = listener:recoverOrientation()
    assert(forward == Axis.Z)
    assert(up == Axis.Y)
    assert(listener:recoverForwardOrientation() == Axis.Z)
    assert(listener:recoverUpOrientation() == Axis.Y)
  )"));
}

TEST_CASE("LuaAudio Sound") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local audioSystem = AudioSystem.new() -- Initializing the audio device & context, needed before all audio action

    local sound = Sound.new()
    sound       = WavFormat.load(FilePath.new(RAZ_TESTS_ROOT .. "assets/sounds/notif_ting.wav"))

    assert(sound:getBufferIndex() >= 0)
    assert(sound:getFormat() == AudioFormat.MONO_I16)
    assert(sound:getFrequency() == 48000)

    sound:init()
    sound:load()

    sound.pitch = 0.5
    assert(sound.pitch == 0.5)

    sound.gain = 0
    assert(sound.gain == 0)

    sound.position = Vec3f.new(1, 2, 3)
    assert(sound.position == Vec3f.new(1, 2, 3))

    sound.velocity = Vec3f.new(0, 0, 10)
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
}
