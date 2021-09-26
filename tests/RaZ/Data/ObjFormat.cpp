#include "Catch.hpp"

#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"

TEST_CASE("ObjFormat load quad faces") {
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT + "../assets/meshes/ballQuads.obj"s);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 439);
  CHECK(mesh.recoverTriangleCount() == 760);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  CHECK(meshRenderer.getMaterials().empty());
}

TEST_CASE("ObjFormat load Blinn-Phong") {
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT + "assets/meshes/çûbè_BP.obj"s);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.recoverTriangleCount() == 12);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  REQUIRE(meshRenderer.getMaterials().size() == 1);
  REQUIRE(meshRenderer.getMaterials().front()->getType() == Raz::MaterialType::BLINN_PHONG);

  const auto& material = static_cast<const Raz::MaterialBlinnPhong&>(*meshRenderer.getMaterials().front());

  CHECK(material.getBaseColor() == Raz::Vec3f(1.f));
  CHECK(material.getAmbient() == Raz::Vec3f(0.67f));
  CHECK(material.getSpecular() == Raz::Vec3f(0.33f));
  CHECK(material.getEmissive() == Raz::Vec3f(0.f));

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Diffuse map
  {
    CHECK(material.getDiffuseMap()->getBindingIndex() == 0);

    REQUIRE_FALSE(material.getDiffuseMap()->getImage().isEmpty());

    CHECK(material.getDiffuseMap()->getImage().getColorspace() == Raz::ImageColorspace::RGBA);
    CHECK(material.getDiffuseMap()->getImage().getWidth() == 2);
    CHECK(material.getDiffuseMap()->getImage().getHeight() == 2);

    REQUIRE(material.getDiffuseMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* diffuseData = static_cast<const uint8_t*>(material.getDiffuseMap()->getImage().getDataPtr());

    // RGBR image with alpha, flipped vertically: verifying that values are BRRG with 50% opacity

    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------

    CHECK(diffuseData[0] == 0);
    CHECK(diffuseData[1] == 0);
    CHECK(diffuseData[2] == 255);
    CHECK(diffuseData[3] == 127);

    CHECK(diffuseData[4] == 255);
    CHECK(diffuseData[5] == 0);
    CHECK(diffuseData[6] == 0);
    CHECK(diffuseData[7] == 127);

    CHECK(diffuseData[8]  == 255);
    CHECK(diffuseData[9]  == 0);
    CHECK(diffuseData[10] == 0);
    CHECK(diffuseData[11] == 127);

    CHECK(diffuseData[12] == 0);
    CHECK(diffuseData[13] == 255);
    CHECK(diffuseData[14] == 0);
    CHECK(diffuseData[15] == 127);
  }

  // Ambient map
  {
    CHECK(material.getAmbientMap()->getBindingIndex() == 1);

    REQUIRE_FALSE(material.getAmbientMap()->getImage().isEmpty());

    CHECK(material.getAmbientMap()->getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(material.getAmbientMap()->getImage().getWidth() == 2);
    CHECK(material.getAmbientMap()->getImage().getHeight() == 2);

    REQUIRE(material.getAmbientMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* ambientData = static_cast<const uint8_t*>(material.getAmbientMap()->getImage().getDataPtr());

    // ---------
    // | B | B |
    // |-------|
    // | B | B |
    // ---------

    CHECK(ambientData[0] == 0);
    CHECK(ambientData[1] == 0);
    CHECK(ambientData[2] == 255);

    CHECK(ambientData[3] == 0);
    CHECK(ambientData[4] == 0);
    CHECK(ambientData[5] == 255);

    CHECK(ambientData[6] == 0);
    CHECK(ambientData[7] == 0);
    CHECK(ambientData[8] == 255);

    CHECK(ambientData[9]  == 0);
    CHECK(ambientData[10] == 0);
    CHECK(ambientData[11] == 255);
  }

  // Specular map
  {
    CHECK(material.getSpecularMap()->getBindingIndex() == 2);

    REQUIRE_FALSE(material.getSpecularMap()->getImage().isEmpty());

    CHECK(material.getSpecularMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getSpecularMap()->getImage().getWidth() == 2);
    CHECK(material.getSpecularMap()->getImage().getHeight() == 2);

    REQUIRE(material.getSpecularMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* specularData = static_cast<const uint8_t*>(material.getSpecularMap()->getImage().getDataPtr());

    // ---------
    // | X |   |
    // |-------|
    // |   | X |
    // ---------

    CHECK(specularData[0] == 0);
    CHECK(specularData[1] == 255);
    CHECK(specularData[2] == 255);
    CHECK(specularData[3] == 0);
  }

  // Emissive map
  {
    CHECK(material.getEmissiveMap()->getBindingIndex() == 3);

    REQUIRE_FALSE(material.getEmissiveMap()->getImage().isEmpty());

    CHECK(material.getEmissiveMap()->getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(material.getEmissiveMap()->getImage().getWidth() == 2);
    CHECK(material.getEmissiveMap()->getImage().getHeight() == 2);

    REQUIRE(material.getEmissiveMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* emissiveData = static_cast<const uint8_t*>(material.getEmissiveMap()->getImage().getDataPtr());

    // ---------
    // | R | R |
    // |-------|
    // | R | R |
    // ---------

    CHECK(emissiveData[0] == 255);
    CHECK(emissiveData[1] == 0);
    CHECK(emissiveData[2] == 0);

    CHECK(emissiveData[3] == 255);
    CHECK(emissiveData[4] == 0);
    CHECK(emissiveData[5] == 0);

    CHECK(emissiveData[6] == 255);
    CHECK(emissiveData[7] == 0);
    CHECK(emissiveData[8] == 0);

    CHECK(emissiveData[9]  == 255);
    CHECK(emissiveData[10] == 0);
    CHECK(emissiveData[11] == 0);
  }

  // Transparency map
  {
    CHECK(material.getTransparencyMap()->getBindingIndex() == 4);

    REQUIRE_FALSE(material.getTransparencyMap()->getImage().isEmpty());

    CHECK(material.getTransparencyMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getTransparencyMap()->getImage().getWidth() == 2);
    CHECK(material.getTransparencyMap()->getImage().getHeight() == 2);

    REQUIRE(material.getTransparencyMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* transparencyData = static_cast<const uint8_t*>(material.getTransparencyMap()->getImage().getDataPtr());

    // ---------
    // | X | X |
    // |-------|
    // | X | X |
    // ---------

    CHECK(transparencyData[0] == 255);
    CHECK(transparencyData[1] == 255);
    CHECK(transparencyData[2] == 255);
    CHECK(transparencyData[3] == 255);
  }

  // Bump map
  {
    CHECK(material.getBumpMap()->getBindingIndex() == 5);

    REQUIRE_FALSE(material.getBumpMap()->getImage().isEmpty());

    CHECK(material.getBumpMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getBumpMap()->getImage().getWidth() == 2);
    CHECK(material.getBumpMap()->getImage().getHeight() == 2);

    REQUIRE(material.getBumpMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* bumpData = static_cast<const uint8_t*>(material.getBumpMap()->getImage().getDataPtr());

    // ---------
    // |   |   |
    // |-------|
    // |   |   |
    // ---------

    CHECK(bumpData[0] == 0);
    CHECK(bumpData[1] == 0);
    CHECK(bumpData[2] == 0);
    CHECK(bumpData[3] == 0);
  }
}

TEST_CASE("ObjFormat load Cook-Torrance") {
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT + "assets/meshes/çûbè_CT.obj"s);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.recoverTriangleCount() == 12);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  REQUIRE(meshRenderer.getMaterials().size() == 1);
  REQUIRE(meshRenderer.getMaterials().front()->getType() == Raz::MaterialType::COOK_TORRANCE);

  const auto& material = static_cast<const Raz::MaterialCookTorrance&>(*meshRenderer.getMaterials().front());

  CHECK(material.getBaseColor() == Raz::Vec3f(1.f));
  CHECK(material.getMetallicFactor() == 1.f);
  CHECK(material.getRoughnessFactor() == 1.f);

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Albedo map
  {
    CHECK(material.getAlbedoMap()->getBindingIndex() == 0);

    REQUIRE_FALSE(material.getAlbedoMap()->getImage().isEmpty());

    CHECK(material.getAlbedoMap()->getImage().getColorspace() == Raz::ImageColorspace::RGBA);
    CHECK(material.getAlbedoMap()->getImage().getWidth() == 2);
    CHECK(material.getAlbedoMap()->getImage().getHeight() == 2);

    REQUIRE(material.getAlbedoMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* albedoData = static_cast<const uint8_t*>(material.getAlbedoMap()->getImage().getDataPtr());

    // RGBR image with alpha, flipped vertically: verifying that values are BRRG with 50% opacity

    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------

    CHECK(albedoData[0] == 0);
    CHECK(albedoData[1] == 0);
    CHECK(albedoData[2] == 255);
    CHECK(albedoData[3] == 127);

    CHECK(albedoData[4] == 255);
    CHECK(albedoData[5] == 0);
    CHECK(albedoData[6] == 0);
    CHECK(albedoData[7] == 127);

    CHECK(albedoData[8]  == 255);
    CHECK(albedoData[9]  == 0);
    CHECK(albedoData[10] == 0);
    CHECK(albedoData[11] == 127);

    CHECK(albedoData[12] == 0);
    CHECK(albedoData[13] == 255);
    CHECK(albedoData[14] == 0);
    CHECK(albedoData[15] == 127);
  }

  // Normal map
  {
    CHECK(material.getNormalMap()->getBindingIndex() == 1);

    REQUIRE_FALSE(material.getNormalMap()->getImage().isEmpty());

    CHECK(material.getNormalMap()->getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(material.getNormalMap()->getImage().getWidth() == 2);
    CHECK(material.getNormalMap()->getImage().getHeight() == 2);

    REQUIRE(material.getNormalMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* normalData = static_cast<const uint8_t*>(material.getNormalMap()->getImage().getDataPtr());

    // ---------
    // | B | B |
    // |-------|
    // | B | B |
    // ---------

    CHECK(normalData[0] == 0);
    CHECK(normalData[1] == 0);
    CHECK(normalData[2] == 255);

    CHECK(normalData[3] == 0);
    CHECK(normalData[4] == 0);
    CHECK(normalData[5] == 255);

    CHECK(normalData[6] == 0);
    CHECK(normalData[7] == 0);
    CHECK(normalData[8] == 255);

    CHECK(normalData[9]  == 0);
    CHECK(normalData[10] == 0);
    CHECK(normalData[11] == 255);
  }

  // Metallic map
  {
    CHECK(material.getMetallicMap()->getBindingIndex() == 2);

    REQUIRE_FALSE(material.getMetallicMap()->getImage().isEmpty());

    CHECK(material.getMetallicMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getMetallicMap()->getImage().getWidth() == 2);
    CHECK(material.getMetallicMap()->getImage().getHeight() == 2);

    REQUIRE(material.getMetallicMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* metallicData = static_cast<const uint8_t*>(material.getMetallicMap()->getImage().getDataPtr());

    // ---------
    // | X | X |
    // |-------|
    // | X | X |
    // ---------

    CHECK(metallicData[0] == 255);
    CHECK(metallicData[1] == 255);
    CHECK(metallicData[2] == 255);
    CHECK(metallicData[3] == 255);
  }

  // Roughness map
  {
    CHECK(material.getRoughnessMap()->getBindingIndex() == 3);

    REQUIRE_FALSE(material.getRoughnessMap()->getImage().isEmpty());

    CHECK(material.getRoughnessMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getRoughnessMap()->getImage().getWidth() == 2);
    CHECK(material.getRoughnessMap()->getImage().getHeight() == 2);

    REQUIRE(material.getRoughnessMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* roughnessData = static_cast<const uint8_t*>(material.getRoughnessMap()->getImage().getDataPtr());

    // ---------
    // |   |   |
    // |-------|
    // |   |   |
    // ---------

    CHECK(roughnessData[0] == 0);
    CHECK(roughnessData[1] == 0);
    CHECK(roughnessData[2] == 0);
    CHECK(roughnessData[3] == 0);
  }

  // Ambient occlusion map
  {
    CHECK(material.getAmbientOcclusionMap()->getBindingIndex() == 4);

    REQUIRE_FALSE(material.getAmbientOcclusionMap()->getImage().isEmpty());

    CHECK(material.getAmbientOcclusionMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getAmbientOcclusionMap()->getImage().getWidth() == 2);
    CHECK(material.getAmbientOcclusionMap()->getImage().getHeight() == 2);

    REQUIRE(material.getAmbientOcclusionMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* ambientOccData = static_cast<const uint8_t*>(material.getAmbientOcclusionMap()->getImage().getDataPtr());

    // ---------
    // | X |   |
    // |-------|
    // |   | X |
    // ---------

    CHECK(ambientOccData[0] == 0);
    CHECK(ambientOccData[1] == 255);
    CHECK(ambientOccData[2] == 255);
    CHECK(ambientOccData[3] == 0);
  }
}
