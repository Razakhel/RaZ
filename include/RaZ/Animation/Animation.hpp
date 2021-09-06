#pragma once

#ifndef RAZ_ANIMATION_HPP
#define RAZ_ANIMATION_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Animation/Keyframe.hpp"

#include <vector>

namespace Raz {

class Keyframe;

class Animation final : public Component {
public:
  explicit Animation(bool isPlaying = true) : m_isPlaying{ isPlaying } {}
  Animation(const Animation&) = delete;
  Animation(Animation&&) noexcept = default;

  float getFrameTime() const noexcept { return m_frameTime; }
  std::size_t getKeyframeCount() const noexcept { return m_keyframes.size(); }
  const Keyframe& getKeyframe(std::size_t index) const noexcept { return m_keyframes[index]; }
  Keyframe& getKeyframe(std::size_t index) noexcept { return m_keyframes[index]; }
  bool isPlaying() const noexcept { return m_isPlaying; }

  void setFrameTime(float frameTime) { m_frameTime = frameTime; }

  template <typename... Args>
  Keyframe& addKeyframe(Args&&... args) { return m_keyframes.emplace_back(std::forward<Args>(args)...); }
  void play() { m_isPlaying = true; }
  void pause() { m_isPlaying = false; }

  Animation& operator=(const Animation&) = delete;
  Animation& operator=(Animation&&) noexcept = default;

private:
  float m_frameTime {};
  std::vector<Keyframe> m_keyframes {};
  bool m_isPlaying {};
};

} // namespace Raz

#endif // RAZ_ANIMATION_HPP
