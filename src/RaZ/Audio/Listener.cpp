#include "RaZ/Audio/Listener.hpp"

#include <AL/al.h>

namespace Raz {

Listener::Listener(const Vec3f& position, const Vec3f& forwardDir, const Vec3f& upDir) {
  setPosition(position);
  setOrientation(forwardDir, upDir);
}

Listener::Listener(const Vec3f& position, const Mat3f& viewMatrix) {
  setPosition(position);
  setOrientation(viewMatrix);
}

void Listener::setPosition(const Vec3f& position) const noexcept {
  alListener3f(AL_POSITION, position[0], position[1], position[2]);

  if (alGetError() != AL_NO_ERROR)
    std::cerr << "[OpenAL] Error: Failed to set the listener's position." << std::endl;
}

void Listener::setVelocity(const Vec3f& velocity) const noexcept {
  alListener3f(AL_VELOCITY, velocity[0], velocity[1], velocity[2]);

  if (alGetError() != AL_NO_ERROR)
    std::cerr << "[OpenAL] Error: Failed to set the listener's velocity." << std::endl;
}

void Listener::setOrientation(const Vec3f& forwardDirection, const Vec3f& upDirection) const noexcept {
  assert("Error: The Listener's forward direction must be normalized." && FloatUtils::areNearlyEqual(forwardDirection.computeLength(), 1.f));
  assert("Error: The Listener's up direction must be normalized." && FloatUtils::areNearlyEqual(upDirection.computeLength(), 1.f));

  const Vector<float, 6> orientation(forwardDirection[0], forwardDirection[1], forwardDirection[2],
                                     upDirection[0], upDirection[1], upDirection[2]);
  alListenerfv(AL_ORIENTATION, orientation.getDataPtr());

  if (alGetError() != AL_NO_ERROR)
    std::cerr << "[OpenAL] Error: Failed to set the listener's orientation." << std::endl;
}

void Listener::setOrientation(const Mat3f& transformMatrix) const noexcept {
  // The transform matrix contains the forward & up directions in its 3rd & 2nd row, respectively
  setOrientation(transformMatrix.recoverRow(2).normalize(), transformMatrix.recoverRow(1).normalize());
}

} // namespace Raz
