#include "Catch.hpp"

#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Threading.hpp"

TEST_CASE("Sound initialization", "[audio]") {
  const Raz::AudioSystem audio; // Initializing the audio device & context, needed before all audio action

  const Raz::Sound sound;
  CHECK(sound.getBufferIndex() != std::numeric_limits<unsigned int>::max());
}

TEST_CASE("Sound move", "[audio]") {
  const Raz::AudioSystem audio;

  Raz::Sound sound = Raz::WavFormat::load(RAZ_TESTS_ROOT "assets/sounds/notif_ting.wav");
  REQUIRE(sound.getBufferIndex() != std::numeric_limits<unsigned int>::max());

  const unsigned int bufferIndex = sound.getBufferIndex();
  const Raz::AudioFormat format  = sound.getFormat();
  const int frequency   = sound.getFrequency();
  constexpr float pitch = 0.123f;
  constexpr float gain  = 0.987f;
  constexpr Raz::Vec3f position(1.f, 2.f, 3.f);
  constexpr Raz::Vec3f velocity(3.f, 2.f, 1.f);

  sound.setPitch(pitch);
  sound.setGain(gain);
  sound.setPosition(position);
  sound.setVelocity(velocity);

  // Move constructor

  Raz::Sound movedSoundCtor(std::move(sound));

  // The new sound has the same values as the original one
  CHECK(movedSoundCtor.getBufferIndex() == bufferIndex);
  CHECK(movedSoundCtor.getFormat() == format);
  CHECK(movedSoundCtor.getFrequency() == frequency);
  CHECK(movedSoundCtor.recoverPitch() == pitch);
  CHECK(movedSoundCtor.recoverGain() == gain);
  CHECK(movedSoundCtor.recoverPosition() == position);
  CHECK(movedSoundCtor.recoverVelocity() == velocity);

  // The moved sound is now invalid
  CHECK(sound.getBufferIndex() == std::numeric_limits<unsigned int>::max());
  // An invalid sound's parameters are all 0
  CHECK(sound.recoverPitch() == 0.f);
  CHECK(sound.recoverGain() == 0.f);
  CHECK(sound.recoverPosition() == Raz::Vec3f(0.f));
  CHECK(sound.recoverVelocity() == Raz::Vec3f(0.f));

  // Move assignment operator

  Raz::Sound movedSoundOp;

  const unsigned int movedSoundOpBufferIndex = movedSoundOp.getBufferIndex();
  constexpr float movedSoundOpPitch = 0.456f;
  constexpr float movedSoundOpGain  = 0.654f;
  constexpr Raz::Vec3f movedSoundOpPosition(10.f, 20.f, 30.f);
  constexpr Raz::Vec3f movedSoundOpVelocity(30.f, 20.f, 10.f);

  movedSoundOp.setPitch(movedSoundOpPitch);
  movedSoundOp.setGain(movedSoundOpGain);
  movedSoundOp.setPosition(movedSoundOpPosition);
  movedSoundOp.setVelocity(movedSoundOpVelocity);

  movedSoundOp = std::move(movedSoundCtor);

  CHECK(movedSoundOp.getBufferIndex() == bufferIndex);
  CHECK(movedSoundOp.getFormat() == format);
  CHECK(movedSoundOp.getFrequency() == frequency);
  CHECK(movedSoundOp.recoverPitch() == pitch);
  CHECK(movedSoundOp.recoverGain() == gain);
  CHECK(movedSoundOp.recoverPosition() == position);
  CHECK(movedSoundOp.recoverVelocity() == velocity);

  // The moved-to & moved-from sounds now have their indices swapped
  CHECK(movedSoundCtor.getBufferIndex() == movedSoundOpBufferIndex);
  CHECK(movedSoundCtor.recoverPitch() == movedSoundOpPitch);
  CHECK(movedSoundCtor.recoverGain() == movedSoundOpGain);
  CHECK(movedSoundCtor.recoverPosition() == movedSoundOpPosition);
  CHECK(movedSoundCtor.recoverVelocity() == movedSoundOpVelocity);
}

TEST_CASE("Sound operations", "[audio]") {
  const Raz::AudioSystem audio;

  const Raz::Sound sound = Raz::WavFormat::load(RAZ_TESTS_ROOT "assets/sounds/notif_ting.wav");
  REQUIRE(sound.getBufferIndex() != std::numeric_limits<unsigned int>::max());

  sound.setGain(0.f); // Nobody wants his ears assaulted by a wild sound when launching tests

  sound.play();
  CHECK(sound.isPlaying());

  Raz::Threading::sleep(50); // So that the sound is actually played for a tiny duration

  sound.pause();
  CHECK(sound.isPaused());

  const float elapsedMinutes = sound.recoverElapsedTime();
  CHECK(elapsedMinutes > 0.f);

  sound.stop();
  CHECK(sound.isStopped());

  sound.rewind();
  CHECK(sound.recoverState() == Raz::SoundState::INITIAL);
}
