#include "RaZ/Entity.hpp"
#include "RaZ/Data/BoundingVolumeHierarchy.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/MeshDistanceField.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("MeshDistanceField computation", "[data]") {
  // See: https://www.geogebra.org/m/nn5jtkrt

  // The MDF requires a definition of at least 2 on each axis
  CHECK_THROWS(Raz::MeshDistanceField(Raz::AABB({}, {}), 0, 0, 0));
  CHECK_THROWS(Raz::MeshDistanceField(Raz::AABB({}, {}), 1, 1, 1));
  CHECK_NOTHROW(Raz::MeshDistanceField(Raz::AABB({}, {}), 2, 2, 2));

  const Raz::AABB fieldBox(Raz::Vec3f(-1.f), Raz::Vec3f(1.f));

  Raz::MeshDistanceField mdf(fieldBox, 9, 9, 9); // Choosing odd numbers to get the exact center position
  CHECK(mdf.getDistance(0, 0, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(8, 8, 8) == std::numeric_limits<float>::max());

  CHECK_THROWS(mdf.compute(1)); // No BVH set

  Raz::BoundingVolumeHierarchy bvh;
  mdf.setBvh(bvh);

  mdf.compute(1); // The BVH is empty, nothing will be computed
  CHECK(mdf.getDistance(0, 0, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(8, 8, 8) == std::numeric_limits<float>::max());

  Raz::Entity mesh(0);
  mesh.addComponent<Raz::Mesh>(Raz::AABB(fieldBox.getMinPosition() * 0.5f, fieldBox.getMaxPosition() * 0.5f));
  bvh.build({ &mesh });

  mdf.compute(1); // A sample count too low isn't enough to find much intersection
  // Corners
  CHECK(mdf.getDistance(0, 0, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(8, 8, 8) == std::numeric_limits<float>::max());
  // Edges
  CHECK(mdf.getDistance(4, 0, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(0, 4, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(0, 0, 4) == std::numeric_limits<float>::max());
  // Faces
  CHECK(mdf.getDistance(0, 4, 4) == 0.5f); // The first sample direction should be +X
  CHECK(mdf.getDistance(4, 0, 4) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(4, 4, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(4, 4, 8) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(4, 8, 4) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(8, 4, 4) == std::numeric_limits<float>::max());
  // Center
  CHECK(mdf.getDistance(4, 4, 4) == -0.5f);

  mdf.compute(2);
  // Corners
  CHECK(mdf.getDistance(0, 0, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(8, 8, 8) == 1.f);
  // Edges
  CHECK(mdf.getDistance(4, 0, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(0, 4, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(0, 0, 4) == 1.f);
  // Faces
  CHECK_THAT(mdf.getDistance(0, 4, 4), IsNearlyEqualTo(0.577350259f));
  CHECK(mdf.getDistance(4, 0, 4) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(4, 4, 0) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(4, 4, 8) == std::numeric_limits<float>::max());
  CHECK(mdf.getDistance(4, 8, 4) == std::numeric_limits<float>::max());
  CHECK_THAT(mdf.getDistance(8, 4, 4), IsNearlyEqualTo(0.782987058f));
  // Center
  CHECK_THAT(mdf.getDistance(4, 4, 4), IsNearlyEqualTo(-0.577350259f));

  mdf.compute(100);
  // Corner to corner, should tend toward 0.87
  CHECK_THAT(mdf.getDistance(0, 0, 0), IsNearlyEqualTo(0.912753105f));
  CHECK_THAT(mdf.getDistance(8, 0, 0), IsNearlyEqualTo(0.966534495f));
  CHECK_THAT(mdf.getDistance(8, 8, 0), IsNearlyEqualTo(0.980392158f));
  CHECK_THAT(mdf.getDistance(0, 8, 0), IsNearlyEqualTo(1.06382978f));
  CHECK_THAT(mdf.getDistance(0, 8, 8), IsNearlyEqualTo(1.162790895f));
  CHECK_THAT(mdf.getDistance(0, 0, 8), IsNearlyEqualTo(1.045123935f));
  CHECK_THAT(mdf.getDistance(8, 0, 8), IsNearlyEqualTo(1.048231006f));
  CHECK_THAT(mdf.getDistance(8, 8, 8), IsNearlyEqualTo(1.132885695f));
  // Edge to edge, should tend toward 0.71
  CHECK_THAT(mdf.getDistance(4, 0, 0), IsNearlyEqualTo(0.724637687f));
  CHECK_THAT(mdf.getDistance(0, 4, 0), IsNearlyEqualTo(0.772372246f));
  CHECK_THAT(mdf.getDistance(0, 0, 4), IsNearlyEqualTo(0.749056816f));
  CHECK_THAT(mdf.getDistance(4, 8, 8), IsNearlyEqualTo(0.761324167f));
  CHECK_THAT(mdf.getDistance(8, 4, 8), IsNearlyEqualTo(0.76935935f));
  CHECK_THAT(mdf.getDistance(8, 8, 4), IsNearlyEqualTo(0.725454986f));
  // Side to side, should tend toward 0.5
  CHECK_THAT(mdf.getDistance(4, 4, 0), IsNearlyEqualTo(0.510620058f));
  CHECK_THAT(mdf.getDistance(0, 4, 4), IsNearlyEqualTo(0.503709972f));
  CHECK_THAT(mdf.getDistance(4, 0, 4), IsNearlyEqualTo(0.50505048f));
  CHECK_THAT(mdf.getDistance(4, 4, 8), IsNearlyEqualTo(0.501562536f));
  CHECK_THAT(mdf.getDistance(8, 4, 4), IsNearlyEqualTo(0.504095972f));
  CHECK_THAT(mdf.getDistance(4, 8, 4), IsNearlyEqualTo(0.50505048f));
  // Inside at mid-distance between center & corners, should tend toward -0.25
  CHECK_THAT(mdf.getDistance(3, 3, 3), IsNearlyEqualTo(-0.250781268f));
  CHECK_THAT(mdf.getDistance(3, 3, 5), IsNearlyEqualTo(-0.252047986f));
  CHECK_THAT(mdf.getDistance(3, 5, 3), IsNearlyEqualTo(-0.250781268f));
  CHECK_THAT(mdf.getDistance(3, 5, 5), IsNearlyEqualTo(-0.252047986f));
  CHECK_THAT(mdf.getDistance(5, 3, 3), IsNearlyEqualTo(-0.250781268f));
  CHECK_THAT(mdf.getDistance(5, 3, 5), IsNearlyEqualTo(-0.251854986f));
  CHECK_THAT(mdf.getDistance(5, 5, 3), IsNearlyEqualTo(-0.250781268f));
  CHECK_THAT(mdf.getDistance(5, 5, 5), IsNearlyEqualTo(-0.251854986f));
  // Center to side, should tend toward -0.5
  CHECK_THAT(mdf.getDistance(4, 4, 4), IsNearlyEqualTo(-0.501562536f));

  // Degenerate cases due to a grid definition too low, should tend toward 0 but fail to find an intersection other than the closest opposite inner face:

  // Right on each corner
  CHECK_THAT(mdf.getDistance(2, 2, 2), IsNearlyEqualTo(-1.010100961f));
  CHECK_THAT(mdf.getDistance(2, 2, 6), IsNearlyEqualTo(-1.048736811f));
  CHECK_THAT(mdf.getDistance(2, 6, 2), IsNearlyEqualTo(-1.010100961f));
  CHECK_THAT(mdf.getDistance(2, 6, 6), IsNearlyEqualTo(-1.003125072f));
  CHECK_THAT(mdf.getDistance(6, 2, 2), IsNearlyEqualTo(-1.022850156f));
  CHECK_THAT(mdf.getDistance(6, 2, 6), IsNearlyEqualTo(-1.030927777f));
  CHECK_THAT(mdf.getDistance(6, 6, 2), IsNearlyEqualTo(-1.045168638f));
  CHECK_THAT(mdf.getDistance(6, 6, 6), IsNearlyEqualTo(-1.008191943f));
  // Right in the middle of each edge
  CHECK_THAT(mdf.getDistance(2, 4, 2), IsNearlyEqualTo(-0.50505048f));
  CHECK_THAT(mdf.getDistance(2, 4, 6), IsNearlyEqualTo(-0.526315749f));
  CHECK_THAT(mdf.getDistance(4, 2, 2), IsNearlyEqualTo(-0.515975773f));
  CHECK_THAT(mdf.getDistance(4, 2, 6), IsNearlyEqualTo(-0.524368405f));
  CHECK_THAT(mdf.getDistance(4, 6, 2), IsNearlyEqualTo(-0.522584319f));
  CHECK_THAT(mdf.getDistance(4, 6, 6), IsNearlyEqualTo(-0.503709972f));
  CHECK_THAT(mdf.getDistance(6, 4, 2), IsNearlyEqualTo(-0.537634432f));
  CHECK_THAT(mdf.getDistance(6, 4, 6), IsNearlyEqualTo(-0.515463889f));
  // Right in the middle of each face
  CHECK_THAT(mdf.getDistance(2, 4, 4), IsNearlyEqualTo(-0.501562536f));
  CHECK_THAT(mdf.getDistance(6, 4, 4), IsNearlyEqualTo(-0.511425078f));
  CHECK_THAT(mdf.getDistance(4, 2, 4), IsNearlyEqualTo(-0.511425078f));
  CHECK_THAT(mdf.getDistance(4, 6, 4), IsNearlyEqualTo(-0.501562536f));
  CHECK_THAT(mdf.getDistance(4, 4, 2), IsNearlyEqualTo(-0.50505048f));
  CHECK_THAT(mdf.getDistance(4, 4, 6), IsNearlyEqualTo(-0.503709972f));
}

TEST_CASE("MeshDistanceField slices", "[data]") {
  // Creating a distance field with a single triangle inside
  //
  // -----^-----
  // |  /   \  |
  // |/_______\|

  Raz::Entity mesh(0);
  mesh.addComponent<Raz::Mesh>(Raz::Triangle(Raz::Vec3f(-1.f, -0.5f, 0.f), Raz::Vec3f(1.f, -0.5f, 0.f), Raz::Vec3f(0.f, 0.5f, 0.f)),
                               Raz::Vec2f(), Raz::Vec2f(), Raz::Vec2f());

  Raz::BoundingVolumeHierarchy bvh;
  bvh.build({ &mesh });

  Raz::MeshDistanceField mdf(Raz::AABB(Raz::Vec3f(-1.f, -0.5f, -0.25f), Raz::Vec3f(1.f, 0.5f, 0.25f)), 4, 3, 2);
  mdf.setBvh(bvh);
  mdf.compute(10);

  constexpr float backDistance  = -0.289784729f;
  constexpr float frontDistance = 0.271375328f;

  CHECK_THAT(mdf.getDistance(1, 1, 0), IsNearlyEqualTo(backDistance));
  CHECK_THAT(mdf.getDistance(1, 1, 1), IsNearlyEqualTo(frontDistance));

  const std::vector<Raz::Image> imageSlices = mdf.recoverSlices();
  REQUIRE(imageSlices.size() == 2); // The number of slices is equal to the MDF's depth definition

  for (const Raz::Image& depthSlice : imageSlices) {
    CHECK(depthSlice.getWidth() == 4);
    CHECK(depthSlice.getHeight() == 3);
    CHECK(depthSlice.getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(depthSlice.getDataType() == Raz::ImageDataType::FLOAT);
  }

  CHECK_THAT(imageSlices[0].recoverPixel<float>(1, 1), IsNearlyEqualTo(backDistance));
  CHECK_THAT(imageSlices[1].recoverPixel<float>(1, 1), IsNearlyEqualTo(frontDistance));
}
