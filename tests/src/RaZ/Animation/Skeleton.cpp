#include "Catch.hpp"

#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Math/Quaternion.hpp"

using namespace Raz::Literals;

TEST_CASE("Skeleton rotation chain") {
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

  const Raz::Mat4f rootRotationMat = rootRotation.computeMatrix();

  // Checking that the rotation has been applied to the root joint...
  CHECK_THAT(root.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotationMat));

  // ... and to every subsequent joint
  CHECK_THAT(child1.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotationMat));
  CHECK_THAT(child11.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotationMat));
  CHECK_THAT(child12.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotationMat));
  CHECK_THAT(child2.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotationMat));

  const Raz::Quaternionf child1Rotation(180_deg, Raz::Axis::X);

  // However, when applying a rotation on a joint further in the tree
  child1.rotate(child1Rotation);

  const Raz::Mat4f resChild1RotationMat = (rootRotation * child1Rotation).computeMatrix();

  // Checking that child1 and its children have been affected...
  CHECK_THAT(child1.computeTransformMatrix(), IsNearlyEqualToMatrix(resChild1RotationMat));
  CHECK_THAT(child11.computeTransformMatrix(), IsNearlyEqualToMatrix(resChild1RotationMat));
  CHECK_THAT(child12.computeTransformMatrix(), IsNearlyEqualToMatrix(resChild1RotationMat));

  // ... but not the other joints
  CHECK_THAT(root.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotationMat));
  CHECK_THAT(child2.computeTransformMatrix(), IsNearlyEqualToMatrix(rootRotationMat));
}
