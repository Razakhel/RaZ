#include "Catch.hpp"

#include "RaZ/Render/Mesh.hpp"

TEST_CASE("Mesh imported OBJ quad faces") {
  const Raz::Mesh mesh(RAZ_TESTS_ROOT + "../assets/meshes/ballQuads.obj"s);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 439);
  CHECK(mesh.recoverTriangleCount() == 760);

  CHECK(mesh.getMaterials().empty());
}

TEST_CASE("Mesh imported OBJ cube (Blinn-Phong)") {
  const Raz::Mesh mesh(RAZ_TESTS_ROOT + "assets/meshes/cube_BP.obj"s);

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.recoverTriangleCount() == 12);

  REQUIRE(mesh.getMaterials().size() == 1);
  REQUIRE(mesh.getMaterials().front()->getType() == Raz::MaterialType::BLINN_PHONG);

  const auto& material = static_cast<const Raz::MaterialBlinnPhong&>(*mesh.getMaterials().front());

  CHECK(material.getBaseColor() == Raz::Vec3f(1.f));
  CHECK(material.getAmbient() == Raz::Vec3f(0.67f));
  CHECK(material.getSpecular() == Raz::Vec3f(0.33f));
  CHECK(material.getEmissive() == Raz::Vec3f(0.f));

  // Each texture is flipped vertically when imported; the values are checked accordingly

  // Diffuse map
  {
    REQUIRE_FALSE(material.getDiffuseMap()->getImage().isEmpty());

    CHECK(material.getDiffuseMap()->getImage().getColorspace() == Raz::ImageColorspace::RGBA);
    CHECK(material.getDiffuseMap()->getImage().getWidth() == 2);
    CHECK(material.getDiffuseMap()->getImage().getHeight() == 2);

    REQUIRE(material.getDiffuseMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* diffuseData = static_cast<const uint8_t*>(material.getDiffuseMap()->getImage().getDataPtr());

    // RGBR image with alpha, flipped vertically: verifying that values are BRRG with 50% opacity

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
    REQUIRE_FALSE(material.getAmbientMap()->getImage().isEmpty());

    CHECK(material.getAmbientMap()->getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(material.getAmbientMap()->getImage().getWidth() == 2);
    CHECK(material.getAmbientMap()->getImage().getHeight() == 2);

    REQUIRE(material.getAmbientMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* ambientData = static_cast<const uint8_t*>(material.getAmbientMap()->getImage().getDataPtr());

    CHECK(ambientData[0] == 0);
    CHECK(ambientData[1] == 0);
    CHECK(ambientData[2] == 255);

    CHECK(ambientData[3] == 0);
    CHECK(ambientData[4] == 0);
    CHECK(ambientData[5] == 255);

    CHECK(ambientData[6] == 0);
    CHECK(ambientData[7] == 0);
    CHECK(ambientData[8] == 255);
  }

  // Specular map
  {
    REQUIRE_FALSE(material.getSpecularMap()->getImage().isEmpty());

    CHECK(material.getSpecularMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getSpecularMap()->getImage().getWidth() == 2);
    CHECK(material.getSpecularMap()->getImage().getHeight() == 2);

    REQUIRE(material.getSpecularMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* specularData = static_cast<const uint8_t*>(material.getSpecularMap()->getImage().getDataPtr());

    CHECK(specularData[0] == 0);
    CHECK(specularData[1] == 255);
    CHECK(specularData[2] == 255);
    CHECK(specularData[3] == 0);
  }

  // Emissive map
  {
    REQUIRE_FALSE(material.getEmissiveMap()->getImage().isEmpty());

    CHECK(material.getEmissiveMap()->getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(material.getEmissiveMap()->getImage().getWidth() == 2);
    CHECK(material.getEmissiveMap()->getImage().getHeight() == 2);

    REQUIRE(material.getEmissiveMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* emissiveData = static_cast<const uint8_t*>(material.getEmissiveMap()->getImage().getDataPtr());

    CHECK(emissiveData[0] == 255);
    CHECK(emissiveData[1] == 0);
    CHECK(emissiveData[2] == 0);

    CHECK(emissiveData[3] == 255);
    CHECK(emissiveData[4] == 0);
    CHECK(emissiveData[5] == 0);

    CHECK(emissiveData[6] == 255);
    CHECK(emissiveData[7] == 0);
    CHECK(emissiveData[8] == 0);
  }

  // Transparency map
  {
    REQUIRE_FALSE(material.getTransparencyMap()->getImage().isEmpty());

    CHECK(material.getTransparencyMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getTransparencyMap()->getImage().getWidth() == 2);
    CHECK(material.getTransparencyMap()->getImage().getHeight() == 2);

    REQUIRE(material.getTransparencyMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* transparencyData = static_cast<const uint8_t*>(material.getTransparencyMap()->getImage().getDataPtr());

    CHECK(transparencyData[0] == 255);
    CHECK(transparencyData[1] == 255);
    CHECK(transparencyData[2] == 255);
    CHECK(transparencyData[3] == 255);
  }

  // Bump map
  {
    REQUIRE_FALSE(material.getBumpMap()->getImage().isEmpty());

    CHECK(material.getBumpMap()->getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(material.getBumpMap()->getImage().getWidth() == 2);
    CHECK(material.getBumpMap()->getImage().getHeight() == 2);

    REQUIRE(material.getBumpMap()->getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* bumpData = static_cast<const uint8_t*>(material.getBumpMap()->getImage().getDataPtr());

    CHECK(bumpData[0] == 0);
    CHECK(bumpData[1] == 0);
    CHECK(bumpData[2] == 0);
    CHECK(bumpData[3] == 0);
  }
}

#if defined(FBX_ENABLED)
TEST_CASE("Mesh imported FBX") {
  const Raz::Mesh mesh(RAZ_TESTS_ROOT + "../assets/meshes/shaderBall.fbx"s);

  CHECK(mesh.getSubmeshes().size() == 8);
  CHECK(mesh.recoverVertexCount() == 40004);
  CHECK(mesh.recoverTriangleCount() == 78312);
  CHECK(mesh.getMaterials().size() == 4);
}
#endif
