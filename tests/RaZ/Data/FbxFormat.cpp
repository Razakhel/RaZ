#include "Catch.hpp"

#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Utils/FilePath.hpp"

#if defined(FBX_ENABLED)
TEST_CASE("FbxFormat load basic") {
  const Raz::Mesh mesh(RAZ_TESTS_ROOT + "../assets/meshes/shaderBall.fbx"s);

  CHECK(mesh.getSubmeshes().size() == 8);
  CHECK(mesh.recoverVertexCount() == 40004);
  CHECK(mesh.recoverTriangleCount() == 78312);
  //CHECK(mesh.getMaterials().size() == 4);
}
#endif
