#include "Catch.hpp"

#include "RaZ/Render/Material.hpp"

TEST_CASE("Material attributes") {
  Raz::MaterialCookTorrance material;
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
