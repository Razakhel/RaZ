#pragma once

#ifndef RAZ_ANIMATIONSYSTEM_HPP
#define RAZ_ANIMATIONSYSTEM_HPP

#include "RaZ/System.hpp"

namespace Raz {

class AnimationSystem : public System {
public:
  AnimationSystem();

  bool update(float deltaTime) override;
};

} // namespace Raz

#endif // RAZ_ANIMATIONSYSTEM_HPP
