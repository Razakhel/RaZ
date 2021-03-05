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
  Animation() = default;

  std::size_t getKeyframeCount() const noexcept { return m_keyframes.size(); }
  const Keyframe& getKeyframe(std::size_t index) const noexcept { return m_keyframes[index]; }
  Keyframe& getKeyframe(std::size_t index) noexcept { return m_keyframes[index]; }

  template <typename... Args>
  Keyframe& addKeyframe(Args&&... args) { return m_keyframes.emplace_back(std::forward<Args>(args)...); }

private:
  std::vector<Keyframe> m_keyframes {};
};

} // namespace Raz

#endif // RAZ_ANIMATION_HPP
