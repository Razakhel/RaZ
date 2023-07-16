#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <AL/al.h>

#include <string>

namespace Raz {

namespace {

constexpr const char* recoverAlErrorStr(int errorCode) {
  switch (errorCode) {
    case AL_INVALID_NAME:      return "Invalid name";
    case AL_INVALID_ENUM:      return "Invalid enum";
    case AL_INVALID_VALUE:     return "Invalid value";
    case AL_INVALID_OPERATION: return "Invalid operation";
    case AL_OUT_OF_MEMORY:     return "Out of memory";
    case AL_NO_ERROR:          return "No error";
    default:                   return "Unknown error";
  }
}

inline void checkError(const std::string& errorMsg) {
  const int errorCode = alGetError();

  if (errorCode != AL_NO_ERROR)
    Logger::error("[OpenAL] " + errorMsg + " (" + recoverAlErrorStr(errorCode) + ").");
}

} // namespace

Listener::Listener(const Vec3f& position, const Vec3f& forwardDir, const Vec3f& upDir) {
  setPosition(position);
  setOrientation(forwardDir, upDir);
}

Listener::Listener(const Vec3f& position, const Mat3f& rotationMatrix) {
  setPosition(position);
  setOrientation(rotationMatrix);
}

void Listener::setGain(float gain) const noexcept {
  assert("Error: The listener's gain must be positive." && gain >= 0.f);

  alListenerf(AL_GAIN, gain);
  checkError("Failed to set the listener's gain");
}

float Listener::recoverGain() const noexcept {
  float gain {};

  alGetListenerf(AL_GAIN, &gain);
  checkError("Failed to recover the listener's gain");

  return gain;
}

void Listener::setPosition(const Vec3f& position) const noexcept {
  alListener3f(AL_POSITION, position.x(), position.y(), position.z());
  checkError("Failed to set the listener's position");
}

Vec3f Listener::recoverPosition() const noexcept {
  Vec3f position;

  alGetListener3f(AL_POSITION, &position.x(), &position.y(), &position.z());
  checkError("Failed to recover the listener's position");

  return position;
}

void Listener::setVelocity(const Vec3f& velocity) const noexcept {
  alListener3f(AL_VELOCITY, velocity.x(), velocity.y(), velocity.z());
  checkError("Failed to set the listener's velocity");
}

Vec3f Listener::recoverVelocity() const noexcept {
  Vec3f velocity;

  alGetListener3f(AL_VELOCITY, &velocity.x(), &velocity.y(), &velocity.z());
  checkError("Failed to recover the listener's velocity");

  return velocity;
}

void Listener::setOrientation(const Vec3f& forwardDirection) const noexcept {
  setOrientation(forwardDirection, Axis::Y);
}

void Listener::setOrientation(const Vec3f& forwardDirection, const Vec3f& upDirection) const noexcept {
  assert("Error: The Listener's forward direction must be normalized." && FloatUtils::areNearlyEqual(forwardDirection.computeLength(), 1.f));
  assert("Error: The Listener's up direction must be normalized." && FloatUtils::areNearlyEqual(upDirection.computeLength(), 1.f));

  const Vector<float, 6> orientation(forwardDirection.x(), forwardDirection.y(), forwardDirection.z(),
                                     upDirection.x(), upDirection.y(), upDirection.z());
  alListenerfv(AL_ORIENTATION, orientation.getDataPtr());
  checkError("Failed to set the listener's orientation");
}

void Listener::setOrientation(const Mat3f& rotationMatrix) const noexcept {
  setOrientation(-rotationMatrix.recoverColumn(2).normalize(), rotationMatrix.recoverColumn(1).normalize());
}

std::pair<Vec3f, Vec3f> Listener::recoverOrientation() const noexcept {
  Vector<float, 6> orientation;

  alGetListenerfv(AL_ORIENTATION, orientation.getDataPtr());
  checkError("Failed to recover the listener's orientation");

  return { Vec3f(orientation[0], orientation[1], orientation[2]), Vec3f(orientation[3], orientation[4], orientation[5]) };
}

Vec3f Listener::recoverForwardOrientation() const noexcept {
  return recoverOrientation().first;
}

Vec3f Listener::recoverUpOrientation() const noexcept {
  return recoverOrientation().second;
}

} // namespace Raz
