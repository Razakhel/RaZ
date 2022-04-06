#include "Catch.hpp"

#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/Shape.hpp"

TEST_CASE("MeshRenderer materials") {
  Raz::MeshRenderer meshRenderer;

  CHECK(meshRenderer.getSubmeshRenderers().empty());
  CHECK(meshRenderer.getMaterials().empty());

  meshRenderer.addMaterial(Raz::MaterialBlinnPhong::create());

  CHECK(meshRenderer.getMaterials().size() == 1);
  CHECK(meshRenderer.getMaterials()[0]->getType() == Raz::MaterialType::BLINN_PHONG);
  CHECK(static_cast<Raz::MaterialBlinnPhong&>(*meshRenderer.getMaterials()[0]).getDiffuse() == Raz::Vec3f(1.f));
  CHECK(meshRenderer.getSubmeshRenderers().empty()); // Adding a material doesn't add a submesh renderer

  meshRenderer.addMaterial(Raz::MaterialCookTorrance::create());

  CHECK(meshRenderer.getMaterials().size() == 2);
  CHECK(meshRenderer.getMaterials()[0]->getType() == Raz::MaterialType::BLINN_PHONG);
  CHECK(meshRenderer.getMaterials()[1]->getType() == Raz::MaterialType::COOK_TORRANCE);
  CHECK(static_cast<Raz::MaterialCookTorrance&>(*meshRenderer.getMaterials()[1]).getBaseColor() == Raz::Vec3f(1.f));

  // Adding submesh renderers to test material removal
  meshRenderer.addSubmeshRenderer().setMaterialIndex(1);
  meshRenderer.addSubmeshRenderer().setMaterialIndex(42);
  meshRenderer.addSubmeshRenderer().setMaterialIndex(150);

  meshRenderer.removeMaterial(0);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 3); // Removing an existing material doesn't remove any submesh renderer
  // However, it does change their material indices: they are all decremented by 1 to match the removed space
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 41);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == 149);

  meshRenderer.setMaterial(Raz::MaterialBlinnPhong::create(Raz::Vec3f(0.f)));

  // Setting a material replaces all existing ones
  CHECK(meshRenderer.getMaterials().size() == 1);
  CHECK(meshRenderer.getMaterials()[0]->getType() == Raz::MaterialType::BLINN_PHONG);
  CHECK(static_cast<Raz::MaterialBlinnPhong&>(*meshRenderer.getMaterials()[0]).getDiffuse() == Raz::Vec3f(0.f));

  // It also sets a material index of 0 to all existing submesh renderers
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == 0);

  meshRenderer.removeMaterial(0);

  CHECK(meshRenderer.getMaterials().empty());

  // The submesh renderers already have a material index of 0; removing the last material doesn't change it
  CHECK(meshRenderer.getSubmeshRenderers().size() == 3);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == 0);
}

TEST_CASE("MeshRenderer clone") {
  Raz::MeshRenderer meshRenderer;

  meshRenderer.addSubmeshRenderer().setMaterialIndex(42);
  meshRenderer.addSubmeshRenderer().setMaterialIndex(150);

  meshRenderer.addMaterial(Raz::MaterialBlinnPhong::create(Raz::Vec3f(0.f)));
  meshRenderer.addMaterial(Raz::MaterialCookTorrance::create(Raz::Vec3f(0.f), 0.f, 0.f));

  Raz::MeshRenderer clonedMeshRenderer = meshRenderer.clone();

  CHECK(clonedMeshRenderer.getSubmeshRenderers().size() == 2);
  CHECK(clonedMeshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 42);
  CHECK(clonedMeshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 150);

  CHECK(clonedMeshRenderer.getMaterials().size() == 2);
  CHECK(clonedMeshRenderer.getMaterials()[0]->getType() == Raz::MaterialType::BLINN_PHONG);
  CHECK(static_cast<Raz::MaterialBlinnPhong&>(*clonedMeshRenderer.getMaterials()[0]).getDiffuse() == Raz::Vec3f(0.f));
  CHECK(clonedMeshRenderer.getMaterials()[1]->getType() == Raz::MaterialType::COOK_TORRANCE);
  CHECK(static_cast<Raz::MaterialCookTorrance&>(*clonedMeshRenderer.getMaterials()[1]).getBaseColor() == Raz::Vec3f(0.f));

}

TEST_CASE("MeshRenderer loading") {
  Raz::MeshRenderer meshRenderer(Raz::Mesh(Raz::Sphere(Raz::Vec3f(0.f), 1.f), 1, Raz::SphereMeshType::UV));

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getMaterials().size() == 1); // A default material is created after loading if none exists

  meshRenderer.getSubmeshRenderers()[0].setMaterialIndex(42);
  static_cast<Raz::MaterialBlinnPhong&>(*meshRenderer.getMaterials()[0]).setDiffuse(Raz::Vec3f(0.f));

  meshRenderer.load(Raz::Mesh(Raz::Plane(1.f), 1.f, 1.f));

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1); // No submesh renderer is added again
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 42); // The submesh renderers are left untouched

  CHECK(meshRenderer.getMaterials().size() == 1); // One material already exists; none has been added
  CHECK(static_cast<Raz::MaterialBlinnPhong&>(*meshRenderer.getMaterials()[0]).getDiffuse() == Raz::Vec3f(0.f)); // The materials are left untouched
}
