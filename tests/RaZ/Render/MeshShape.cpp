#include "Catch.hpp"

#include "RaZ/Render/Mesh.hpp"

TEST_CASE("UV sphere mesh from Sphere") {
  const Raz::Sphere sphere(Raz::Vec3f(1.f, 2.f, 3.f), 2.5f);

  // UV sphere mesh with 10 splits in latitude/longitude
  {
    Raz::Mesh mesh(sphere, 10, Raz::SphereMeshType::UV);

    CHECK(mesh.getSubmeshes().size() == 1);
    CHECK(mesh.recoverVertexCount() == 121);
    CHECK(mesh.recoverTriangleCount() == 180);

    const Raz::AABB& boundingBox = mesh.computeBoundingBox();

    CHECK_THAT(boundingBox.computeCentroid(), IsNearlyEqualToVector(sphere.computeCentroid()));

    // The bounding box's Z coordinate is not exactly radius/-radius, due to the approximation made by the low longitude count
    const Raz::Vec3f expectedMaxPos = Raz::Vec3f(sphere.getRadius(), sphere.getRadius(), 2.3776417f) + sphere.getCenter();
    const Raz::Vec3f expectedMinPos = Raz::Vec3f(-sphere.getRadius(), -sphere.getRadius(), -2.3776412f) + sphere.getCenter();

    CHECK_THAT(boundingBox.getRightTopFrontPos(), IsNearlyEqualToVector(expectedMaxPos));
    CHECK_THAT(boundingBox.getLeftBottomBackPos(), IsNearlyEqualToVector(expectedMinPos));

    const Raz::Vec3f expectedHalfExtents = Raz::Vec3f(Raz::Vec2f(sphere.getRadius()), 2.3776414f);
    CHECK_THAT(boundingBox.computeHalfExtents(), IsNearlyEqualToVector(expectedHalfExtents));
  }

  // UV sphere mesh with 100 splits in latitude/longitude
  {
    Raz::Mesh mesh(sphere, 100, Raz::SphereMeshType::UV);

    CHECK(mesh.getSubmeshes().size() == 1);
    CHECK(mesh.recoverVertexCount() == 10201);
    CHECK(mesh.recoverTriangleCount() == 19800);

    const Raz::AABB& boundingBox = mesh.computeBoundingBox();

    CHECK_THAT(boundingBox.computeCentroid(), IsNearlyEqualToVector(sphere.computeCentroid()));

    // With 100 splits, the bounding box's Z is now equal to radius/-radius as expected
    const Raz::Vec3f expectedMaxPos = Raz::Vec3f(sphere.getRadius()) + sphere.getCenter();
    const Raz::Vec3f expectedMinPos = Raz::Vec3f(-sphere.getRadius()) + sphere.getCenter();

    CHECK_THAT(boundingBox.getRightTopFrontPos(), IsNearlyEqualToVector(expectedMaxPos));
    CHECK_THAT(boundingBox.getLeftBottomBackPos(), IsNearlyEqualToVector(expectedMinPos));

    CHECK_THAT(boundingBox.computeHalfExtents(), IsNearlyEqualToVector(Raz::Vec3f(sphere.getRadius())));
  }
}
