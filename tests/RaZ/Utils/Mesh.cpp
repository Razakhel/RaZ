#include "catch/catch.hpp"
#include "RaZ/Render/Mesh.hpp"

TEST_CASE("Mesh imported OBJ quad faces") {
  const Raz::Mesh mesh("../../assets/meshes/ballQuads.obj");

  REQUIRE(mesh.getSubmeshes().size() == 1);
  REQUIRE(mesh.recoverVertexCount() == 439);
  REQUIRE(mesh.recoverTriangleCount() == 760);

  REQUIRE(mesh.getMaterials().empty());
}
