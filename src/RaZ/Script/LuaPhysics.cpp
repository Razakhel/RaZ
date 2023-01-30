#include "RaZ/Physics/Collider.hpp"
#include "RaZ/Physics/PhysicsSystem.hpp"
#include "RaZ/Physics/RigidBody.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerPhysicsTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Collider> collider = state.new_usertype<Collider>("Collider",
                                                                    sol::constructors<Collider()>(),
                                                                    sol::base_classes, sol::bases<Component>());
    collider["getShapeType"] = &Collider::getShapeType;
    collider["hasShape"]     = &Collider::hasShape;
    collider["getShape"]     = [] (Collider& c) { return &c.getShape(); };
    collider["setShape"]     = [] (Collider& c, Shape& s) { c.setShape(std::move(s)); };
    collider["intersects"]   = sol::overload(PickOverload<const Collider&>(&Collider::intersects),
                                             PickOverload<const Shape&>(&Collider::intersects),
                                             [] (Collider& c, const Ray& r) { return c.intersects(r); },
                                             PickOverload<const Ray&, RayHit*>(&Collider::intersects));
  }

  {
    sol::usertype<PhysicsSystem> physicsSystem = state.new_usertype<PhysicsSystem>("PhysicsSystem",
                                                                                   sol::constructors<PhysicsSystem()>(),
                                                                                   sol::base_classes, sol::bases<System>());
    physicsSystem["gravity"]  = sol::property(&PhysicsSystem::getGravity, &PhysicsSystem::setGravity);
    physicsSystem["friction"] = sol::property(&PhysicsSystem::getFriction, &PhysicsSystem::setFriction);
  }

  {
    sol::usertype<RigidBody> rigidBody = state.new_usertype<RigidBody>("RigidBody",
                                                                       sol::constructors<RigidBody(float, float)>(),
                                                                       sol::base_classes, sol::bases<Component>());
    rigidBody["mass"]       = sol::property(&RigidBody::getMass, &RigidBody::setMass);
    rigidBody["getInvMass"] = &RigidBody::getInvMass;
    rigidBody["bounciness"] = sol::property(&RigidBody::getBounciness, &RigidBody::setBounciness);
    rigidBody["velocity"]   = sol::property(&RigidBody::getVelocity, &RigidBody::setVelocity);
    rigidBody["forces"]     = sol::property(&RigidBody::getForces, &RigidBody::setForces<Vec3f>);
  }
}

} // namespace Raz
