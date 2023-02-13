#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerAnimationTypes() {
  sol::state& state = getState();

  {
    sol::usertype<SkeletonJoint> skeletonJoint = state.new_usertype<SkeletonJoint>("SkeletonJoint",
                                                                                   sol::constructors<SkeletonJoint(),
                                                                                                     SkeletonJoint(const Quaternionf&, const Vec3f&)>(),
                                                                                   sol::base_classes, sol::bases<GraphNode<SkeletonJoint>>());
    skeletonJoint["getParentCount"] = &SkeletonJoint::getParentCount;
    skeletonJoint["getParent"]      = PickNonConstOverload<std::size_t>(&SkeletonJoint::getParent);
    skeletonJoint["getChildCount"]  = &SkeletonJoint::getChildCount;
    skeletonJoint["getChild"]       = PickNonConstOverload<std::size_t>(&SkeletonJoint::getChild);
    skeletonJoint["isRoot"]         = &SkeletonJoint::isRoot;
    skeletonJoint["isLeaf"]         = &SkeletonJoint::isLeaf;
    skeletonJoint["isIsolated"]     = &SkeletonJoint::isIsolated;
    skeletonJoint["addParents"]     = [] (SkeletonJoint& j, sol::variadic_args args) { for (auto parent : args) j.addParents(parent); };
    skeletonJoint["removeParents"]  = [] (SkeletonJoint& j, sol::variadic_args args) { for (auto parent : args) j.removeParents(parent); };
    skeletonJoint["addChildren"]    = [] (SkeletonJoint& j, sol::variadic_args args) { for (auto child : args) j.addChildren(child); };
    skeletonJoint["removeChildren"] = [] (SkeletonJoint& j, sol::variadic_args args) { for (auto child : args) j.removeChildren(child); };

    skeletonJoint["rotation"]               = sol::property(&SkeletonJoint::getRotation, &SkeletonJoint::setRotation);
    skeletonJoint["translation"]            = sol::property(&SkeletonJoint::getTranslation, &SkeletonJoint::setTranslation);
    skeletonJoint["rotate"]                 = &SkeletonJoint::rotate;
    skeletonJoint["translate"]              = &SkeletonJoint::translate;
    skeletonJoint["computeTransformMatrix"] = &SkeletonJoint::computeTransformMatrix;
  }

  {
    sol::usertype<Skeleton> skeleton = state.new_usertype<Skeleton>("Skeleton",
                                                                    sol::constructors<Skeleton(),
                                                                                      Skeleton(std::size_t)>());
    skeleton["getNodeCount"] = &Skeleton::getNodeCount;
    skeleton["getNode"]      = PickNonConstOverload<std::size_t>(&Skeleton::getNode);
    skeleton["addNode"]      = &Skeleton::addNode<>;
    skeleton["removeNode"]   = &Skeleton::removeNode;
  }
}

} // namespace Raz
