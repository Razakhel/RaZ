#include "Catch.hpp"

#include "RaZ/Render/Material.hpp"

TEST_CASE("Material attributes") {
  Raz::Material material(Raz::MaterialType::COOK_TORRANCE);
  CHECK(material.getAttributeCount() == 4);

  CHECK(material.hasAttribute("uniMaterial.baseColor"));
  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.baseColor") == Raz::Vec3f(1.f));
  CHECK(material.hasAttribute("uniMaterial.metallicFactor"));
  CHECK(material.getAttribute<float>("uniMaterial.metallicFactor") == 1.f);

  material.setAttribute(Raz::Vec3f(0.f), "uniMaterial.baseColor");
  CHECK(material.getAttributeCount() == 4); // The attribute already exists, none has been added
  CHECK(material.hasAttribute("uniMaterial.baseColor"));
  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.baseColor") == Raz::Vec3f(0.f)); // But its value has been changed

  material.removeAttribute("uniMaterial.baseColor");
  CHECK(material.getAttributeCount() == 3);
  CHECK_FALSE(material.hasAttribute("uniMaterial.baseColor"));

  material.clearAttributes();
  CHECK(material.getAttributeCount() == 0);

  material.setAttribute(42, "test");
  CHECK(material.getAttributeCount() == 1);
  CHECK(material.getAttribute<int>("test") == 42);
}

TEST_CASE("Material textures") {
  Raz::Material material(Raz::MaterialType::COOK_TORRANCE);
  CHECK(material.getTextureCount() == 6);

  CHECK(material.hasTexture("uniMaterial.baseColorMap"));

  // Textures can be recovered with either their index in the list or their uniform name
  const unsigned int origTextureIndex = material.getTexture("uniMaterial.baseColorMap").getIndex();
  CHECK(material.getTexture(0).getIndex() == origTextureIndex);

  material.setTexture(Raz::Texture::create(), "uniMaterial.baseColorMap");
  CHECK(material.getTextureCount() == 6); // The texture already exists, none has been added
  CHECK(material.hasTexture("uniMaterial.baseColorMap"));
  CHECK_FALSE(material.getTexture("uniMaterial.baseColorMap").getIndex() == origTextureIndex); // But its value has been changed

  material.removeTexture("uniMaterial.baseColorMap");
  CHECK(material.getTextureCount() == 5);
  CHECK_FALSE(material.hasTexture("uniMaterial.baseColorMap"));

  material.clearTextures();
  CHECK(material.getTextureCount() == 0);

  Raz::TexturePtr texture = Raz::Texture::create();
  material.setTexture(texture, "test");
  CHECK(material.getTextureCount() == 1);
  CHECK(material.hasTexture("test"));
  CHECK(material.hasTexture(*texture)); // Can be checked with either the uniform name or the texture itself

  // The same texture can be paired with different uniform names
  material.setTexture(texture, "test2");
  CHECK(material.getTextureCount() == 2);
  CHECK(material.hasTexture("test2"));
  CHECK(material.hasTexture(*texture));
  // Getting a texture with either of the uniform names returns the same
  CHECK(material.getTexture("test").getIndex() == material.getTexture("test2").getIndex());

  material.removeTexture(*texture);
  CHECK(material.getTextureCount() == 0); // Removing with a texture removes all associated entries
}