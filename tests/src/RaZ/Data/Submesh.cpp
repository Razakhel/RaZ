#include "RaZ/Data/Submesh.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

TEST_CASE("Vertex basic", "[data]") {
  constexpr Raz::Vertex vertex1{ Raz::Vec3f(0.f), Raz::Vec2f(1.f), Raz::Axis::Z, Raz::Axis::X };
  CHECK(vertex1.position == Raz::Vec3f(0.f));
  CHECK(vertex1.texcoords == Raz::Vec2f(1.f));
  CHECK(vertex1.normal == Raz::Axis::Z);
  CHECK(vertex1.tangent == Raz::Axis::X);

  constexpr Raz::Vertex vertex2{
    vertex1.position + std::numeric_limits<float>::epsilon(),
    vertex1.texcoords + std::numeric_limits<float>::epsilon(),
    vertex1.normal + std::numeric_limits<float>::epsilon(),
    vertex1.tangent + std::numeric_limits<float>::epsilon(),
  };
  CHECK(vertex1 == vertex2); // Vertex equality checks their near-equality
  CHECK_FALSE(vertex1 != vertex2); // Likewise for vertex inequality
  CHECK_FALSE(vertex1.strictlyEquals(vertex2));
  CHECK_FALSE(std::hash<Raz::Vertex>()(vertex1) == std::hash<Raz::Vertex>()(vertex2));
}

TEST_CASE("Vertex printing", "[data]") {
  constexpr Raz::Vertex vertex{
    Raz::Vec3f(1.f, 2.f, 3.f),
    Raz::Vec2f(0.123456789f, 0.987654321f),
    Raz::Vec3f(4.f, 5.f, 6.f),
    Raz::Vec3f(7.f, 8.f, 9.f),
  };

  std::stringstream stream;

  stream << vertex;
  CHECK(stream.str() ==
    "{\n"
    "  [ 1, 2, 3 ]\n"
    "  [ 0.123457, 0.987654 ]\n"
    "  [ 4, 5, 6 ]\n"
    "  [ 7, 8, 9 ]\n"
    "}");
}

TEST_CASE("Submesh basic", "[data]") {
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
