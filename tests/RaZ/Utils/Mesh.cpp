#include "Catch.hpp"

#include "RaZ/Render/Mesh.hpp"

TEST_CASE("Mesh imported OBJ quad faces") {
  const Raz::Mesh mesh("../../assets/meshes/ballQuads.obj");

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 439);
  CHECK(mesh.recoverTriangleCount() == 760);

  CHECK(mesh.getMaterials().empty());
}

#if defined(FBX_ENABLED)
TEST_CASE("Mesh imported FBX") {
  const Raz::Mesh mesh("../../assets/meshes/shaderBall.fbx");

  CHECK(mesh.getSubmeshes().size() == 8);
  CHECK(mesh.recoverVertexCount() == 40004);
  CHECK(mesh.recoverTriangleCount() == 78312);
  CHECK(mesh.getMaterials().size() == 4);
}
#endif
