#pragma once

#ifndef RAZ_ANIMATIONSYSTEM_HPP
#define RAZ_ANIMATIONSYSTEM_HPP

#include "RaZ/System.hpp"

#include <unordered_map>

namespace Raz {

class AnimationSystem final : public System {
public:
  AnimationSystem();

  bool update(float deltaTime) override;

protected:
  void linkEntity(const EntityPtr& entity) override;
  void unlinkEntity(const EntityPtr& entity) override;

private:
  std::unordered_map<Entity*, float> m_timers {};
};

} // namespace Raz

#endif // RAZ_ANIMATIONSYSTEM_HPP
