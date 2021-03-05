#include "Catch.hpp"

#include "RaZ/Utils/BvhFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("BVH import") {
  const Raz::BvhFormat file(RAZ_TESTS_ROOT + "assets/animations/ànîm.bvh"s);

  // Skeleton

  CHECK(file.skeleton.getNodeCount() == 5);
  CHECK(file.skeleton.getNode(0).getTranslation() == Raz::Vec3f(1.f, 2.f, 3.f));
  CHECK(file.skeleton.getNode(1).getTranslation() == Raz::Vec3f(10.f, 10.f, 10.f));
  CHECK(file.skeleton.getNode(2).getTranslation() == Raz::Vec3f(20.f, 20.f, 20.f));
  CHECK(file.skeleton.getNode(3).getTranslation() == Raz::Vec3f(21.f, 21.f, 21.f));
  CHECK(file.skeleton.getNode(4).getTranslation() == Raz::Vec3f(22.f, 22.f, 22.f));

  // Animation

  CHECK(file.animation.getKeyframeCount() == 2);

  {
    const Raz::Keyframe& keyframe = file.animation.getKeyframe(0);

    CHECK(keyframe.getKeyTime() == 0.033333f);
    CHECK(keyframe.getJointTransformCount() == file.skeleton.getNodeCount());

    CHECK(keyframe.getJointTransform(0).getRotation() == Raz::Quaternionf(0.998057663, 0.0337132, 0.0444906, 0.0276579));
    CHECK(keyframe.getJointTransform(0).getTranslation() == Raz::Vec3f(0.f, 1.f, 2.f));

    CHECK(keyframe.getJointTransform(1).getRotation() == Raz::Quaternionf(0.994561672, 0.0644603, 0.0663437, 0.0478584));
    CHECK(keyframe.getJointTransform(1).getTranslation() == Raz::Vec3f(0.f));

    CHECK(keyframe.getJointTransform(2).getRotation() == Raz::Quaternionf(0.989207, 0.0939784, 0.08838, 0.0694729));
    CHECK(keyframe.getJointTransform(2).getTranslation() == Raz::Vec3f(0.f));

    CHECK(keyframe.getJointTransform(3).getRotation() == Raz::Quaternionf(0.982205749, 0.124400832, 0.108678, 0.089362));
    CHECK(keyframe.getJointTransform(3).getTranslation() == Raz::Vec3f(0.f));

    CHECK(keyframe.getJointTransform(4).getRotation() == Raz::Quaternionf(0.973697, 0.155572, 0.127152532, 0.107441));
    CHECK(keyframe.getJointTransform(4).getTranslation() == Raz::Vec3f(0.f));
  }

  {
    const Raz::Keyframe& keyframe = file.animation.getKeyframe(1);

    CHECK(keyframe.getKeyTime() == 0.033333f);
    CHECK(keyframe.getJointTransformCount() == file.skeleton.getNodeCount());

    CHECK(keyframe.getJointTransform(0).getRotation() == Raz::Quaternionf(0.979321539, 0.099087, 0.116802841, 0.132167399));
    CHECK(keyframe.getJointTransform(0).getTranslation() == Raz::Vec3f(17.f, 16.f, 15.f));

    CHECK(keyframe.getJointTransform(1).getRotation() == Raz::Quaternionf(0.989207, 0.0939784, 0.0694729, 0.08838));
    CHECK(keyframe.getJointTransform(1).getTranslation() == Raz::Vec3f(0.f));

    CHECK(keyframe.getJointTransform(2).getRotation() == Raz::Quaternionf(0.994561672, 0.0644603, 0.0478584, 0.0663437));
    CHECK(keyframe.getJointTransform(2).getTranslation() == Raz::Vec3f(0.f));

    CHECK(keyframe.getJointTransform(3).getRotation() == Raz::Quaternionf(0.998137414, 0.0359955, 0.0246143, 0.0426652));
    CHECK(keyframe.getJointTransform(3).getTranslation() == Raz::Vec3f(0.f));

    CHECK(keyframe.getJointTransform(4).getRotation() == Raz::Quaternionf(0.999809623, 0.00872521, -0.000152299, 0.0174517));
    CHECK(keyframe.getJointTransform(4).getTranslation() == Raz::Vec3f(0.f));
  }
}
