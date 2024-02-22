#include "Catch.hpp"

#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("WavFormat load/save", "[data]") {
  const Raz::Sound origSound = Raz::WavFormat::load(RAZ_TESTS_ROOT "assets/sounds/notif_ting.wav");
  // Since no AudioSystem has been created, the Sound's buffer index cannot be valid. This is not the purpose of this test anyway

  CHECK(origSound.getFormat() == Raz::AudioFormat::MONO_I16);
  CHECK(origSound.getFrequency() == 48000);

  Raz::WavFormat::save("téstÊxpørt.wav", origSound);

  const Raz::Sound savedSound = Raz::WavFormat::load("téstÊxpørt.wav");
  CHECK(savedSound.getFormat() == origSound.getFormat());
  CHECK(savedSound.getFrequency() == origSound.getFrequency());

  const std::vector<uint8_t>& origSoundData  = Raz::Internal::SoundAccess::getData(origSound);
  const std::vector<uint8_t>& savedSoundData = Raz::Internal::SoundAccess::getData(savedSound);
  REQUIRE(savedSoundData.size() == origSoundData.size());
  CHECK(std::equal(savedSoundData.cbegin(), savedSoundData.cend(), origSoundData.cbegin()));
}
