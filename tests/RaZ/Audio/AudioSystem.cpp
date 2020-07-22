#include "Catch.hpp"

#include "RaZ/Audio/AudioSystem.hpp"

TEST_CASE("AudioSystem initialization") {
  {
    Raz::AudioSystem audio;
    CHECK(!audio.recoverCurrentDevice().empty());
  }

  {
    Raz::AudioSystem audio("non-existing device");
    CHECK(audio.recoverCurrentDevice().empty());
  }
}

TEST_CASE("AudioSystem devices recovery") {
  const std::vector<std::string> devices = Raz::AudioSystem::recoverDevices();

  Raz::AudioSystem audio;
  const std::string currentDevice = audio.recoverCurrentDevice();

  CHECK(std::find(devices.cbegin(), devices.cend(), currentDevice) != devices.cend());
}
