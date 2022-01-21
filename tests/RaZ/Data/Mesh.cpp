#include "Catch.hpp"

#include "RaZ/Data/Mesh.hpp"

TEST_CASE("Mesh plane") {
  const Raz::Plane plane(1.5f, Raz::Axis::Y);

  constexpr float width = 1.f;
  constexpr float depth = 1.f;
  Raz::Mesh mesh(plane, width, depth);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 4);
  CHECK(mesh.recoverTriangleCount() == 2);

  const Raz::Submesh& submesh = mesh.getSubmeshes().front();

  CHECK(submesh.getVertices()[0].position == Raz::Vec3f(-width, plane.getDistance(), depth));
  CHECK(submesh.getVertices()[0].normal == plane.getNormal());
  CHECK(submesh.getVertices()[0].texcoords == Raz::Vec2f(0.f));

  CHECK(submesh.getVertices()[1].position == Raz::Vec3f(width, plane.getDistance(), depth));
  CHECK(submesh.getVertices()[1].normal == plane.getNormal());
  CHECK(submesh.getVertices()[1].texcoords == Raz::Vec2f(1.f, 0.f));

  CHECK(submesh.getVertices()[2].position == Raz::Vec3f(width, plane.getDistance(), -depth));
  CHECK(submesh.getVertices()[2].normal == plane.getNormal());
  CHECK(submesh.getVertices()[2].texcoords == Raz::Vec2f(1.f, 1.f));

  CHECK(submesh.getVertices()[3].position == Raz::Vec3f(-width, plane.getDistance(), -depth));
  CHECK(submesh.getVertices()[3].normal == plane.getNormal());
  CHECK(submesh.getVertices()[3].texcoords == Raz::Vec2f(0.f, 1.f));

  // Checking that the mesh's triangles are constructed in a counter-clockwise order according to the plane's normal
  CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[0]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[1]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[2]].position).isCounterClockwise(plane.getNormal()));

  CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[3]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[4]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[5]].position).isCounterClockwise(plane.getNormal()));

  const Raz::AABB& boundingBox = mesh.computeBoundingBox();

  CHECK(boundingBox.computeCentroid() == Raz::Vec3f(0.f, 1.5f, 0.f));

  constexpr Raz::Vec3f expectedMinPos(-1.f, 1.5f, -1.f);
  constexpr Raz::Vec3f expectedMaxPos(1.f, 1.5f, 1.f);

  CHECK(boundingBox.getMinPosition() == expectedMinPos);
  CHECK(boundingBox.getMaxPosition() == expectedMaxPos);
}

TEST_CASE("Mesh UV sphere") {
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
    const Raz::Vec3f expectedMinPos = Raz::Vec3f(-sphere.getRadius(), -sphere.getRadius(), -2.3776412f) + sphere.getCenter();
    const Raz::Vec3f expectedMaxPos = Raz::Vec3f(sphere.getRadius(), sphere.getRadius(), 2.3776417f) + sphere.getCenter();

    CHECK_THAT(boundingBox.getMinPosition(), IsNearlyEqualToVector(expectedMinPos));
    CHECK_THAT(boundingBox.getMaxPosition(), IsNearlyEqualToVector(expectedMaxPos));

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
    const Raz::Vec3f expectedMinPos = Raz::Vec3f(-sphere.getRadius()) + sphere.getCenter();
    const Raz::Vec3f expectedMaxPos = Raz::Vec3f(sphere.getRadius()) + sphere.getCenter();

    CHECK_THAT(boundingBox.getMinPosition(), IsNearlyEqualToVector(expectedMinPos));
    CHECK_THAT(boundingBox.getMaxPosition(), IsNearlyEqualToVector(expectedMaxPos));

    CHECK_THAT(boundingBox.computeHalfExtents(), IsNearlyEqualToVector(Raz::Vec3f(sphere.getRadius())));
  }
}

TEST_CASE("Mesh icosphere") {
  const Raz::Sphere sphere(Raz::Vec3f(1.f, 2.f, 3.f), 2.5f);

  // Icosphere mesh with 1 subdivision
  {
    Raz::Mesh mesh(sphere, 1, Raz::SphereMeshType::ICO);

    CHECK(mesh.getSubmeshes().size() == 1);
    CHECK(mesh.recoverVertexCount() == 12);
    CHECK(mesh.recoverTriangleCount() == 20);

    const Raz::AABB& boundingBox = mesh.computeBoundingBox();

    CHECK_THAT(boundingBox.computeCentroid(), IsNearlyEqualToVector(sphere.computeCentroid()));

    // The bounding box's components are not exactly radius/-radius, due to the approximation made by the low subdivision count
    const Raz::Vec3f expectedMinPos = Raz::Vec3f(-2.1266272f) + sphere.getCenter();
    const Raz::Vec3f expectedMaxPos = Raz::Vec3f(2.1266272f) + sphere.getCenter();

    CHECK_THAT(boundingBox.getMinPosition(), IsNearlyEqualToVector(expectedMinPos));
    CHECK_THAT(boundingBox.getMaxPosition(), IsNearlyEqualToVector(expectedMaxPos));

    CHECK_THAT(boundingBox.computeHalfExtents(), IsNearlyEqualToVector(Raz::Vec3f(2.1266272f)));
  }

  // TODO: uncomment the following test when subdivisions are effective

  // Icosphere mesh with 2 subdivisions
//  {
//    Raz::Mesh mesh(sphere, 2, Raz::SphereMeshType::ICO);
//
//    CHECK(mesh.getSubmeshes().size() == 1);
//    CHECK(mesh.recoverVertexCount() == 24);
//    CHECK(mesh.recoverTriangleCount() == 180);
//
//    const Raz::AABB& boundingBox = mesh.computeBoundingBox();
//
//    CHECK_THAT(boundingBox.computeCentroid(), IsNearlyEqualToVector(sphere.computeCentroid()));
//
//    // With 2 subdivisions, the bounding box's components are now equal to radius/-radius as expected
//    const Raz::Vec3f expectedMinPos = Raz::Vec3f(-sphere.getRadius()) + sphere.getCenter();
//    const Raz::Vec3f expectedMaxPos = Raz::Vec3f(sphere.getRadius()) + sphere.getCenter();
//
//    CHECK_THAT(boundingBox.getMinPosition(), IsNearlyEqualToVector(expectedMinPos));
//    CHECK_THAT(boundingBox.getMaxPosition(), IsNearlyEqualToVector(expectedMaxPos));
//
//    CHECK_THAT(boundingBox.computeHalfExtents(), IsNearlyEqualToVector(Raz::Vec3f(sphere.getRadius())));
//  }
}
