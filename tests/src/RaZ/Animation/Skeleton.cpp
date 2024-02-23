#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Quaternion.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Raz::Literals;

TEST_CASE("Skeleton rotation chain", "[animation]") {
  // Creating a skeleton so that the following configuration is reproduced:
  //
  //             / child11
  //     / child1
  // root        \ child12
  //     \ child2
  //

  Raz::Skeleton skeleton;
  Raz::SkeletonJoint& root = skeleton.addNode();

  Raz::SkeletonJoint& child1  = skeleton.addNode();
  Raz::SkeletonJoint& child11 = skeleton.addNode();
  Raz::SkeletonJoint& child12 = skeleton.addNode();

  Raz::SkeletonJoint& child2 = skeleton.addNode();

  root.addChildren(child1, child2);
  child1.addChildren(child11, child12);

  const Raz::Quaternionf rootRotation(180_deg, Raz::Axis::Y);

  // Applying a rotation on the root joint
  root.rotate(rootRotation);

  // Checking that the rotation has been applied to the root joint...
  CHECK_THAT(root.getRotation(), IsNearlyEqualToQuaternion(rootRotation));
  CHECK_THAT(root.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotation.computeMatrix()));

  // ... and to every subsequent joint
  CHECK_THAT(child1.getRotation(), IsNearlyEqualToQuaternion(rootRotation));
  CHECK_THAT(child11.getRotation(), IsNearlyEqualToQuaternion(rootRotation));
  CHECK_THAT(child12.getRotation(), IsNearlyEqualToQuaternion(rootRotation));
  CHECK_THAT(child2.getRotation(), IsNearlyEqualToQuaternion(rootRotation));

  const Raz::Quaternionf child1Rotation(-180_deg, Raz::Axis::Y);

  // However, when applying a rotation on a joint further in the tree
  child1.rotate(child1Rotation);

  // Checking that child1 and its children have been affected...
  CHECK_THAT(child1.getRotation(), IsNearlyEqualToQuaternion(Raz::Quaternionf::identity()));
  CHECK_THAT(child11.getRotation(), IsNearlyEqualToQuaternion(Raz::Quaternionf::identity()));
  CHECK_THAT(child12.getRotation(), IsNearlyEqualToQuaternion(Raz::Quaternionf::identity()));

  // ... but not the other joints
  CHECK_THAT(root.getRotation(), IsNearlyEqualToQuaternion(rootRotation));
  CHECK_THAT(child2.getRotation(), IsNearlyEqualToQuaternion(rootRotation));

  root.removeChildren(child1, child2);
  CHECK(root.isIsolated());

  root.rotate(child1Rotation);

  // Since the root now doesn't have any child, only itself has been transformed
  CHECK_THAT(root.getRotation(), IsNearlyEqualToQuaternion(Raz::Quaternionf::identity()));
  CHECK_THAT(child1.getRotation(), IsNearlyEqualToQuaternion(Raz::Quaternionf::identity()));
  CHECK_THAT(child11.getRotation(), IsNearlyEqualToQuaternion(Raz::Quaternionf::identity()));
  CHECK_THAT(child12.getRotation(), IsNearlyEqualToQuaternion(Raz::Quaternionf::identity()));
  CHECK_THAT(child2.getRotation(), IsNearlyEqualToQuaternion(rootRotation));
}
