#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/GltfFormat.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GltfFormat load glTF", "[data]") {
  const auto [mesh, meshRenderer] = Raz::GltfFormat::load(RAZ_TESTS_ROOT "assets/meshes/çûbè.gltf");

  REQUIRE(mesh.getSubmeshes().size() == 3);
  REQUIRE(mesh.getSubmeshes()[0].getVertexCount() == 36);
  CHECK(mesh.getSubmeshes()[1].getVertexCount() == 36);
  CHECK(mesh.recoverVertexCount() == 108);
  CHECK(mesh.getSubmeshes()[0].getTriangleIndexCount() == 36);
  CHECK(mesh.getSubmeshes()[1].getTriangleIndexCount() == 36);
  CHECK(mesh.recoverTriangleCount() == 36); // Total index count / 3

  // Meshes are chained in the order 2 -> 1 -> 0, hence applying each of its parents' transforms before its own

  CHECK(mesh.getSubmeshes()[0].getVertices()[0]  == Raz::Vertex{ Raz::Vec3f(121.97f, 82.03f, 122.03f), Raz::Vec2f(0.f, 0.f), -Raz::Axis::X, -Raz::Axis::Z });
  CHECK(mesh.getSubmeshes()[0].getVertices()[1]  == Raz::Vertex{ Raz::Vec3f(121.97f, 82.01f, 122.01f), Raz::Vec2f(0.f, 1.f), -Raz::Axis::X, -Raz::Axis::Z });
  CHECK(mesh.getSubmeshes()[0].getVertices()[17] == Raz::Vertex{ Raz::Vec3f(121.99f, 82.01f, 122.03f), Raz::Vec2f(0.f, 1.f), -Raz::Axis::Y, -Raz::Axis::Z });
  CHECK(mesh.getSubmeshes()[0].getVertices()[35] == Raz::Vertex{ Raz::Vec3f(121.99f, 82.01f, 122.01f), Raz::Vec2f(0.f, 1.f), -Raz::Axis::Y, -Raz::Axis::Z });

  CHECK(mesh.getSubmeshes()[1].getVertices()[0]  == Raz::Vertex{ Raz::Vec3f(120.2f, 80.2f, 119.8f), Raz::Vec2f(0.f, 0.f), -Raz::Axis::Z, -Raz::Axis::Y });
  CHECK(mesh.getSubmeshes()[1].getVertices()[1]  == Raz::Vertex{ Raz::Vec3f(119.8f, 79.8f, 119.8f), Raz::Vec2f(0.f, 1.f), -Raz::Axis::Z, -Raz::Axis::Y });
  CHECK(mesh.getSubmeshes()[1].getVertices()[17] == Raz::Vertex{ Raz::Vec3f(119.8f, 80.2f, 120.2f), Raz::Vec2f(0.f, 1.f), -Raz::Axis::X, -Raz::Axis::Y });
  CHECK(mesh.getSubmeshes()[1].getVertices()[35] == Raz::Vertex{ Raz::Vec3f(119.8f, 79.8f, 120.2f), Raz::Vec2f(0.f, 1.f), -Raz::Axis::X, -Raz::Axis::Y });

  CHECK(mesh.getSubmeshes()[2].getVertices()[0]  == Raz::Vertex{ Raz::Vec3f(102.f, 98.f, 98.f),   Raz::Vec2f(0.f, 0.f), -Raz::Axis::Z, -Raz::Axis::X });
  CHECK(mesh.getSubmeshes()[2].getVertices()[1]  == Raz::Vertex{ Raz::Vec3f(98.f, 102.f, 98.f),   Raz::Vec2f(0.f, 1.f), -Raz::Axis::Z, -Raz::Axis::X });
  CHECK(mesh.getSubmeshes()[2].getVertices()[17] == Raz::Vertex{ Raz::Vec3f(102.f, 102.f, 102.f), Raz::Vec2f(0.f, 1.f),  Raz::Axis::Y, -Raz::Axis::X });
  CHECK(mesh.getSubmeshes()[2].getVertices()[35] == Raz::Vertex{ Raz::Vec3f(98.f, 102.f, 102.f),  Raz::Vec2f(0.f, 1.f),  Raz::Axis::Y, -Raz::Axis::X });

  REQUIRE(meshRenderer.getSubmeshRenderers().size() == 3);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getRenderMode() == Raz::RenderMode::TRIANGLE);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[1].getRenderMode() == Raz::RenderMode::TRIANGLE);
  CHECK(meshRenderer.getSubmeshRenderers()[1].getMaterialIndex() == 0);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getRenderMode() == Raz::RenderMode::POINT);
  CHECK(meshRenderer.getSubmeshRenderers()[2].getMaterialIndex() == 0);

  REQUIRE(meshRenderer.getMaterials().size() == 1);

  const Raz::RenderShaderProgram& matProgram = meshRenderer.getMaterials()[0].getProgram();

  CHECK(matProgram.getAttributeCount() == 5);
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.99f));
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.75f));
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.5f);
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 0.25f);
  CHECK(matProgram.getAttribute<Raz::Vec4f>(Raz::MaterialAttribute::Sheen) == Raz::Vec4f(0.1f, 0.2f, 0.3f, 0.4f));

  CHECK(matProgram.getTextureCount() == 7);

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::BaseColor));

    const auto& baseColorMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::BaseColor));

    REQUIRE(baseColorMap.getWidth() == 2);
    REQUIRE(baseColorMap.getHeight() == 2);
    REQUIRE(baseColorMap.getColorspace() == Raz::TextureColorspace::SRGBA);
    REQUIRE(baseColorMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image baseColorImg = baseColorMap.recoverImage();
    REQUIRE_FALSE(baseColorImg.isEmpty());

    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------

    CHECK(baseColorImg.recoverPixel<uint8_t, 4>(0, 0) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Red), 127));
    CHECK(baseColorImg.recoverPixel<uint8_t, 4>(1, 0) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Green), 127));
    CHECK(baseColorImg.recoverPixel<uint8_t, 4>(0, 1) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Blue), 127));
    CHECK(baseColorImg.recoverPixel<uint8_t, 4>(1, 1) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Red), 127));
  }

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::Emissive));

    const auto& emissiveMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Emissive));

    REQUIRE(emissiveMap.getWidth() == 2);
    REQUIRE(emissiveMap.getHeight() == 2);
    REQUIRE(emissiveMap.getColorspace() == Raz::TextureColorspace::SRGB);
    REQUIRE(emissiveMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image emissiveImg = emissiveMap.recoverImage();
    REQUIRE_FALSE(emissiveImg.isEmpty());

    // ---------
    // | R | R |
    // |-------|
    // | R | R |
    // ---------

    CHECK(emissiveImg.recoverPixel<uint8_t, 3>(0, 0) == Raz::Vec3b(Raz::ColorPreset::Red));
    CHECK(emissiveImg.recoverPixel<uint8_t, 3>(1, 0) == Raz::Vec3b(Raz::ColorPreset::Red));
    CHECK(emissiveImg.recoverPixel<uint8_t, 3>(0, 1) == Raz::Vec3b(Raz::ColorPreset::Red));
    CHECK(emissiveImg.recoverPixel<uint8_t, 3>(1, 1) == Raz::Vec3b(Raz::ColorPreset::Red));
  }

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::Normal));

    const auto& normalMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Normal));

    REQUIRE(normalMap.getWidth() == 2);
    REQUIRE(normalMap.getHeight() == 2);
    REQUIRE(normalMap.getColorspace() == Raz::TextureColorspace::RGB);
    REQUIRE(normalMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image normalImg = normalMap.recoverImage();
    REQUIRE_FALSE(normalImg.isEmpty());

    // ---------
    // | G | G |
    // |-------|
    // | G | G |
    // ---------

    CHECK(normalImg.recoverPixel<uint8_t, 3>(0, 0) == Raz::Vec3b(Raz::ColorPreset::Green));
    CHECK(normalImg.recoverPixel<uint8_t, 3>(1, 0) == Raz::Vec3b(Raz::ColorPreset::Green));
    CHECK(normalImg.recoverPixel<uint8_t, 3>(0, 1) == Raz::Vec3b(Raz::ColorPreset::Green));
    CHECK(normalImg.recoverPixel<uint8_t, 3>(1, 1) == Raz::Vec3b(Raz::ColorPreset::Green));
  }

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::Metallic));

    const auto& metallicMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Metallic));

    REQUIRE(metallicMap.getWidth() == 2);
    REQUIRE(metallicMap.getHeight() == 2);
    REQUIRE(metallicMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(metallicMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image metallicImg = metallicMap.recoverImage();
    REQUIRE_FALSE(metallicImg.isEmpty());

    // The metalness is taken from the image's blue (3rd) channel. The input image being:
    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------
    // The loaded texture should then be:
    // ---------
    // | 0 | 0 |
    // |-------|
    // | 1 | 0 |
    // ---------

    CHECK(metallicImg.recoverPixel<uint8_t>(0, 0) == 0);
    CHECK(metallicImg.recoverPixel<uint8_t>(1, 0) == 0);
    CHECK(metallicImg.recoverPixel<uint8_t>(0, 1) == 255);
    CHECK(metallicImg.recoverPixel<uint8_t>(1, 1) == 0);
  }

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::Roughness));

    const auto& roughnessMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Roughness));

    REQUIRE(roughnessMap.getWidth() == 2);
    REQUIRE(roughnessMap.getHeight() == 2);
    REQUIRE(roughnessMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(roughnessMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image roughnessImg = roughnessMap.recoverImage();
    REQUIRE_FALSE(roughnessImg.isEmpty());

    // The roughness is taken from the image's green (2nd) channel. The input image being:
    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------
    // The loaded texture should then be:
    // ---------
    // | 0 | 1 |
    // |-------|
    // | 0 | 0 |
    // ---------

    CHECK(roughnessImg.recoverPixel<uint8_t>(0, 0) == 0);
    CHECK(roughnessImg.recoverPixel<uint8_t>(1, 0) == 255);
    CHECK(roughnessImg.recoverPixel<uint8_t>(0, 1) == 0);
    CHECK(roughnessImg.recoverPixel<uint8_t>(1, 1) == 0);
  }

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::Sheen));

    const auto& sheenMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Sheen));

    REQUIRE(sheenMap.getWidth() == 2);
    REQUIRE(sheenMap.getHeight() == 2);
    REQUIRE(sheenMap.getColorspace() == Raz::TextureColorspace::SRGBA);
    REQUIRE(sheenMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image sheenImg = sheenMap.recoverImage();
    REQUIRE_FALSE(sheenImg.isEmpty());

    // RGBR image with 1001 alpha

    // ---------   ---------
    // | R | G |   | 1 | 0 |
    // |-------| + |-------|
    // | B | R |   | 0 | 1 |
    // ---------   ---------

    CHECK(sheenImg.recoverPixel<uint8_t, 4>(0, 0) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Red), 255));
    CHECK(sheenImg.recoverPixel<uint8_t, 4>(1, 0) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Green), 0));
    CHECK(sheenImg.recoverPixel<uint8_t, 4>(0, 1) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Blue), 0));
    CHECK(sheenImg.recoverPixel<uint8_t, 4>(1, 1) == Raz::Vec4b(Raz::Vec3b(Raz::ColorPreset::Red), 255));
  }

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::Ambient));

    const auto& ambientOcclusionMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Ambient));

    REQUIRE(ambientOcclusionMap.getWidth() == 2);
    REQUIRE(ambientOcclusionMap.getHeight() == 2);
    REQUIRE(ambientOcclusionMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(ambientOcclusionMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image ambientOcclusionImg = ambientOcclusionMap.recoverImage();
    REQUIRE_FALSE(ambientOcclusionImg.isEmpty());

    // The ambient occlusion is taken from the image's red (1st) channel. The input image being:
    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------
    // The loaded texture should then be:
    // ---------
    // | 1 | 0 |
    // |-------|
    // | 0 | 1 |
    // ---------

    CHECK(ambientOcclusionImg.recoverPixel<uint8_t>(0, 0) == 255);
    CHECK(ambientOcclusionImg.recoverPixel<uint8_t>(1, 0) == 0);
    CHECK(ambientOcclusionImg.recoverPixel<uint8_t>(0, 1) == 0);
    CHECK(ambientOcclusionImg.recoverPixel<uint8_t>(1, 1) == 255);
  }
}

TEST_CASE("GltfFormat load GLB simple", "[data]") {
  const auto [mesh, meshRenderer] = Raz::GltfFormat::load(RAZ_TESTS_ROOT "assets/meshes/ßøӾ.glb");

  REQUIRE(mesh.getSubmeshes().size() == 1);
  REQUIRE(mesh.getSubmeshes()[0].getVertexCount() == 24);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.getSubmeshes()[0].getTriangleIndexCount() == 36);
  CHECK(mesh.recoverTriangleCount() == 12); // Total index count / 3

  CHECK(mesh.getSubmeshes()[0].getVertices()[0]  == Raz::Vertex{ Raz::Vec3f(-0.5f, -0.5f, 0.5f), Raz::Vec2f(0.f, 0.f),  Raz::Axis::Z, Raz::Vec3f(0.f) });
  CHECK(mesh.getSubmeshes()[0].getVertices()[1]  == Raz::Vertex{ Raz::Vec3f(0.5f, -0.5f, 0.5f),  Raz::Vec2f(0.f, 0.f),  Raz::Axis::Z, Raz::Vec3f(0.f) });
  CHECK(mesh.getSubmeshes()[0].getVertices()[12] == Raz::Vertex{ Raz::Vec3f(-0.5f, 0.5f, 0.5f),  Raz::Vec2f(0.f, 0.f),  Raz::Axis::Y, Raz::Vec3f(0.f) });
  CHECK(mesh.getSubmeshes()[0].getVertices()[23] == Raz::Vertex{ Raz::Vec3f(0.5f, 0.5f, -0.5f),  Raz::Vec2f(0.f, 0.f), -Raz::Axis::Z, Raz::Vec3f(0.f) });

  REQUIRE(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getRenderMode() == Raz::RenderMode::TRIANGLE);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);

  REQUIRE(meshRenderer.getMaterials().size() == 1);

  const Raz::RenderShaderProgram& matProgram = meshRenderer.getMaterials()[0].getProgram();

  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.8f, 0.f, 0.f));
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.f));
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.f);
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 1.f);
}

TEST_CASE("GltfFormat load GLB textured", "[data]") {
  const auto [mesh, meshRenderer] = Raz::GltfFormat::load(RAZ_TESTS_ROOT "assets/meshes/ßøӾTêxtùrëd.glb");

  REQUIRE(mesh.getSubmeshes().size() == 1);
  REQUIRE(mesh.getSubmeshes()[0].getVertexCount() == 24);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.getSubmeshes()[0].getTriangleIndexCount() == 36);
  CHECK(mesh.recoverTriangleCount() == 12); // Total index count / 3

  CHECK(mesh.getSubmeshes()[0].getVertices()[0]  == Raz::Vertex{ Raz::Vec3f(-0.5f, -0.5f, 0.5f), Raz::Vec2f(0.f, 0.f),  Raz::Axis::Z, Raz::Vec3f(0.f) });
  CHECK(mesh.getSubmeshes()[0].getVertices()[1]  == Raz::Vertex{ Raz::Vec3f(0.5f, -0.5f, 0.5f),  Raz::Vec2f(0.f, 0.f),  Raz::Axis::Z, Raz::Vec3f(0.f) });
  CHECK(mesh.getSubmeshes()[0].getVertices()[12] == Raz::Vertex{ Raz::Vec3f(0.5f, -0.5f, 0.5f),  Raz::Vec2f(0.f, 0.f), -Raz::Axis::Y, Raz::Vec3f(0.f) });
  CHECK(mesh.getSubmeshes()[0].getVertices()[23] == Raz::Vertex{ Raz::Vec3f(0.5f, 0.5f, -0.5f),  Raz::Vec2f(1.f, 1.f), -Raz::Axis::Z, Raz::Axis::X });

  REQUIRE(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getRenderMode() == Raz::RenderMode::TRIANGLE);
  CHECK(meshRenderer.getSubmeshRenderers()[0].getMaterialIndex() == 0);

  REQUIRE(meshRenderer.getMaterials().size() == 1);

  const Raz::RenderShaderProgram& matProgram = meshRenderer.getMaterials()[0].getProgram();

  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::ColorPreset::White);
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::ColorPreset::Black);
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.f);
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 1.f);

  {
    REQUIRE(matProgram.hasTexture(Raz::MaterialTexture::BaseColor));

    const auto& baseColorMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::BaseColor));

    REQUIRE(baseColorMap.getWidth() == 256);
    REQUIRE(baseColorMap.getHeight() == 256);
    REQUIRE(baseColorMap.getColorspace() == Raz::TextureColorspace::SRGB);
    REQUIRE(baseColorMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image baseColorImg = baseColorMap.recoverImage();
    REQUIRE_FALSE(baseColorImg.isEmpty());

    CHECK(baseColorImg.recoverPixel<uint8_t, 3>(0, 0) == Raz::Vec3b(220));
    CHECK(baseColorImg.recoverPixel<uint8_t, 3>(64, 64) == Raz::Vec3b(108, 173, 223));
    CHECK(baseColorImg.recoverPixel<uint8_t, 3>(127, 127) == Raz::Vec3b(255));
    CHECK(baseColorImg.recoverPixel<uint8_t, 3>(167, 71) == Raz::Vec3b(255));
    CHECK(baseColorImg.recoverPixel<uint8_t, 3>(192, 192) == Raz::Vec3b(92, 135, 39));
    CHECK(baseColorImg.recoverPixel<uint8_t, 3>(255, 255) == Raz::Vec3b(220));
  }
}
