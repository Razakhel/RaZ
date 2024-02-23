#include "RaZ/Audio/AudioSystem.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Math/Vector.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Listener data", "[audio]") {
  const Raz::AudioSystem audio; // Initializing the audio device & context, needed before all audio action

  const Raz::Listener listener;

  CHECK(listener.recoverGain() == 1.f);
  CHECK(listener.recoverPosition() == Raz::Vec3f(0.f));
  CHECK(listener.recoverVelocity() == Raz::Vec3f(0.f));
  CHECK(listener.recoverForwardOrientation() == -Raz::Axis::Z);
  CHECK(listener.recoverUpOrientation() == Raz::Axis::Y);

  constexpr float gain = 0.25f;
  constexpr Raz::Vec3f position(1.f, 2.f, 3.f);
  constexpr Raz::Vec3f velocity(3.f, 2.f, 1.f);
  constexpr Raz::Vec3f forward = Raz::Axis::Y;
  constexpr Raz::Vec3f up = -Raz::Axis::X;

  listener.setGain(gain);
  listener.setPosition(position);
  listener.setVelocity(velocity);
  listener.setOrientation(forward, up);

  CHECK(listener.recoverGain() == gain);
  CHECK(listener.recoverPosition() == position);
  CHECK(listener.recoverVelocity() == velocity);
  CHECK(listener.recoverForwardOrientation() == forward);
  CHECK(listener.recoverUpOrientation() == up);
}
