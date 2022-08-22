#include "Catch.hpp"

#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
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
    material.setAttribute(Raz::Vec3f(1.f, 0.f, 0.f), "uniMaterial.baseColor");
    material.setAttribute(Raz::Vec3f(0.f, 1.f, 0.f), "uniMaterial.emissive");
    material.setAttribute(0.25f, "uniMaterial.metallicFactor");
    material.setAttribute(0.75f, "uniMaterial.roughnessFactor");
  }

  {
    Raz::Material& material = meshRenderer.addMaterial(Raz::Material(Raz::MaterialType::BLINN_PHONG));
    material.setAttribute(Raz::Vec3f(1.f, 0.f, 0.f), "uniMaterial.baseColor");
    material.setAttribute(Raz::Vec3f(0.f, 1.f, 0.f), "uniMaterial.emissive");
    material.setAttribute(Raz::Vec3f(0.f, 0.f, 1.f), "uniMaterial.ambient");
    material.setAttribute(Raz::Vec3f(1.f, 0.f, 1.f), "uniMaterial.specular");
    material.setAttribute(0.5f, "uniMaterial.transparency");
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

  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.baseColor") == Raz::Vec3f(0.99f));
  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.emissive") == Raz::Vec3f(0.75f));
  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.ambient") == Raz::Vec3f(0.5f));
  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.specular") == Raz::Vec3f(0.25f));

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Diffuse map
  {
    const Raz::Texture& diffuseMap = material.getTexture("uniMaterial.baseColorMap");

    REQUIRE_FALSE(diffuseMap.getImage().isEmpty());

    CHECK(diffuseMap.getImage().getColorspace() == Raz::ImageColorspace::RGBA);
    CHECK(diffuseMap.getImage().getWidth() == 2);
    CHECK(diffuseMap.getImage().getHeight() == 2);

    REQUIRE(diffuseMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* diffuseData = static_cast<const uint8_t*>(diffuseMap.getImage().getDataPtr());

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

  // Emissive map
  {
    const Raz::Texture& emissiveMap = material.getTexture("uniMaterial.emissiveMap");

    REQUIRE_FALSE(emissiveMap.getImage().isEmpty());

    CHECK(emissiveMap.getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(emissiveMap.getImage().getWidth() == 2);
    CHECK(emissiveMap.getImage().getHeight() == 2);

    REQUIRE(emissiveMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* emissiveData = static_cast<const uint8_t*>(emissiveMap.getImage().getDataPtr());

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

  // Ambient map
  {
    const Raz::Texture& ambientMap = material.getTexture("uniMaterial.ambientMap");

    REQUIRE_FALSE(ambientMap.getImage().isEmpty());

    CHECK(ambientMap.getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(ambientMap.getImage().getWidth() == 2);
    CHECK(ambientMap.getImage().getHeight() == 2);

    REQUIRE(ambientMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* ambientData = static_cast<const uint8_t*>(ambientMap.getImage().getDataPtr());

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
    const Raz::Texture& specularMap = material.getTexture("uniMaterial.specularMap");

    REQUIRE_FALSE(specularMap.getImage().isEmpty());

    CHECK(specularMap.getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(specularMap.getImage().getWidth() == 2);
    CHECK(specularMap.getImage().getHeight() == 2);

    REQUIRE(specularMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* specularData = static_cast<const uint8_t*>(specularMap.getImage().getDataPtr());

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

  // Transparency map
  {
    const Raz::Texture& transparencyMap = material.getTexture("uniMaterial.transparencyMap");

    REQUIRE_FALSE(transparencyMap.getImage().isEmpty());

    CHECK(transparencyMap.getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(transparencyMap.getImage().getWidth() == 2);
    CHECK(transparencyMap.getImage().getHeight() == 2);

    REQUIRE(transparencyMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* transparencyData = static_cast<const uint8_t*>(transparencyMap.getImage().getDataPtr());

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
    const Raz::Texture& bumpMap = material.getTexture("uniMaterial.bumpMap");

    REQUIRE_FALSE(bumpMap.getImage().isEmpty());

    CHECK(bumpMap.getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(bumpMap.getImage().getWidth() == 2);
    CHECK(bumpMap.getImage().getHeight() == 2);

    REQUIRE(bumpMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* bumpData = static_cast<const uint8_t*>(bumpMap.getImage().getDataPtr());

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

  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.baseColor") == Raz::Vec3f(0.99f));
  CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.emissive") == Raz::Vec3f(0.75f));
  CHECK(material.getAttribute<float>("uniMaterial.metallicFactor") == 0.5f);
  CHECK(material.getAttribute<float>("uniMaterial.roughnessFactor") == 0.25f);

  // Each texture is flipped vertically when imported; the values are checked accordingly:
  //    ---------
  //    | 3 | 4 |
  //    |-------|
  //    | 1 | 2 |
  //    ---------

  // Albedo map
  {
    const Raz::Texture& albedoMap = material.getTexture("uniMaterial.baseColorMap");

    REQUIRE_FALSE(albedoMap.getImage().isEmpty());

    CHECK(albedoMap.getImage().getColorspace() == Raz::ImageColorspace::RGBA);
    CHECK(albedoMap.getImage().getWidth() == 2);
    CHECK(albedoMap.getImage().getHeight() == 2);

    REQUIRE(albedoMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* albedoData = static_cast<const uint8_t*>(albedoMap.getImage().getDataPtr());

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

  // Emissive map
  {
    const Raz::Texture& emissiveMap = material.getTexture("uniMaterial.emissiveMap");

    REQUIRE_FALSE(emissiveMap.getImage().isEmpty());

    CHECK(emissiveMap.getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(emissiveMap.getImage().getWidth() == 2);
    CHECK(emissiveMap.getImage().getHeight() == 2);

    REQUIRE(emissiveMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* emissiveData = static_cast<const uint8_t*>(emissiveMap.getImage().getDataPtr());

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

  // Normal map
  {
    const Raz::Texture& normalMap = material.getTexture("uniMaterial.normalMap");

    REQUIRE_FALSE(normalMap.getImage().isEmpty());

    CHECK(normalMap.getImage().getColorspace() == Raz::ImageColorspace::RGB);
    CHECK(normalMap.getImage().getWidth() == 2);
    CHECK(normalMap.getImage().getHeight() == 2);

    REQUIRE(normalMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* normalData = static_cast<const uint8_t*>(normalMap.getImage().getDataPtr());

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
    const Raz::Texture& metallicMap = material.getTexture("uniMaterial.metallicMap");

    REQUIRE_FALSE(metallicMap.getImage().isEmpty());

    CHECK(metallicMap.getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(metallicMap.getImage().getWidth() == 2);
    CHECK(metallicMap.getImage().getHeight() == 2);

    REQUIRE(metallicMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* metallicData = static_cast<const uint8_t*>(metallicMap.getImage().getDataPtr());

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
    const Raz::Texture& roughnessMap = material.getTexture("uniMaterial.roughnessMap");

    REQUIRE_FALSE(roughnessMap.getImage().isEmpty());

    CHECK(roughnessMap.getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(roughnessMap.getImage().getWidth() == 2);
    CHECK(roughnessMap.getImage().getHeight() == 2);

    REQUIRE(roughnessMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* roughnessData = static_cast<const uint8_t*>(roughnessMap.getImage().getDataPtr());

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
    const Raz::Texture& ambientOcclusionMap = material.getTexture("uniMaterial.ambientMap");

    REQUIRE_FALSE(ambientOcclusionMap.getImage().isEmpty());

    CHECK(ambientOcclusionMap.getImage().getColorspace() == Raz::ImageColorspace::GRAY);
    CHECK(ambientOcclusionMap.getImage().getWidth() == 2);
    CHECK(ambientOcclusionMap.getImage().getHeight() == 2);

    REQUIRE(ambientOcclusionMap.getImage().getDataType() == Raz::ImageDataType::BYTE);

    const auto* ambientOccData = static_cast<const uint8_t*>(ambientOcclusionMap.getImage().getDataPtr());

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
    CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.baseColor").strictlyEquals(Raz::Vec3f(1.f, 1.f, 1.f)));
    CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.emissive").strictlyEquals(Raz::Vec3f(0.f, 0.f, 0.f)));
    CHECK(material.getAttribute<float>("uniMaterial.metallicFactor") == 0.f);
    CHECK(material.getAttribute<float>("uniMaterial.roughnessFactor") == 1.f);
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
      CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.baseColor").strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.emissive").strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(material.getAttribute<float>("uniMaterial.metallicFactor") == 0.25f);
      CHECK(material.getAttribute<float>("uniMaterial.roughnessFactor") == 0.75f);
    }

    {
      const Raz::Material& material = meshRendererData.getMaterials()[1];
      CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.baseColor").strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.emissive").strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.ambient").strictlyEquals(Raz::Vec3f(0.f, 0.f, 1.f)));
      CHECK(material.getAttribute<Raz::Vec3f>("uniMaterial.specular").strictlyEquals(Raz::Vec3f(1.f, 0.f, 1.f)));
      CHECK(material.getAttribute<float>("uniMaterial.transparency") == 0.5f);
    }
  }
}
