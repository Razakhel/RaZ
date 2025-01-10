#include "RaZ/Audio/AudioData.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("WavFormat load/save", "[data]") {
  const Raz::AudioData origAudioData = Raz::WavFormat::load(RAZ_TESTS_ROOT "assets/sounds/notif_ting.wav");
  CHECK(origAudioData.format == Raz::AudioFormat::MONO_I16);
  CHECK(origAudioData.frequency == 48000);
  CHECK(origAudioData.buffer.size() == 608894);

  Raz::WavFormat::save("téstÊxpørt.wav", origAudioData);

  const Raz::AudioData savedAudioData = Raz::WavFormat::load("téstÊxpørt.wav");
  CHECK(savedAudioData.format == origAudioData.format);
  CHECK(savedAudioData.frequency == origAudioData.frequency);
  CHECK(savedAudioData.buffer == origAudioData.buffer);
}
