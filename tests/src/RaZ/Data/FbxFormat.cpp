#include "Catch.hpp"

#include "RaZ/Data/FbxFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("FbxFormat load basic", "[data]") {
  const auto [mesh, meshRenderer] = Raz::FbxFormat::load(RAZ_TESTS_ROOT "../assets/meshes/shaderBall.fbx");

  CHECK(mesh.getSubmeshes().size() == 7);
  CHECK(mesh.recoverVertexCount() == 40004);
  CHECK(mesh.recoverTriangleCount() == 78312);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 7);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 1);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == 2);
  CHECK(meshRenderer.getSubmeshRenderers()[3].getMaterialIndex() == 3);
  CHECK(meshRenderer.getSubmeshRenderers()[4].getMaterialIndex() == 3);
  CHECK(meshRenderer.getSubmeshRenderers()[5].getMaterialIndex() == 3);
  CHECK(meshRenderer.getSubmeshRenderers()[6].getMaterialIndex() == 3);

  CHECK(meshRenderer.getMaterials().size() == 4);
  for (const Raz::Material& material : meshRenderer.getMaterials())
    CHECK_FALSE(material.isEmpty());
}
