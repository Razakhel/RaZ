#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Render/MeshRenderer.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

Raz::Mesh createMesh() {
  Raz::Mesh mesh;

  {
    Raz::Submesh& submesh = mesh.addSubmesh();

    std::vector<Raz::Vertex>& vertices = submesh.getVertices();
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(10.f), Raz::Vec2f(0.f), Raz::Axis::X });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(20.f), Raz::Vec2f(0.1f), Raz::Axis::Y });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(30.f), Raz::Vec2f(0.2f), Raz::Axis::Z });

    std::vector<unsigned int>& indices = submesh.getTriangleIndices();
    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
  }

  {
    Raz::Submesh& submesh = mesh.addSubmesh();

    std::vector<Raz::Vertex>& vertices = submesh.getVertices();
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(100.f), Raz::Vec2f(0.8f), Raz::Axis::Z });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(200.f), Raz::Vec2f(0.9f), Raz::Axis::X });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(300.f), Raz::Vec2f(1.f), Raz::Axis::Y });

    std::vector<unsigned int>& indices = submesh.getTriangleIndices();
    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
  }

  return mesh;
}

Raz::MeshRenderer createMeshRenderer() {
  Raz::MeshRenderer meshRenderer;

  meshRenderer.addSubmeshRenderer().setMaterialIndex(0);
  meshRenderer.addSubmeshRenderer().setMaterialIndex(1);

  {
    Raz::RenderShaderProgram& matProgram = meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::COOK_TORRANCE)).getProgram();
    matProgram.setAttribute(Raz::Vec3f(1.f, 0.f, 0.f), Raz::MaterialAttribute::BaseColor);
    matProgram.setAttribute(Raz::Vec3f(0.f, 1.f, 0.f), Raz::MaterialAttribute::Emissive);
    matProgram.setAttribute(0.25f, Raz::MaterialAttribute::Metallic);
    matProgram.setAttribute(0.75f, Raz::MaterialAttribute::Roughness);
  }

  {
    Raz::RenderShaderProgram& matProgram = meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::BLINN_PHONG)).getProgram();
    matProgram.setAttribute(Raz::Vec3f(1.f, 0.f, 0.f), Raz::MaterialAttribute::BaseColor);
    matProgram.setAttribute(Raz::Vec3f(0.f, 1.f, 0.f), Raz::MaterialAttribute::Emissive);
    matProgram.setAttribute(Raz::Vec3f(0.f, 0.f, 1.f), Raz::MaterialAttribute::Ambient);
    matProgram.setAttribute(Raz::Vec3f(1.f, 0.f, 1.f), Raz::MaterialAttribute::Specular);
    matProgram.setAttribute(0.5f, Raz::MaterialAttribute::Opacity);
  }

  return meshRenderer;
}

} // namespace

TEST_CASE("ObjFormat load quad faces", "[data]") {
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT "../assets/meshes/ballQuads.obj");

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 439);
  CHECK(mesh.recoverTriangleCount() == 760);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  CHECK(meshRenderer.getMaterials().size() == 1);
}

TEST_CASE("ObjFormat load Blinn-Phong", "[data]") {
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT "assets/meshes/çûbè_BP.obj");

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.recoverTriangleCount() == 12);

  const Raz::Submesh& submesh = mesh.getSubmeshes().front();

  auto checkData = [&submesh] (std::size_t startIndex, const Raz::Vec3f& normal, const Raz::Vec3f& tangent) {
    REQUIRE(tangent.dot(normal) == 0.f);

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex    ]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 1]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 2]].position).isCounterClockwise(normal));

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 3]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 4]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 5]].position).isCounterClockwise(normal));

    for (std::size_t i = startIndex; i < startIndex + 6; ++i) {
      const Raz::Vertex& vert = submesh.getVertices()[submesh.getTriangleIndices()[i]];

      CHECK(vert.normal.strictlyEquals(normal));
      CHECK(vert.tangent.strictlyEquals(tangent));
    }
  };

  checkData(0, Raz::Axis::Y, Raz::Axis::X);
  checkData(6, -Raz::Axis::X, Raz::Axis::Z);
  checkData(12, Raz::Axis::X, -Raz::Axis::Z);
  checkData(18, -Raz::Axis::Z, -Raz::Axis::X);
  checkData(24, Raz::Axis::Z, Raz::Axis::X);
  checkData(30, -Raz::Axis::Y, Raz::Axis::X);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  REQUIRE(meshRenderer.getMaterials().size() == 1);

  const Raz::RenderShaderProgram& matProgram = meshRenderer.getMaterials().front().getProgram();

  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.99f));
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.75f));
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Ambient) == Raz::Vec3f(0.5f));
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Specular) == Raz::Vec3f(0.25f));

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Diffuse map
  {
    const auto& diffuseMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::BaseColor));

    CHECK(diffuseMap.getWidth() == 2);
    CHECK(diffuseMap.getHeight() == 2);
    CHECK(diffuseMap.getColorspace() == Raz::TextureColorspace::SRGBA);
    REQUIRE(diffuseMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image diffuseImg = diffuseMap.recoverImage();
    REQUIRE_FALSE(diffuseImg.isEmpty());

    // RGBR image with alpha, flipped vertically: checking that values are BRRG with 50% opacity

    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------

    CHECK(diffuseImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(diffuseImg.recoverByteValue(0, 0, 1) == 0);
    CHECK(diffuseImg.recoverByteValue(0, 0, 2) == 255);
    CHECK(diffuseImg.recoverByteValue(0, 0, 3) == 127);

    CHECK(diffuseImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(diffuseImg.recoverByteValue(1, 0, 1) == 0);
    CHECK(diffuseImg.recoverByteValue(1, 0, 2) == 0);
    CHECK(diffuseImg.recoverByteValue(1, 0, 3) == 127);

    CHECK(diffuseImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(diffuseImg.recoverByteValue(0, 1, 1) == 0);
    CHECK(diffuseImg.recoverByteValue(0, 1, 2) == 0);
    CHECK(diffuseImg.recoverByteValue(0, 1, 3) == 127);

    CHECK(diffuseImg.recoverByteValue(1, 1, 0) == 0);
    CHECK(diffuseImg.recoverByteValue(1, 1, 1) == 255);
    CHECK(diffuseImg.recoverByteValue(1, 1, 2) == 0);
    CHECK(diffuseImg.recoverByteValue(1, 1, 3) == 127);
  }

  // Emissive map
  {
    const auto& emissiveMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Emissive));

    CHECK(emissiveMap.getWidth() == 2);
    CHECK(emissiveMap.getHeight() == 2);
    CHECK(emissiveMap.getColorspace() == Raz::TextureColorspace::SRGB);
    REQUIRE(emissiveMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image emissiveImg = emissiveMap.recoverImage();
    REQUIRE_FALSE(emissiveImg.isEmpty());

    // ---------
    // | R | R |
    // |-------|
    // | R | R |
    // ---------

    CHECK(emissiveImg.recoverByteValue(0, 0, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(0, 0, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(0, 0, 2) == 0);

    CHECK(emissiveImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(1, 0, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(1, 0, 2) == 0);

    CHECK(emissiveImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(0, 1, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(0, 1, 2) == 0);

    CHECK(emissiveImg.recoverByteValue(1, 1, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(1, 1, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(1, 1, 2) == 0);
  }

  // Ambient map
  {
    const auto& ambientMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Ambient));

    CHECK(ambientMap.getWidth() == 2);
    CHECK(ambientMap.getHeight() == 2);
    CHECK(ambientMap.getColorspace() == Raz::TextureColorspace::SRGB);
    REQUIRE(ambientMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image ambientImg = ambientMap.recoverImage();
    REQUIRE_FALSE(ambientImg.isEmpty());

    // ---------
    // | B | B |
    // |-------|
    // | B | B |
    // ---------

    CHECK(ambientImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(ambientImg.recoverByteValue(0, 0, 1) == 0);
    CHECK(ambientImg.recoverByteValue(0, 0, 2) == 255);

    CHECK(ambientImg.recoverByteValue(1, 0, 0) == 0);
    CHECK(ambientImg.recoverByteValue(1, 0, 1) == 0);
    CHECK(ambientImg.recoverByteValue(1, 0, 2) == 255);

    CHECK(ambientImg.recoverByteValue(0, 1, 0) == 0);
    CHECK(ambientImg.recoverByteValue(0, 1, 1) == 0);
    CHECK(ambientImg.recoverByteValue(0, 1, 2) == 255);

    CHECK(ambientImg.recoverByteValue(1, 1, 0) == 0);
    CHECK(ambientImg.recoverByteValue(1, 1, 1) == 0);
    CHECK(ambientImg.recoverByteValue(1, 1, 2) == 255);
  }

  // Specular map
  {
    const auto& specularMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Specular));

    CHECK(specularMap.getWidth() == 2);
    CHECK(specularMap.getHeight() == 2);
    CHECK(specularMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(specularMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image specularImg = specularMap.recoverImage();
    REQUIRE_FALSE(specularImg.isEmpty());

    // ---------
    // | X |   |
    // |-------|
    // |   | X |
    // ---------

    CHECK(specularImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(specularImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(specularImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(specularImg.recoverByteValue(1, 1, 0) == 0);
  }

  // Opacity map
  {
    const auto& opacityMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Opacity));

    CHECK(opacityMap.getWidth() == 2);
    CHECK(opacityMap.getHeight() == 2);
    CHECK(opacityMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(opacityMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image opacityImg = opacityMap.recoverImage();
    REQUIRE_FALSE(opacityImg.isEmpty());

    // ---------
    // | X | X |
    // |-------|
    // | X | X |
    // ---------

    CHECK(opacityImg.recoverByteValue(0, 0, 0) == 255);
    CHECK(opacityImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(opacityImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(opacityImg.recoverByteValue(1, 1, 0) == 255);
  }

  // Bump map
  {
    const auto& bumpMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Bump));

    CHECK(bumpMap.getWidth() == 2);
    CHECK(bumpMap.getHeight() == 2);
    CHECK(bumpMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(bumpMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image bumpImg = bumpMap.recoverImage();
    REQUIRE_FALSE(bumpImg.isEmpty());

    // ---------
    // |   |   |
    // |-------|
    // |   |   |
    // ---------

    CHECK(bumpImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(bumpImg.recoverByteValue(1, 0, 0) == 0);
    CHECK(bumpImg.recoverByteValue(0, 1, 0) == 0);
    CHECK(bumpImg.recoverByteValue(1, 1, 0) == 0);
  }
}

TEST_CASE("ObjFormat load Cook-Torrance", "[data]") {
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT "assets/meshes/çûbè_CT.obj");

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.recoverTriangleCount() == 12);

  const Raz::Submesh& submesh = mesh.getSubmeshes().front();

  auto checkData = [&submesh] (std::size_t startIndex, const Raz::Vec3f& normal, const Raz::Vec3f& tangent) {
    REQUIRE(tangent.dot(normal) == 0.f);

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 1]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 2]].position).isCounterClockwise(normal));

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 3]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 4]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 5]].position).isCounterClockwise(normal));

    for (std::size_t i = startIndex; i < startIndex + 6; ++i) {
      const Raz::Vertex& vert = submesh.getVertices()[submesh.getTriangleIndices()[i]];

      CHECK(vert.normal.strictlyEquals(normal));
      CHECK(vert.tangent.strictlyEquals(tangent));
    }
  };

  checkData(0, Raz::Axis::Y, Raz::Axis::X);
  checkData(6, -Raz::Axis::X, Raz::Axis::Z);
  checkData(12, Raz::Axis::X, -Raz::Axis::Z);
  checkData(18, -Raz::Axis::Z, -Raz::Axis::X);
  checkData(24, Raz::Axis::Z, Raz::Axis::X);
  checkData(30, -Raz::Axis::Y, Raz::Axis::X);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  REQUIRE(meshRenderer.getMaterials().size() == 1);

  const Raz::RenderShaderProgram& matProgram = meshRenderer.getMaterials().front().getProgram();

  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.99f));
  CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.75f));
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.5f);
  CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 0.25f);

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Albedo map
  {
    const auto& albedoMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::BaseColor));

    CHECK(albedoMap.getWidth() == 2);
    CHECK(albedoMap.getHeight() == 2);
    CHECK(albedoMap.getColorspace() == Raz::TextureColorspace::SRGBA);
    REQUIRE(albedoMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image albedoImg = albedoMap.recoverImage();
    REQUIRE_FALSE(albedoImg.isEmpty());

    // RGBR image with alpha, flipped vertically: checking that values are BRRG with 50% opacity

    // ---------
    // | R | G |
    // |-------|
    // | B | R |
    // ---------

    CHECK(albedoImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(albedoImg.recoverByteValue(0, 0, 1) == 0);
    CHECK(albedoImg.recoverByteValue(0, 0, 2) == 255);
    CHECK(albedoImg.recoverByteValue(0, 0, 3) == 127);

    CHECK(albedoImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(albedoImg.recoverByteValue(1, 0, 1) == 0);
    CHECK(albedoImg.recoverByteValue(1, 0, 2) == 0);
    CHECK(albedoImg.recoverByteValue(1, 0, 3) == 127);

    CHECK(albedoImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(albedoImg.recoverByteValue(0, 1, 1) == 0);
    CHECK(albedoImg.recoverByteValue(0, 1, 2) == 0);
    CHECK(albedoImg.recoverByteValue(0, 1, 3) == 127);

    CHECK(albedoImg.recoverByteValue(1, 1, 0) == 0);
    CHECK(albedoImg.recoverByteValue(1, 1, 1) == 255);
    CHECK(albedoImg.recoverByteValue(1, 1, 2) == 0);
    CHECK(albedoImg.recoverByteValue(1, 1, 3) == 127);
  }

  // Emissive map
  {
    const auto& emissiveMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Emissive));

    CHECK(emissiveMap.getWidth() == 2);
    CHECK(emissiveMap.getHeight() == 2);
    CHECK(emissiveMap.getColorspace() == Raz::TextureColorspace::SRGB);
    REQUIRE(emissiveMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image emissiveImg = emissiveMap.recoverImage();
    REQUIRE_FALSE(emissiveImg.isEmpty());

    // ---------
    // | R | R |
    // |-------|
    // | R | R |
    // ---------

    CHECK(emissiveImg.recoverByteValue(0, 0, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(0, 0, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(0, 0, 2) == 0);

    CHECK(emissiveImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(1, 0, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(1, 0, 2) == 0);

    CHECK(emissiveImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(0, 1, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(0, 1, 2) == 0);

    CHECK(emissiveImg.recoverByteValue(1, 1, 0) == 255);
    CHECK(emissiveImg.recoverByteValue(1, 1, 1) == 0);
    CHECK(emissiveImg.recoverByteValue(1, 1, 2) == 0);
  }

  // Normal map
  {
    const auto& normalMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Normal));

    CHECK(normalMap.getWidth() == 2);
    CHECK(normalMap.getHeight() == 2);
    CHECK(normalMap.getColorspace() == Raz::TextureColorspace::RGB);
    REQUIRE(normalMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image normalImg = normalMap.recoverImage();
    REQUIRE_FALSE(normalImg.isEmpty());

    // ---------
    // | B | B |
    // |-------|
    // | B | B |
    // ---------

    CHECK(normalImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(normalImg.recoverByteValue(0, 0, 1) == 0);
    CHECK(normalImg.recoverByteValue(0, 0, 2) == 255);

    CHECK(normalImg.recoverByteValue(1, 0, 0) == 0);
    CHECK(normalImg.recoverByteValue(1, 0, 1) == 0);
    CHECK(normalImg.recoverByteValue(1, 0, 2) == 255);

    CHECK(normalImg.recoverByteValue(0, 1, 0) == 0);
    CHECK(normalImg.recoverByteValue(0, 1, 1) == 0);
    CHECK(normalImg.recoverByteValue(0, 1, 2) == 255);

    CHECK(normalImg.recoverByteValue(1, 1, 0) == 0);
    CHECK(normalImg.recoverByteValue(1, 1, 1) == 0);
    CHECK(normalImg.recoverByteValue(1, 1, 2) == 255);
  }

  // Metallic map
  {
    const auto& metallicMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Metallic));

    CHECK(metallicMap.getWidth() == 2);
    CHECK(metallicMap.getHeight() == 2);
    CHECK(metallicMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(metallicMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image metallicImg = metallicMap.recoverImage();
    REQUIRE_FALSE(metallicImg.isEmpty());

    // ---------
    // | X | X |
    // |-------|
    // | X | X |
    // ---------

    CHECK(metallicImg.recoverByteValue(0, 0, 0) == 255);
    CHECK(metallicImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(metallicImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(metallicImg.recoverByteValue(1, 1, 0) == 255);
  }

  // Roughness map
  {
    const auto& roughnessMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Roughness));

    CHECK(roughnessMap.getWidth() == 2);
    CHECK(roughnessMap.getHeight() == 2);
    CHECK(roughnessMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(roughnessMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image roughnessImg = roughnessMap.recoverImage();
    REQUIRE_FALSE(roughnessImg.isEmpty());

    // ---------
    // |   |   |
    // |-------|
    // |   |   |
    // ---------

    CHECK(roughnessImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(roughnessImg.recoverByteValue(1, 0, 0) == 0);
    CHECK(roughnessImg.recoverByteValue(0, 1, 0) == 0);
    CHECK(roughnessImg.recoverByteValue(1, 1, 0) == 0);
  }

  // Ambient occlusion map
  {
    const auto& ambientOcclusionMap = static_cast<const Raz::Texture2D&>(matProgram.getTexture(Raz::MaterialTexture::Ambient));

    CHECK(ambientOcclusionMap.getWidth() == 2);
    CHECK(ambientOcclusionMap.getHeight() == 2);
    CHECK(ambientOcclusionMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(ambientOcclusionMap.getDataType() == Raz::TextureDataType::BYTE);

    const Raz::Image ambientOcclusionImg = ambientOcclusionMap.recoverImage();
    REQUIRE_FALSE(ambientOcclusionImg.isEmpty());

    // ---------
    // | X |   |
    // |-------|
    // |   | X |
    // ---------

    CHECK(ambientOcclusionImg.recoverByteValue(0, 0, 0) == 0);
    CHECK(ambientOcclusionImg.recoverByteValue(1, 0, 0) == 255);
    CHECK(ambientOcclusionImg.recoverByteValue(0, 1, 0) == 255);
    CHECK(ambientOcclusionImg.recoverByteValue(1, 1, 0) == 0);
  }
}

TEST_CASE("ObjFormat save", "[data]") {
  const auto checkMeshData = [] (const Raz::Mesh& mesh) {
    CHECK(mesh.getSubmeshes().size() == 2);

    {
      const Raz::Submesh& submesh = mesh.getSubmeshes()[0];

      CHECK(submesh.getVertexCount() == 3);

      CHECK(submesh.getVertices()[0] == Raz::Vertex{ Raz::Vec3f(10.f), Raz::Vec2f(0.f), Raz::Axis::X });
      CHECK(submesh.getVertices()[1] == Raz::Vertex{ Raz::Vec3f(20.f), Raz::Vec2f(0.1f), Raz::Axis::Y });
      CHECK(submesh.getVertices()[2] == Raz::Vertex{ Raz::Vec3f(30.f), Raz::Vec2f(0.2f), Raz::Axis::Z });

      CHECK(submesh.getTriangleIndexCount() == 3);

      CHECK(submesh.getTriangleIndices()[0] == 0);
      CHECK(submesh.getTriangleIndices()[1] == 1);
      CHECK(submesh.getTriangleIndices()[2] == 2);
    }

    {
      const Raz::Submesh& submesh = mesh.getSubmeshes()[1];

      CHECK(submesh.getVertexCount() == 3);

      CHECK(submesh.getVertices()[0] == Raz::Vertex{ Raz::Vec3f(100.f), Raz::Vec2f(0.8f), Raz::Axis::Z });
      CHECK(submesh.getVertices()[1] == Raz::Vertex{ Raz::Vec3f(200.f), Raz::Vec2f(0.9f), Raz::Axis::X });
      CHECK(submesh.getVertices()[2] == Raz::Vertex{ Raz::Vec3f(300.f), Raz::Vec2f(1.f), Raz::Axis::Y });

      CHECK(submesh.getTriangleIndexCount() == 3);

      CHECK(submesh.getTriangleIndices()[0] == 0);
      CHECK(submesh.getTriangleIndices()[1] == 1);
      CHECK(submesh.getTriangleIndices()[2] == 2);
    }
  };

  const Raz::Mesh mesh = createMesh();
  Raz::ObjFormat::save("téstÊxpørt.obj", mesh);

  {
    const auto [meshData, meshRendererData] = Raz::ObjFormat::load("téstÊxpørt.obj");

    checkMeshData(meshData);

    CHECK(meshRendererData.getSubmeshRenderers().size() == 2);

    CHECK(meshRendererData.getSubmeshRenderers()[0].getMaterialIndex() == 0);
    CHECK(meshRendererData.getSubmeshRenderers()[1].getMaterialIndex() == 0);

    // A default Cook-Torrance material is added if none has been imported
    CHECK(meshRendererData.getMaterials().size() == 1);

    const Raz::RenderShaderProgram& matProgram = meshRendererData.getMaterials().front().getProgram();
    CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor).strictlyEquals(Raz::Vec3f(1.f, 1.f, 1.f)));
    CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive).strictlyEquals(Raz::Vec3f(0.f, 0.f, 0.f)));
    CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.f);
    CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 1.f);
  }

  const Raz::MeshRenderer meshRenderer = createMeshRenderer();
  Raz::ObjFormat::save("téstÊxpørt.obj", mesh, &meshRenderer);

  {
    const auto [meshData, meshRendererData] = Raz::ObjFormat::load("téstÊxpørt.obj");

    checkMeshData(meshData);

    CHECK(meshRendererData.getSubmeshRenderers().size() == 2);

    CHECK(meshRendererData.getSubmeshRenderers()[0].getMaterialIndex() == 0);
    CHECK(meshRendererData.getSubmeshRenderers()[1].getMaterialIndex() == 1);

    CHECK(meshRendererData.getMaterials().size() == 2);

    {
      const Raz::RenderShaderProgram& matProgram = meshRendererData.getMaterials()[0].getProgram();
      CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor).strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive).strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.25f);
      CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 0.75f);
    }

    {
      const Raz::RenderShaderProgram& matProgram = meshRendererData.getMaterials()[1].getProgram();
      CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor).strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive).strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Ambient).strictlyEquals(Raz::Vec3f(0.f, 0.f, 1.f)));
      CHECK(matProgram.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Specular).strictlyEquals(Raz::Vec3f(1.f, 0.f, 1.f)));
      CHECK(matProgram.getAttribute<float>(Raz::MaterialAttribute::Opacity) == 0.5f);
    }
  }
}
