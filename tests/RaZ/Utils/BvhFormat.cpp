#include "Catch.hpp"

#include "RaZ/Utils/BvhFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("BVH import") {
  const Raz::BvhFormat file(RAZ_TESTS_ROOT + "assets/animation/ànîm.bvh"s);

  CHECK(file.skeleton.getNodeCount() == 5);
  CHECK(file.skeleton.getNode(0).getTranslation() == Raz::Vec3f(1.f, 2.f, 3.f));
  CHECK(file.skeleton.getNode(1).getTranslation() == Raz::Vec3f(10.f, 10.f, 10.f));
  CHECK(file.skeleton.getNode(2).getTranslation() == Raz::Vec3f(20.f, 20.f, 20.f));
  CHECK(file.skeleton.getNode(3).getTranslation() == Raz::Vec3f(21.f, 21.f, 21.f));
  CHECK(file.skeleton.getNode(4).getTranslation() == Raz::Vec3f(22.f, 22.f, 22.f));
}
