#include "Catch.hpp"

#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("Sound import WAV") {
  Raz::AudioSystem audio; // Initializing the audio device & context, needed before all audio action

  Raz::Sound sound;
  CHECK(sound.getBufferIndex() != std::numeric_limits<unsigned int>::max());

  sound.load(RAZ_TESTS_ROOT + "assets/sounds/notif_ting.wav"s);
  CHECK(sound.getFormat() == Raz::SoundFormat::MONO_I16);
  CHECK(sound.getFrequency() == 48000);
}
