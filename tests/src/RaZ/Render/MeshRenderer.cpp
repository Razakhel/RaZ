#include "Catch.hpp"

#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/Shape.hpp"

TEST_CASE("MeshRenderer materials") {
  Raz::MeshRenderer meshRenderer;

  CHECK(meshRenderer.getSubmeshRenderers().empty());
  CHECK(meshRenderer.getMaterials().empty());

  meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::BLINN_PHONG));

  CHECK(meshRenderer.getMaterials().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().empty()); // Adding a material doesn't add a submesh renderer

  meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::COOK_TORRANCE));

  CHECK(meshRenderer.getMaterials().size() == 2);

  // Adding submesh renderers to test material removal
  meshRenderer.addSubmeshRenderer().setMaterialIndex(0);
  meshRenderer.addSubmeshRenderer().setMaterialIndex(42);
  meshRenderer.addSubmeshRenderer().setMaterialIndex(150);

  meshRenderer.removeMaterial(0);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 3); // Removing an existing material doesn't remove any submesh renderer
  // However, it does change their material indices: if a submesh renderer referenced the removed material, its index is invalidated;
  //   otherwise, all indices are decremented by 1 to match the removed space
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == std::numeric_limits<std::size_t>::max());
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 41);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == 149);

  meshRenderer.setMaterial(Raz::Material(Raz::MaterialType::COOK_TORRANCE));

  // Setting a material replaces all existing ones
  CHECK(meshRenderer.getMaterials().size() == 1);

  // It also sets a material index of 0 to all existing submesh renderers
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == 0);

  meshRenderer.removeMaterial(0);

  CHECK(meshRenderer.getMaterials().empty());

  // The submesh renderers already have a material index of 0; removing the last material invalidates all indices
  CHECK(meshRenderer.getSubmeshRenderers().size() == 3);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == std::numeric_limits<std::size_t>::max());
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == std::numeric_limits<std::size_t>::max());
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == std::numeric_limits<std::size_t>::max());
}

TEST_CASE("MeshRenderer clone") {
  Raz::MeshRenderer meshRenderer;

  meshRenderer.addSubmeshRenderer().setMaterialIndex(42);
  meshRenderer.addSubmeshRenderer().setMaterialIndex(150);

  meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::BLINN_PHONG)).getProgram().setAttribute(Raz::Vec3f(0.5f), Raz::MaterialAttribute::BaseColor);
  meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::COOK_TORRANCE)).getProgram().setAttribute(Raz::Vec3f(0.5f), Raz::MaterialAttribute::BaseColor);

  Raz::MeshRenderer clonedMeshRenderer = meshRenderer.clone();

  CHECK(clonedMeshRenderer.getSubmeshRenderers().size() == 2);
  CHECK(clonedMeshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 42);
  CHECK(clonedMeshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 150);

  CHECK(clonedMeshRenderer.getMaterials().size() == 2);
  CHECK(clonedMeshRenderer.getMaterials()[0].getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.5f));
  CHECK(clonedMeshRenderer.getMaterials()[1].getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.5f));
}

TEST_CASE("MeshRenderer loading") {
  Raz::MeshRenderer meshRenderer(Raz::Mesh(Raz::Sphere(Raz::Vec3f(0.f), 1.f), 1, Raz::SphereMeshType::UV));

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getMaterials().size() == 1); // A default material is created after loading if none exists

  meshRenderer.getSubmeshRenderers()[0].setMaterialIndex(42);
  meshRenderer.getMaterials()[0].getProgram().setAttribute(Raz::Vec3f(0.f), Raz::MaterialAttribute::BaseColor);

  meshRenderer.load(Raz::Mesh(Raz::Plane(1.f), 1.f, 1.f));

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1); // No submesh renderer is added again
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 42); // The submesh renderers are left untouched

  CHECK(meshRenderer.getMaterials().size() == 1); // One material already exists; none has been added
  // The materials are left untouched
  CHECK(meshRenderer.getMaterials()[0].getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.f));
}
