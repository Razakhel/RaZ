#pragma once

#ifndef RAZ_LISTENER_HPP
#define RAZ_LISTENER_HPP

#include "RaZ/Component.hpp"

namespace Raz {

template <typename T, std::size_t W, std::size_t H>
class Matrix;
using Mat3f = Matrix<float, 3, 3>;

template <typename T, std::size_t Size>
class Vector;
using Vec3f = Vector<float, 3>;

class Listener final : public Component {
public:
  Listener() = default;
  explicit Listener(const Vec3f& position) noexcept { setPosition(position); }
  Listener(const Vec3f& position, const Vec3f& forwardDirection, const Vec3f& upDirection) noexcept;
  Listener(const Vec3f& position, const Mat3f& rotationMatrix) noexcept;
  Listener(const Listener&) = delete;
  Listener(Listener&&) noexcept = default;

  /// Sets the listener's gain (master volume).
  /// \param gain Listener's gain; must be positive. 1 is the default.
  void setGain(float gain) const noexcept;
  /// Recovers the listener's gain (master volume).
  /// \return Listener's gain.
  float recoverGain() const noexcept;
  void setPosition(const Vec3f& position) const noexcept;
  Vec3f recoverPosition() const noexcept;
  void setVelocity(const Vec3f& velocity) const noexcept;
  Vec3f recoverVelocity() const noexcept;
  /// Sets the listener's forward orientation. Uses the Y axis as the up direction.
  /// \param forwardDirection Forward direction to be set.
  void setOrientation(const Vec3f& forwardDirection) const noexcept;
  /// Sets the listener's forward & up orientation.
  /// \param forwardDirection Forward direction to be set.
  /// \param upDirection Up direction to be set.
  void setOrientation(const Vec3f& forwardDirection, const Vec3f& upDirection) const noexcept;
  /// Sets the listener's forward & up orientation, respectively taken from the given rotation matrix's 3rd & 2nd columns.
  /// \note The forward direction is reversed, as the engine uses a right-handed coordinate system with its Z pointing to the viewer.
  /// \param rotationMatrix Rotation matrix from which to extract the orientation.
  void setOrientation(const Mat3f& rotationMatrix) const noexcept;
  std::pair<Vec3f, Vec3f> recoverOrientation() const noexcept;
  Vec3f recoverForwardOrientation() const noexcept;
  Vec3f recoverUpOrientation() const noexcept;

  Listener& operator=(const Listener&) = delete;
  Listener& operator=(Listener&&) noexcept = default;
};

} // namespace Raz

#endif // RAZ_LISTENER_HPP
