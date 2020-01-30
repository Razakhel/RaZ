#include "Catch.hpp"

#include "RaZ/Render/Mesh.hpp"

TEST_CASE("Mesh from Sphere") {
  const Raz::Sphere sphere(Raz::Vec3f(0.f), 1.f);

  // Sphere mesh with 10 splits in latitude/longitude
  {
    Raz::Mesh mesh(sphere, 10, 10);

    CHECK(mesh.getSubmeshes().size() == 1);
    CHECK(mesh.recoverVertexCount() == 121);
    CHECK(mesh.recoverTriangleCount() == 180);

    const Raz::AABB& boundingBox = mesh.computeBoundingBox();

    // The bounding box's Z coordinate is not exactly 1/-1, due to the approximation made by the low longitude count
    CHECK(boundingBox.getRightTopFrontPos() == Raz::Vec3f(1.f, 1.f, 0.9510565f));
    CHECK(boundingBox.getLeftBottomBackPos() == Raz::Vec3f(-1.f, -1.f, -0.9510565f));
  }

  // Sphere mesh with 100 splits in latitude/longitude
  {
    Raz::Mesh mesh(sphere, 100, 100);

    CHECK(mesh.getSubmeshes().size() == 1);
    CHECK(mesh.recoverVertexCount() == 10201);
    CHECK(mesh.recoverTriangleCount() == 19800);

    const Raz::AABB& boundingBox = mesh.computeBoundingBox();

    // With 100 splits, the bounding box's Z is now at 1/-1 as expected
    CHECK(boundingBox.getRightTopFrontPos() == Raz::Vec3f(1.f));
    CHECK(boundingBox.getLeftBottomBackPos() == Raz::Vec3f(-1.f));
  }
}
