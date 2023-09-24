#include "RaZ/Entity.hpp"
#if defined(RAZ_USE_AUDIO)
#include "RaZ/Audio/Listener.hpp"
#include "RaZ/Audio/Sound.hpp"
#endif
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
#if defined(RAZ_USE_AUDIO)
                                                    Listener,
#endif
                                                    Mesh,
                                                    MeshRenderer,
                                                    RigidBody,
#if defined(RAZ_USE_AUDIO)
                                                    Sound,
#endif
                                                    Transform>();
    entity["hasCamera"]            = &Entity::hasComponent<Camera>;
    entity["hasCollider"]          = &Entity::hasComponent<Collider>;
    entity["hasLight"]             = &Entity::hasComponent<Light>;
#if defined(RAZ_USE_AUDIO)
    entity["hasListener"]          = &Entity::hasComponent<Listener>;
#endif
    entity["hasMesh"]              = &Entity::hasComponent<Mesh>;
    entity["hasMeshRenderer"]      = &Entity::hasComponent<MeshRenderer>;
    entity["hasRigidBody"]         = &Entity::hasComponent<RigidBody>;
#if defined(RAZ_USE_AUDIO)
    entity["hasSound"]             = &Entity::hasComponent<Sound>;
#endif
    entity["hasTransform"]         = &Entity::hasComponent<Transform>;
    entity["getCamera"]            = [] (Entity& e) { return &e.getComponent<Camera>(); };
    entity["getCollider"]          = [] (Entity& e) { return &e.getComponent<Collider>(); };
    entity["getLight"]             = [] (Entity& e) { return &e.getComponent<Light>(); };
#if defined(RAZ_USE_AUDIO)
    entity["getListener"]          = [] (Entity& e) { return &e.getComponent<Listener>(); };
#endif
    entity["getMesh"]              = [] (Entity& e) { return &e.getComponent<Mesh>(); };
    entity["getMeshRenderer"]      = [] (Entity& e) { return &e.getComponent<MeshRenderer>(); };
    entity["getRigidBody"]         = [] (Entity& e) { return &e.getComponent<RigidBody>(); };
#if defined(RAZ_USE_AUDIO)
    entity["getSound"]             = [] (Entity& e) { return &e.getComponent<Sound>(); };
#endif
    entity["getTransform"]         = [] (Entity& e) { return &e.getComponent<Transform>(); };
  }
}

} // namespace Raz
