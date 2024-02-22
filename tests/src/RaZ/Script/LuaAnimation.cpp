#include "Catch.hpp"

#include "RaZ/Script/LuaWrapper.hpp"

TEST_CASE("LuaAnimation Skeleton", "[script][lua][animation]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local skeleton = Skeleton.new()
    skeleton       = Skeleton.new(0)
    skeleton       = BvhFormat.load(FilePath.new(RAZ_TESTS_ROOT .. "assets/animation/ànîm.bvh"))

    assert(skeleton:getNodeCount() == 5)

    local rootJoint = skeleton:getNode(0)
    assert(rootJoint:isRoot())

    rootJoint.rotation = Quaternionf.new(Radiansf.new(Constant.Pi), Axis.Y)
    rootJoint:rotate(Quaternionf.new(Radiansf.new(Constant.Pi), Axis.Z))
    rootJoint.translation = Vec3f.new(1)
    rootJoint:translate(Vec3f.new(1, 2, 3))
    assert(rootJoint.translation == Vec3f.new(2, 3, 4))
    assert(rootJoint.rotation == Quaternionf.new(0, 1, 0, 0))
    assert(rootJoint:computeTransformMatrix() == Mat4f.new(1,  0,  0, 2,
                                                           0, -1,  0, 3,
                                                           0,  0, -1, 4,
                                                           0,  0,  0, 1))

    assert(rootJoint:getChildCount() == 2)
    assert(rootJoint:getChild(0):getParentCount() == 1)
    assert(rootJoint:getChild(0):getParent(0) == rootJoint)
    assert(rootJoint:getChild(0):isLeaf())

    rootJoint:removeChildren(rootJoint:getChild(0), rootJoint:getChild(1))
    assert(rootJoint:isIsolated())
    rootJoint:addChildren(skeleton:getNode(1), skeleton:getNode(2))
    assert(rootJoint:getChildCount() == 2)

    local newJoint = skeleton:addNode()
    assert(skeleton:getNodeCount() == 6)

    skeleton:getNode(1):addParents(rootJoint, newJoint)
    assert(skeleton:getNode(1):getParentCount() == 2)
    skeleton:getNode(1):removeParents(rootJoint, newJoint)
    assert(skeleton:getNode(1):isRoot())

    skeleton:removeNode(newJoint)
    assert(skeleton:getNodeCount() == 5)
  )"));
}
