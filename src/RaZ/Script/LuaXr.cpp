#include "RaZ/XR/XrSystem.hpp"
#include "RaZ/Script/LuaWrapper.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

void LuaWrapper::registerXrTypes() {
  sol::state& state = getState();

  {
    sol::usertype<XrSystem> xrSystem = state.new_usertype<XrSystem>("XrSystem",
                                                                    sol::constructors<XrSystem(const std::string&)>(),
                                                                    sol::base_classes, sol::bases<System>());
    xrSystem["getOptimalViewWidth"]  = &XrSystem::getOptimalViewWidth;
    xrSystem["getOptimalViewHeight"] = &XrSystem::getOptimalViewHeight;
  }
}

} // namespace Raz
