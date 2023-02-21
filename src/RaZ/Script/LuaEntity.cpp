#include "RaZ/Entity.hpp"
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Physics/Collider.hpp"
#include "RaZ/Physics/RigidBody.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

template <typename... Args>
auto bindComponents() {
  return sol::overload([] (Entity& entity, Args& comp) -> Args& { return entity.addComponent<Args>(std::move(comp)); }...);
}

void LuaWrapper::registerEntityTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Entity> entity = state.new_usertype<Entity>("Entity",
                                                              sol::constructors<Entity(std::size_t),
                                                                                Entity(std::size_t, bool)>());
    entity["getId"]                = &Entity::getId;
    entity["isEnabled"]            = &Entity::isEnabled;
    entity["getEnabledComponents"] = &Entity::getEnabledComponents;
    entity["enable"]               = sol::overload([] (Entity& e) { e.enable(); },
                                                   PickOverload<bool>(&Entity::enable));
    entity["disable"]              = &Entity::disable;
    entity["addComponent"]         = bindComponents<Camera,
                                                    Collider,
                                                    Light,
                                                    Listener,
                                                    Mesh,
                                                    MeshRenderer,
                                                    RigidBody,
                                                    Sound,
                                                    Transform>();
  }
}

} // namespace Raz
