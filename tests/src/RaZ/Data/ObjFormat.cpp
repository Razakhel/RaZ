#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Render/MeshRenderer.hpp"

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
    Raz::Material& material = meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::COOK_TORRANCE));
    material.setAttribute(Raz::Vec3f(1.f, 0.f, 0.f), Raz::MaterialAttribute::BaseColor);
    material.setAttribute(Raz::Vec3f(0.f, 1.f, 0.f), Raz::MaterialAttribute::Emissive);
    material.setAttribute(0.25f, Raz::MaterialAttribute::Metallic);
    material.setAttribute(0.75f, Raz::MaterialAttribute::Roughness);
  }

  {
    Raz::Material& material = meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::BLINN_PHONG));
    material.setAttribute(Raz::Vec3f(1.f, 0.f, 0.f), Raz::MaterialAttribute::BaseColor);
    material.setAttribute(Raz::Vec3f(0.f, 1.f, 0.f), Raz::MaterialAttribute::Emissive);
    material.setAttribute(Raz::Vec3f(0.f, 0.f, 1.f), Raz::MaterialAttribute::Ambient);
    material.setAttribute(Raz::Vec3f(1.f, 0.f, 1.f), Raz::MaterialAttribute::Specular);
    material.setAttribute(0.5f, Raz::MaterialAttribute::Transparency);
  }

  return meshRenderer;
}

} // namespace

TEST_CASE("ObjFormat load quad faces") {
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT "../assets/meshes/ballQuads.obj");

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 439);
  CHECK(mesh.recoverTriangleCount() == 760);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  CHECK(meshRenderer.getMaterials().size() == 1);
}

TEST_CASE("ObjFormat load Blinn-Phong") {
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

  const Raz::Material& material = meshRenderer.getMaterials().front();

  CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.99f));
  CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.75f));
  CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Ambient) == Raz::Vec3f(0.5f));
  CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Specular) == Raz::Vec3f(0.25f));

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Diffuse map
  {
    const auto& diffuseMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::BaseColor));

    CHECK(diffuseMap.getWidth() == 2);
    CHECK(diffuseMap.getHeight() == 2);
    CHECK(diffuseMap.getColorspace() == Raz::TextureColorspace::RGBA);
    REQUIRE(diffuseMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image diffuseImg = diffuseMap.recoverImage();
    REQUIRE_FALSE(diffuseImg.isEmpty());

    const auto* diffuseData = static_cast<const uint8_t*>(diffuseImg.getDataPtr());

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
#endif
  }

  // Emissive map
  {
    const auto& emissiveMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Emissive));

    CHECK(emissiveMap.getWidth() == 2);
    CHECK(emissiveMap.getHeight() == 2);
    CHECK(emissiveMap.getColorspace() == Raz::TextureColorspace::RGB);
    REQUIRE(emissiveMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image emissiveImg = emissiveMap.recoverImage();
    REQUIRE_FALSE(emissiveImg.isEmpty());

    const auto* emissiveData = static_cast<const uint8_t*>(emissiveImg.getDataPtr());

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
#endif
  }

  // Ambient map
  {
    const auto& ambientMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Ambient));

    CHECK(ambientMap.getWidth() == 2);
    CHECK(ambientMap.getHeight() == 2);
    CHECK(ambientMap.getColorspace() == Raz::TextureColorspace::RGB);
    REQUIRE(ambientMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image ambientImg = ambientMap.recoverImage();
    REQUIRE_FALSE(ambientImg.isEmpty());

    const auto* ambientData = static_cast<const uint8_t*>(ambientImg.getDataPtr());

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
#endif
  }

  // Specular map
  {
    const auto& specularMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Specular));

    CHECK(specularMap.getWidth() == 2);
    CHECK(specularMap.getHeight() == 2);
    CHECK(specularMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(specularMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image specularImg = specularMap.recoverImage();
    REQUIRE_FALSE(specularImg.isEmpty());

    const auto* specularData = static_cast<const uint8_t*>(specularImg.getDataPtr());

    // ---------
    // | X |   |
    // |-------|
    // |   | X |
    // ---------

    CHECK(specularData[0] == 0);
    CHECK(specularData[1] == 255);
    CHECK(specularData[2] == 255);
    CHECK(specularData[3] == 0);
#endif
  }

  // Transparency map
  {
    const auto& transparencyMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Transparency));

    CHECK(transparencyMap.getWidth() == 2);
    CHECK(transparencyMap.getHeight() == 2);
    CHECK(transparencyMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(transparencyMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image transparencyImg = transparencyMap.recoverImage();
    REQUIRE_FALSE(transparencyImg.isEmpty());

    const auto* transparencyData = static_cast<const uint8_t*>(transparencyImg.getDataPtr());

    // ---------
    // | X | X |
    // |-------|
    // | X | X |
    // ---------

    CHECK(transparencyData[0] == 255);
    CHECK(transparencyData[1] == 255);
    CHECK(transparencyData[2] == 255);
    CHECK(transparencyData[3] == 255);
#endif
  }

  // Bump map
  {
    const auto& bumpMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Bump));

    CHECK(bumpMap.getWidth() == 2);
    CHECK(bumpMap.getHeight() == 2);
    CHECK(bumpMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(bumpMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image bumpImg = bumpMap.recoverImage();
    REQUIRE_FALSE(bumpImg.isEmpty());

    const auto* bumpData = static_cast<const uint8_t*>(bumpImg.getDataPtr());

    // ---------
    // |   |   |
    // |-------|
    // |   |   |
    // ---------

    CHECK(bumpData[0] == 0);
    CHECK(bumpData[1] == 0);
    CHECK(bumpData[2] == 0);
    CHECK(bumpData[3] == 0);
#endif
  }
}

TEST_CASE("ObjFormat load Cook-Torrance") {
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

  const Raz::Material& material = meshRenderer.getMaterials().front();

  CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor) == Raz::Vec3f(0.99f));
  CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive) == Raz::Vec3f(0.75f));
  CHECK(material.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.5f);
  CHECK(material.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 0.25f);

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Albedo map
  {
    const auto& albedoMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::BaseColor));

    CHECK(albedoMap.getWidth() == 2);
    CHECK(albedoMap.getHeight() == 2);
    CHECK(albedoMap.getColorspace() == Raz::TextureColorspace::RGBA);
    REQUIRE(albedoMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image albedoImg = albedoMap.recoverImage();
    REQUIRE_FALSE(albedoImg.isEmpty());

    const auto* albedoData = static_cast<const uint8_t*>(albedoImg.getDataPtr());

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
#endif
  }

  // Emissive map
  {
    const auto& emissiveMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Emissive));

    CHECK(emissiveMap.getWidth() == 2);
    CHECK(emissiveMap.getHeight() == 2);
    CHECK(emissiveMap.getColorspace() == Raz::TextureColorspace::RGB);
    REQUIRE(emissiveMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image emissiveImg = emissiveMap.recoverImage();
    REQUIRE_FALSE(emissiveImg.isEmpty());

    const auto* emissiveData = static_cast<const uint8_t*>(emissiveImg.getDataPtr());

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
#endif
  }

  // Normal map
  {
    const auto& normalMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Normal));

    CHECK(normalMap.getWidth() == 2);
    CHECK(normalMap.getHeight() == 2);
    CHECK(normalMap.getColorspace() == Raz::TextureColorspace::RGB);
    REQUIRE(normalMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image normalImg = normalMap.recoverImage();
    REQUIRE_FALSE(normalImg.isEmpty());

    const auto* normalData = static_cast<const uint8_t*>(normalImg.getDataPtr());

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
#endif
  }

  // Metallic map
  {
    const auto& metallicMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Metallic));

    CHECK(metallicMap.getWidth() == 2);
    CHECK(metallicMap.getHeight() == 2);
    CHECK(metallicMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(metallicMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image metallicImg = metallicMap.recoverImage();
    REQUIRE_FALSE(metallicImg.isEmpty());

    const auto* metallicData = static_cast<const uint8_t*>(metallicImg.getDataPtr());

    // ---------
    // | X | X |
    // |-------|
    // | X | X |
    // ---------

    CHECK(metallicData[0] == 255);
    CHECK(metallicData[1] == 255);
    CHECK(metallicData[2] == 255);
    CHECK(metallicData[3] == 255);
#endif
  }

  // Roughness map
  {
    const auto& roughnessMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Roughness));

    CHECK(roughnessMap.getWidth() == 2);
    CHECK(roughnessMap.getHeight() == 2);
    CHECK(roughnessMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(roughnessMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image roughnessImg = roughnessMap.recoverImage();
    REQUIRE_FALSE(roughnessImg.isEmpty());

    const auto* roughnessData = static_cast<const uint8_t*>(roughnessImg.getDataPtr());

    // ---------
    // |   |   |
    // |-------|
    // |   |   |
    // ---------

    CHECK(roughnessData[0] == 0);
    CHECK(roughnessData[1] == 0);
    CHECK(roughnessData[2] == 0);
    CHECK(roughnessData[3] == 0);
#endif
  }

  // Ambient occlusion map
  {
    const auto& ambientOcclusionMap = static_cast<const Raz::Texture2D&>(material.getTexture(Raz::MaterialTexture::Ambient));

    CHECK(ambientOcclusionMap.getWidth() == 2);
    CHECK(ambientOcclusionMap.getHeight() == 2);
    CHECK(ambientOcclusionMap.getColorspace() == Raz::TextureColorspace::GRAY);
    REQUIRE(ambientOcclusionMap.getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES)
    const Raz::Image ambientOcclusionImg = ambientOcclusionMap.recoverImage();
    REQUIRE_FALSE(ambientOcclusionImg.isEmpty());

    const auto* ambientOccData = static_cast<const uint8_t*>(ambientOcclusionImg.getDataPtr());

    // ---------
    // | X |   |
    // |-------|
    // |   | X |
    // ---------

    CHECK(ambientOccData[0] == 0);
    CHECK(ambientOccData[1] == 255);
    CHECK(ambientOccData[2] == 255);
    CHECK(ambientOccData[3] == 0);
#endif
  }
}

TEST_CASE("ObjFormat save") {
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

    const Raz::Material& material = meshRendererData.getMaterials().front();
    CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor).strictlyEquals(Raz::Vec3f(1.f, 1.f, 1.f)));
    CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive).strictlyEquals(Raz::Vec3f(0.f, 0.f, 0.f)));
    CHECK(material.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.f);
    CHECK(material.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 1.f);
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
      const Raz::Material& material = meshRendererData.getMaterials()[0];
      CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor).strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive).strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(material.getAttribute<float>(Raz::MaterialAttribute::Metallic) == 0.25f);
      CHECK(material.getAttribute<float>(Raz::MaterialAttribute::Roughness) == 0.75f);
    }

    {
      const Raz::Material& material = meshRendererData.getMaterials()[1];
      CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::BaseColor).strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Emissive).strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Ambient).strictlyEquals(Raz::Vec3f(0.f, 0.f, 1.f)));
      CHECK(material.getAttribute<Raz::Vec3f>(Raz::MaterialAttribute::Specular).strictlyEquals(Raz::Vec3f(1.f, 0.f, 1.f)));
      CHECK(material.getAttribute<float>(Raz::MaterialAttribute::Transparency) == 0.5f);
    }
  }
}
