#pragma once

#ifndef RAZ_LISTENER_HPP
#define RAZ_LISTENER_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Listener final : public Component {
public:
  Listener() = default;
  explicit Listener(const Vec3f& position) { setPosition(position); }
  Listener(const Vec3f& position, const Vec3f& forwardDirection, const Vec3f& upDirection);
  Listener(const Vec3f& position, const Mat3f& viewMatrix);
  Listener(const Listener&) = delete;
  Listener(Listener&& listener) noexcept = default;

  void setPosition(const Vec3f& position) const noexcept;
  Vec3f recoverPosition() const noexcept;
  void setVelocity(const Vec3f& velocity) const noexcept;
  Vec3f recoverVelocity() const noexcept;
  void setOrientation(const Vec3f& forwardDirection, const Vec3f& upDirection = Axis::Y) const noexcept;
  void setOrientation(const Mat3f& transformMatrix) const noexcept;
  std::pair<Vec3f, Vec3f> recoverOrientation() const noexcept;
  Vec3f recoverForwardOrientation() const noexcept { return recoverOrientation().first; }
  Vec3f recoverUpOrientation() const noexcept { return recoverOrientation().second; }

  Listener& operator=(const Listener&) = delete;
  Listener& operator=(Listener&& listener) noexcept = default;
};

} // namespace Raz

#endif // RAZ_LISTENER_HPP
