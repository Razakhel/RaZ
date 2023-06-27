#pragma once

#ifndef RAZ_SCRIPTSYSTEM_HPP
#define RAZ_SCRIPTSYSTEM_HPP

#include "RaZ/System.hpp"

namespace Raz {

class ScriptSystem final : public System {
public:
  /// Default constructor.
  ScriptSystem();

  bool update(const FrameTimeInfo& timeInfo) override;
};

} // namespace Raz

#endif // RAZ_SCRIPTSYSTEM_HPP
