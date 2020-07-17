#include "Catch.hpp"

#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Threading.hpp"

TEST_CASE("Sound initialization") {
  Raz::AudioSystem audio; // Initializing the audio device & context, needed before all audio action

  Raz::Sound sound;
  CHECK(sound.getBufferIndex() != std::numeric_limits<unsigned int>::max());
}

TEST_CASE("Sound WAV import") {
  Raz::Sound sound;
  sound.load(RAZ_TESTS_ROOT + "assets/sounds/notif_ting.wav"s);

  CHECK(sound.getFormat() == Raz::SoundFormat::MONO_I16);
  CHECK(sound.getFrequency() == 48000);
}

TEST_CASE("Sound operations") {
  Raz::AudioSystem audio;
  Raz::Sound sound(RAZ_TESTS_ROOT + "assets/sounds/notif_ting.wav"s);

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
