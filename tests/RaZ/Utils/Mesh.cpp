#include "catch/catch.hpp"
#include "RaZ/Render/Mesh.hpp"

TEST_CASE("Mesh imported OBJ quad faces") {
  const Raz::Mesh mesh("../../assets/meshes/ballQuads.obj");

  REQUIRE(mesh.getSubmeshes().size() == 1);
  REQUIRE(mesh.recoverVertexCount() == 439);
  REQUIRE(mesh.recoverTriangleCount() == 760);

  REQUIRE(mesh.getMaterials().empty());
}

#if defined(FBX_ENABLED)
TEST_CASE("Mesh imported FBX") {
  const Raz::Mesh mesh("../../assets/meshes/shaderBall.fbx");

  REQUIRE(mesh.getSubmeshes().size() == 8);
  REQUIRE(mesh.recoverVertexCount() == 40004);
  REQUIRE(mesh.recoverTriangleCount() == 78312);
  REQUIRE(mesh.getMaterials().size() == 4);
}
#endif
