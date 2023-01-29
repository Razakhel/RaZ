#include "Catch.hpp"

#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Data/BvhFormat.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("BvhFormat load") {
  const Raz::Skeleton skeleton = Raz::BvhFormat::load(RAZ_TESTS_ROOT "assets/animation/ànîm.bvh");

  //                child1 (10, 10, 10)
  //               /
  // root (1, 2, 3)                    child21 (21, 21, 21)
  //              \                   /
  //               child2 (20, 20, 20)
  //                                  \
  //                                   child22 (22, 22, 22)

  CHECK(skeleton.getNodeCount() == 5);

  const Raz::SkeletonJoint& root = skeleton.getNode(0);
  CHECK(root.getTranslation() == Raz::Vec3f(1.f, 2.f, 3.f));

  REQUIRE(root.getChildCount() == 2);
  CHECK(root.getChild(0).getTranslation() == Raz::Vec3f(10.f, 10.f, 10.f));
  CHECK(root.getChild(1).getTranslation() == Raz::Vec3f(20.f, 20.f, 20.f));

  REQUIRE(root.getChild(1).getChildCount() == 2);
  CHECK(root.getChild(1).getChild(0).getTranslation() == Raz::Vec3f(21.f, 21.f, 21.f));
  CHECK(root.getChild(1).getChild(1).getTranslation() == Raz::Vec3f(22.f, 22.f, 22.f));
}
