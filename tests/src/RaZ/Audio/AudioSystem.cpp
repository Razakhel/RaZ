#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/RigidBody.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Raz::Literals;

TEST_CASE("AudioSystem accepted components", "[audio]") {
  Raz::World world(2);

  auto& audio = world.addSystem<Raz::AudioSystem>();

  const Raz::Entity& sound    = world.addEntityWithComponent<Raz::Sound>();
  const Raz::Entity& listener = world.addEntityWithComponents<Raz::Listener, Raz::Transform>(); // The Listener entity requires a Transform component

  world.update({});

  CHECK(audio.containsEntity(sound));
  CHECK(audio.containsEntity(listener));
}

TEST_CASE("AudioSystem initialization", "[audio]") {
  {
    const Raz::AudioSystem audio;
    CHECK_FALSE(audio.recoverCurrentDevice().empty()); // If it is actually empty, audio features won't be available on this platform
  }

  {
    const Raz::AudioSystem audio("non-existing device");
    CHECK(audio.recoverCurrentDevice().empty());
  }
}

TEST_CASE("AudioSystem devices recovery", "[audio]") {
  const std::vector<std::string> devices = Raz::AudioSystem::recoverDevices();

  const Raz::AudioSystem audio;
  const std::string currentDevice = audio.recoverCurrentDevice();

  CHECK(std::find(devices.cbegin(), devices.cend(), currentDevice) != devices.cend());
}

TEST_CASE("AudioSystem attributes update", "[audio]") {
  Raz::World world;

  world.addSystem<Raz::AudioSystem>();

  Raz::Entity& listener   = world.addEntity();
  auto& listenerComp      = listener.addComponent<Raz::Listener>();
  auto& listenerTrans     = listener.addComponent<Raz::Transform>();
  auto& listenerRigidBody = listener.addComponent<Raz::RigidBody>(0.f, 0.f);

  Raz::Entity& sound1   = world.addEntity();
  auto& sound1Comp      = sound1.addComponent<Raz::Sound>();
  auto& sound1Trans     = sound1.addComponent<Raz::Transform>();
  auto& sound1RigidBody = sound1.addComponent<Raz::RigidBody>(0.f, 0.f);

  Raz::Entity& sound2   = world.addEntity();
  auto& sound2Comp      = sound2.addComponent<Raz::Sound>();
  auto& sound2Trans     = sound2.addComponent<Raz::Transform>();
  auto& sound2RigidBody = sound2.addComponent<Raz::RigidBody>(0.f, 0.f);

  world.update({});

  CHECK(listenerComp.recoverPosition() == listenerTrans.getPosition());
  CHECK(listenerComp.recoverForwardOrientation() == -Raz::Axis::Z);
  CHECK(listenerComp.recoverUpOrientation() == Raz::Axis::Y);
  CHECK(listenerComp.recoverVelocity() == listenerRigidBody.getVelocity());
  CHECK(sound1Comp.recoverPosition() == sound1Trans.getPosition());
  CHECK(sound1Comp.recoverVelocity() == sound1RigidBody.getVelocity());
  CHECK(sound2Comp.recoverPosition() == sound2Trans.getPosition());
  CHECK(sound2Comp.recoverVelocity() == sound2RigidBody.getVelocity());

  listenerTrans.translate(Raz::Vec3f(1.f));
  listenerTrans.rotate(-90_deg, -90_deg);
  listenerRigidBody.setVelocity(Raz::Vec3f(1.f));
  sound1Trans.translate(Raz::Vec3f(1.f));
  sound1RigidBody.setVelocity(Raz::Vec3f(1.f));
  sound2Trans.translate(Raz::Vec3f(-1.f));
  sound2RigidBody.setVelocity(Raz::Vec3f(-1.f));
  world.update({});

  CHECK(listenerComp.recoverPosition() == listenerTrans.getPosition());
  CHECK(listenerComp.recoverForwardOrientation() == -Raz::Axis::Y);
  CHECK(listenerComp.recoverUpOrientation() == Raz::Axis::X);
  CHECK(listenerComp.recoverVelocity() == listenerRigidBody.getVelocity());
  CHECK(sound1Comp.recoverPosition() == sound1Trans.getPosition());
  CHECK(sound1Comp.recoverVelocity() == sound1RigidBody.getVelocity());
  CHECK(sound2Comp.recoverPosition() == sound2Trans.getPosition());
  CHECK(sound2Comp.recoverVelocity() == sound2RigidBody.getVelocity());
}
