#include "Catch.hpp"

#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Data/BvhFormat.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("BvhFormat load") {
  const Raz::Skeleton skeleton = Raz::BvhFormat::load(RAZ_TESTS_ROOT "assets/animation/ànîm.bvh");

  CHECK(skeleton.getNodeCount() == 5);
  CHECK(skeleton.getNode(0).getTranslation() == Raz::Vec3f(1.f, 2.f, 3.f));
  CHECK(skeleton.getNode(1).getTranslation() == Raz::Vec3f(10.f, 10.f, 10.f));
  CHECK(skeleton.getNode(2).getTranslation() == Raz::Vec3f(20.f, 20.f, 20.f));
  CHECK(skeleton.getNode(3).getTranslation() == Raz::Vec3f(21.f, 21.f, 21.f));
  CHECK(skeleton.getNode(4).getTranslation() == Raz::Vec3f(22.f, 22.f, 22.f));
}
