#include "Catch.hpp"

#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Threading.hpp"

TEST_CASE("Sound initialization") {
  Raz::AudioSystem audio; // Initializing the audio device & context, needed before all audio action

  Raz::Sound sound;
  CHECK(sound.getBufferIndex() != std::numeric_limits<unsigned int>::max());
}

TEST_CASE("Sound move") {
  Raz::AudioSystem audio;
  Raz::Sound sound = Raz::WavFormat::load(RAZ_TESTS_ROOT + "assets/sounds/notif_ting.wav"s);

  constexpr Raz::Vec3f position(1.f, 2.f, 3.f);
  constexpr Raz::Vec3f velocity(3.f, 2.f, 1.f);

  sound.setPosition(position);
  sound.setVelocity(velocity);

  const unsigned int bufferIndex = sound.getBufferIndex();

  Raz::Sound movedSound(std::move(sound)); // Move constructor

  CHECK(sound.getBufferIndex() == std::numeric_limits<unsigned int>::max());
  CHECK(sound.recoverPosition() == Raz::Vec3f(0.f));
  CHECK(sound.recoverVelocity() == Raz::Vec3f(0.f));

  CHECK(movedSound.getBufferIndex() == bufferIndex);
  CHECK(movedSound.recoverPosition() == position);
  CHECK(movedSound.recoverVelocity() == velocity);

  sound = std::move(movedSound); // Move assignment operator

  CHECK(sound.getBufferIndex() == bufferIndex);
  CHECK(sound.recoverPosition() == position);
  CHECK(sound.recoverVelocity() == velocity);

  CHECK(movedSound.getBufferIndex() == std::numeric_limits<unsigned int>::max());
  CHECK(movedSound.recoverPosition() == Raz::Vec3f(0.f));
  CHECK(movedSound.recoverVelocity() == Raz::Vec3f(0.f));
}

TEST_CASE("Sound WAV import") {
  Raz::Sound sound = Raz::WavFormat::load(RAZ_TESTS_ROOT + "assets/sounds/notif_ting.wav"s);

  CHECK(sound.getFormat() == Raz::SoundFormat::MONO_I16);
  CHECK(sound.getFrequency() == 48000);
}

TEST_CASE("Sound operations") {
  Raz::AudioSystem audio;
  Raz::Sound sound = Raz::WavFormat::load(RAZ_TESTS_ROOT + "assets/sounds/notif_ting.wav"s);
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
