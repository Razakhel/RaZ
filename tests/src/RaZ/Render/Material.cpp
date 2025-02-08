#include "RaZ/Render/Material.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Material predefined types", "[render]") {
  Raz::Material material(Raz::MaterialType::COOK_TORRANCE);

  CHECK(material.getProgram().getAttributeCount() == 5);
  REQUIRE(material.getProgram().hasAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor));
  CHECK(material.getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(1.f));
  REQUIRE(material.getProgram().hasAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive));
  CHECK(material.getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.f));
  REQUIRE(material.getProgram().hasAttribute<float>(Raz::MaterialAttribute::Metallic));
  CHECK(material.getProgram().getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.f);
  REQUIRE(material.getProgram().hasAttribute<float>(Raz::MaterialAttribute::Roughness));
  CHECK(material.getProgram().getAttribute<float>(Raz::MaterialAttribute::Roughness) == 1.f);
  REQUIRE(material.getProgram().hasAttribute<Raz::Vec4f>(Raz::MaterialAttribute::Sheen));
  CHECK(material.getProgram().getAttribute<Raz::Vec4f>(Raz::MaterialAttribute::Sheen) == Raz::Vec4f(0.f));

  CHECK(material.getProgram().getTextureCount() == 7);
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::BaseColor));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Emissive));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Normal));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Metallic));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Roughness));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Ambient));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Sheen));

  material.getProgram().clearAttributes();
  material.getProgram().clearTextures();
  material.loadType(Raz::MaterialType::BLINN_PHONG);

  CHECK(material.getProgram().getAttributeCount() == 5);
  REQUIRE(material.getProgram().hasAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor));
  CHECK(material.getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(1.f));
  REQUIRE(material.getProgram().hasAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive));
  CHECK(material.getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.f));
  REQUIRE(material.getProgram().hasAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Ambient));
  CHECK(material.getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Ambient) == Raz::Vec3f(1.f));
  REQUIRE(material.getProgram().hasAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Specular));
  CHECK(material.getProgram().getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Specular) == Raz::Vec3f(1.f));
  REQUIRE(material.getProgram().hasAttribute<float>(Raz::MaterialAttribute::Opacity));
  CHECK(material.getProgram().getAttribute<float>(Raz::MaterialAttribute::Opacity) == 1.f);

  CHECK(material.getProgram().getTextureCount() == 6);
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::BaseColor));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Emissive));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Ambient));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Specular));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Opacity));
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::Bump));

  material.getProgram().clearAttributes();
  material.getProgram().clearTextures();
  material.loadType(Raz::MaterialType::SINGLE_TEXTURE_2D);

  CHECK(material.getProgram().getAttributeCount() == 0);

  CHECK(material.getProgram().getTextureCount() == 1);
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::BaseColor));

  material.getProgram().clearAttributes();
  material.getProgram().clearTextures();
  material.loadType(Raz::MaterialType::SINGLE_TEXTURE_3D);

  CHECK(material.getProgram().getAttributeCount() == 0);

  CHECK(material.getProgram().getTextureCount() == 1);
  CHECK(material.getProgram().hasTexture(Raz::MaterialTexture::BaseColor));
}
