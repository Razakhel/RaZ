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

private:
  /// Links the entity to the system, registering the entity to the script and calling the script's setup function.
  /// \param entity Entity to be linked.
  void linkEntity(const EntityPtr& entity) override;
};

} // namespace Raz

#endif // RAZ_SCRIPTSYSTEM_HPP
