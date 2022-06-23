#include "Catch.hpp"

#include "RaZ/World.hpp"
#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Math/Transform.hpp"

TEST_CASE("AudioSystem accepted components") {
  Raz::World world(2);

  auto& audio = world.addSystem<Raz::AudioSystem>();

  Raz::Entity& sound    = world.addEntityWithComponent<Raz::Sound>();
  Raz::Entity& listener = world.addEntityWithComponents<Raz::Listener, Raz::Transform>(); // AudioSystem::update() needs a Listener with a Transform component

  world.update(0.f);

  CHECK(audio.containsEntity(sound));
  CHECK(audio.containsEntity(listener));
}

TEST_CASE("AudioSystem initialization") {
  {
    Raz::AudioSystem audio;
    CHECK_FALSE(audio.recoverCurrentDevice().empty()); // If it is actually empty, audio features won't be available on this platform
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
