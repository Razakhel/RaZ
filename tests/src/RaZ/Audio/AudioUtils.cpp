#include "RaZ/Audio/AudioData.hpp"
#include "RaZ/Audio/AudioUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("AudioUtils convert to mono", "[audio]") {
  Raz::AudioData monoDataU8{
    Raz::AudioFormat::MONO_U8,
    44100,
    { 0, 2, 4, 8 }
  };
  Raz::AudioUtils::convertToMono(monoDataU8);
  // The data is already mono and must remain unchanged
  CHECK(monoDataU8.format == Raz::AudioFormat::MONO_U8);
  CHECK(monoDataU8.frequency == 44100);
  CHECK(monoDataU8.buffer == std::vector<uint8_t>{ 0, 2, 4, 8 });

  Raz::AudioData stereoDataU8{
    Raz::AudioFormat::STEREO_U8,
    44100,
    { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 } // Must be a multiple of 2 (1 byte per channel * 2 channels (left & right))
  };
  Raz::AudioUtils::convertToMono(stereoDataU8);
  CHECK(stereoDataU8.format == Raz::AudioFormat::MONO_U8);
  CHECK(stereoDataU8.frequency == 44100);
  CHECK(stereoDataU8.buffer == std::vector<uint8_t>{ 1, 5, 9, 13, 17, 21 });

  Raz::AudioData stereoDataI16{
    Raz::AudioFormat::STEREO_I16,
    44100,
    { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 } // Must be a multiple of 4 (2 bytes per channel * 2 channels (left & right))
  };
  Raz::AudioUtils::convertToMono(stereoDataI16);
  CHECK(stereoDataI16.format == Raz::AudioFormat::MONO_I16);
  CHECK(stereoDataI16.frequency == 44100);
  CHECK(stereoDataI16.buffer == std::vector<uint8_t>{ 2, 4, 10, 12, 18, 20 });
}
