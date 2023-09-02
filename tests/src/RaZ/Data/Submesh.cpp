#include "Catch.hpp"

#include "RaZ/Data/Submesh.hpp"

TEST_CASE("Submesh basic") {
  Raz::Submesh submesh;
  CHECK(submesh.getVertexCount() == 0);
  CHECK(submesh.getLineIndexCount() == 0);
  CHECK(submesh.getTriangleIndexCount() == 0);
  CHECK(submesh.getBoundingBox() == Raz::AABB(Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

  const Raz::AABB& boundingBox = submesh.computeBoundingBox();
  CHECK(boundingBox == Raz::AABB(Raz::Vec3f(std::numeric_limits<float>::max()), Raz::Vec3f(std::numeric_limits<float>::lowest())));
  CHECK(boundingBox == submesh.getBoundingBox());

  submesh.getVertices() = {
    Raz::Vertex{ Raz::Vec3f(-1.f) },
    Raz::Vertex{ Raz::Vec3f(0.f) },
    Raz::Vertex{ Raz::Vec3f(1.f) }
  };
  submesh.getTriangleIndices() = { 0, 1, 2 };
  submesh.getLineIndices() = { 0, 1, 1, 2 };
  CHECK(submesh.getVertexCount() == 3);
  CHECK(submesh.getLineIndexCount() == 4);
  CHECK(submesh.getTriangleIndexCount() == 3);

  submesh.computeBoundingBox();
  CHECK(boundingBox == Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)));
  CHECK(boundingBox == submesh.getBoundingBox());
}
