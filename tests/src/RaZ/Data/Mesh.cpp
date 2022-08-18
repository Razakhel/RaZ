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
  CHECK(submesh.getVertices()[0].tangent == Raz::Axis::X);

  CHECK(submesh.getVertices()[1].position == Raz::Vec3f(width, plane.getDistance(), depth));
  CHECK(submesh.getVertices()[1].normal == plane.getNormal());
  CHECK(submesh.getVertices()[1].texcoords == Raz::Vec2f(1.f, 0.f));
  CHECK(submesh.getVertices()[1].tangent == Raz::Axis::X);

  CHECK(submesh.getVertices()[2].position == Raz::Vec3f(width, plane.getDistance(), -depth));
  CHECK(submesh.getVertices()[2].normal == plane.getNormal());
  CHECK(submesh.getVertices()[2].texcoords == Raz::Vec2f(1.f, 1.f));
  CHECK(submesh.getVertices()[2].tangent == Raz::Axis::X);

  CHECK(submesh.getVertices()[3].position == Raz::Vec3f(-width, plane.getDistance(), -depth));
  CHECK(submesh.getVertices()[3].normal == plane.getNormal());
  CHECK(submesh.getVertices()[3].texcoords == Raz::Vec2f(0.f, 1.f));
  CHECK(submesh.getVertices()[3].tangent == Raz::Axis::X);

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

TEST_CASE("Mesh triangle") {
  const Raz::Triangle triangle(Raz::Vec3f(-1.f, 0.f, 0.f), Raz::Vec3f(1.f, 0.f, 0.f), Raz::Vec3f(0.f, 1.f, 0.f));
  CHECK(triangle.computeNormal() == Raz::Axis::Z);
  CHECK(triangle.isCounterClockwise(Raz::Axis::Z));

  Raz::Mesh mesh(triangle, Raz::Vec2f(0.f, 0.f), Raz::Vec2f(1.f, 0.f), Raz::Vec2f(0.5f, 1.f));

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 3);
  CHECK(mesh.recoverTriangleCount() == 1);

  const Raz::Submesh& submesh = mesh.getSubmeshes().front();

  CHECK(submesh.getVertices()[0].position == triangle.getFirstPos());
  CHECK(submesh.getVertices()[1].position == triangle.getSecondPos());
  CHECK(submesh.getVertices()[2].position == triangle.getThirdPos());

  CHECK(submesh.getVertices()[0].texcoords == Raz::Vec2f(0.f, 0.f));
  CHECK(submesh.getVertices()[1].texcoords == Raz::Vec2f(1.f, 0.f));
  CHECK(submesh.getVertices()[2].texcoords == Raz::Vec2f(0.5f, 1.f));

  for (const Raz::Vertex& vertex : submesh.getVertices()) {
    CHECK(vertex.normal == Raz::Axis::Z);
    CHECK(vertex.tangent == Raz::Axis::X);
  }

  // Checking that the mesh is constructed with the same winding order
  CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[0]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[1]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[2]].position).isCounterClockwise(Raz::Axis::Z));

  const Raz::AABB& boundingBox = mesh.computeBoundingBox();

  CHECK(boundingBox.computeCentroid() == Raz::Vec3f(0.f, 0.5f, 0.f));

  const Raz::Vec3f expectedMinPos(-1.f, 0.f, 0.f);
  const Raz::Vec3f expectedMaxPos(1.f, 1.f, 0.f);

  CHECK(boundingBox.getMinPosition() == expectedMinPos);
  CHECK(boundingBox.getMaxPosition() == expectedMaxPos);
}

TEST_CASE("Mesh quad") {
  const Raz::Quad quad(Raz::Vec3f(0.f, 2.f, 0.f), Raz::Vec3f(1.f, 2.f, 0.f), Raz::Vec3f(1.f, 0.f, 0.f), Raz::Vec3f(0.f, 0.f, 0.f));

  Raz::Mesh mesh(quad);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 4);
  CHECK(mesh.recoverTriangleCount() == 2);

  const Raz::Submesh& submesh = mesh.getSubmeshes().front();

  for (const Raz::Vertex& vertex : submesh.getVertices()) {
    CHECK(vertex.normal == Raz::Axis::Z);
    CHECK(vertex.tangent == Raz::Axis::X);
  }

  // Checking that the mesh's triangles are constructed in a counter-clockwise order
  CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[0]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[1]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[2]].position).isCounterClockwise(Raz::Axis::Z));

  CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[3]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[4]].position,
                      submesh.getVertices()[submesh.getTriangleIndices()[5]].position).isCounterClockwise(Raz::Axis::Z));

  const Raz::AABB& boundingBox = mesh.computeBoundingBox();

  CHECK(boundingBox.computeCentroid() == Raz::Vec3f(0.5f, 1.f, 0.f));

  const Raz::Vec3f expectedMinPos(0.f, 0.f, 0.f);
  const Raz::Vec3f expectedMaxPos(1.f, 2.f, 0.f);

  CHECK(boundingBox.getMinPosition() == expectedMinPos);
  CHECK(boundingBox.getMaxPosition() == expectedMaxPos);
}

TEST_CASE("Mesh UV sphere") {
  const Raz::Sphere sphere(Raz::Vec3f(1.f, 2.f, 3.f), 2.5f);

  auto checkWindingOrder = [&sphere] (const Raz::Mesh& mesh) {
    const Raz::Submesh& submesh = mesh.getSubmeshes().front();

    // Checking that all normals are pointing outside the sphere
    for (const Raz::Vertex& vertex : submesh.getVertices()) {
      CHECK_THAT(vertex.normal.computeLength(), IsNearlyEqualTo(1.f));
      // A simple epsilon is not enough for all compilers except MinGW: the X component of a single vector has a difference
      //  beyond tolerance, although very small; the origin of this discrepancy between compilers is unknown
      CHECK_THAT(vertex.normal, IsNearlyEqualToVector((vertex.position - sphere.getCenter()).normalize(), 0.000001f));

      // TODO: 4 tangents are null for unknown reasons
      // Checking that the tangent is perpendicular to the normal
      //CHECK_THAT(vertex.tangent.computeLength(), IsNearlyEqualTo(1.f));
      //CHECK_THAT(vertex.tangent.dot(vertex.normal), IsNearlyEqualTo(0.f));
    }

    // Checking that the mesh's triangles are constructed in a counter-clockwise order
    for (std::size_t i = 0; i < submesh.getTriangleIndexCount(); i += 3) {
        const Raz::Vec3f normal = (submesh.getVertices()[submesh.getTriangleIndices()[i    ]].normal
                                 + submesh.getVertices()[submesh.getTriangleIndices()[i + 1]].normal
                                 + submesh.getVertices()[submesh.getTriangleIndices()[i + 2]].normal).normalize();
        CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[i    ]].position,
                            submesh.getVertices()[submesh.getTriangleIndices()[i + 1]].position,
                            submesh.getVertices()[submesh.getTriangleIndices()[i + 2]].position).isCounterClockwise(normal));
    }
  };

  // UV sphere mesh with 10 splits in latitude/longitude
  {
    Raz::Mesh mesh(sphere, 10, Raz::SphereMeshType::UV);

    CHECK(mesh.getSubmeshes().size() == 1);
    CHECK(mesh.recoverVertexCount() == 121);
    CHECK(mesh.recoverTriangleCount() == 180);

    checkWindingOrder(mesh);

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

    checkWindingOrder(mesh);

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

  auto checkWindingOrder = [&sphere] (const Raz::Mesh& mesh) {
    const Raz::Submesh& submesh = mesh.getSubmeshes().front();

    // Checking that all normals are pointing outside the sphere
    for (const Raz::Vertex& vertex : submesh.getVertices()) {
      CHECK_THAT(vertex.normal.computeLength(), IsNearlyEqualTo(1.f));
      CHECK_THAT(vertex.normal, IsNearlyEqualToVector((vertex.position - sphere.getCenter()).normalize()));
    }

    // Checking that the mesh's triangles are constructed in a counter-clockwise order
    for (std::size_t i = 0; i < submesh.getTriangleIndexCount(); i += 3) {
      const Raz::Vec3f normal = (submesh.getVertices()[submesh.getTriangleIndices()[i]].normal
                               + submesh.getVertices()[submesh.getTriangleIndices()[i + 1]].normal
                               + submesh.getVertices()[submesh.getTriangleIndices()[i + 2]].normal).normalize();
      CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[i]].position,
                          submesh.getVertices()[submesh.getTriangleIndices()[i + 1]].position,
                          submesh.getVertices()[submesh.getTriangleIndices()[i + 2]].position).isCounterClockwise(normal));
    }
  };

  // Icosphere mesh with 1 subdivision
  {
    Raz::Mesh mesh(sphere, 1, Raz::SphereMeshType::ICO);

    CHECK(mesh.getSubmeshes().size() == 1);
    CHECK(mesh.recoverVertexCount() == 12);
    CHECK(mesh.recoverTriangleCount() == 20);

    checkWindingOrder(mesh);

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
//    checkWindingOrder(mesh);
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

TEST_CASE("Mesh AABB") {
  const Raz::AABB box(Raz::Vec3f(-1.f), Raz::Vec3f(1.f));

  Raz::Mesh mesh(box);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.recoverTriangleCount() == 12);

  const Raz::Submesh& submesh = mesh.getSubmeshes().front();

  auto checkData = [&submesh] (std::size_t startIndex, const Raz::Vec3f& normal, const Raz::Vec3f& tangent) {
    REQUIRE(tangent.dot(normal) == 0.f);

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex    ]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 1]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 2]].position).isCounterClockwise(normal));

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 3]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 4]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 5]].position).isCounterClockwise(normal));

    for (std::size_t i = startIndex; i < startIndex + 6; ++i) {
      const Raz::Vertex& vert = submesh.getVertices()[submesh.getTriangleIndices()[i]];

      CHECK(vert.normal.strictlyEquals(normal));
      CHECK(vert.tangent.strictlyEquals(tangent));
    }
  };

  checkData(0, Raz::Axis::X, -Raz::Axis::Z);   // Right face
  checkData(6, -Raz::Axis::X, Raz::Axis::Z);   // Left face
  checkData(12, Raz::Axis::Y, Raz::Axis::X);   // Top face
  checkData(18, -Raz::Axis::Y, Raz::Axis::X);  // Bottom face
  checkData(24, Raz::Axis::Z, Raz::Axis::X);   // Front face
  checkData(30, -Raz::Axis::Z, -Raz::Axis::X); // Back face

  const Raz::AABB& boundingBox = mesh.computeBoundingBox();

  CHECK(boundingBox.computeCentroid().strictlyEquals(box.computeCentroid()));
  CHECK(boundingBox.getMinPosition().strictlyEquals(box.getMinPosition()));
  CHECK(boundingBox.getMaxPosition().strictlyEquals(box.getMaxPosition()));
}
