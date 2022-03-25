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
    auto material = Raz::MaterialCookTorrance::create();
    material->setBaseColor(Raz::Vec3f(1.f, 0.f, 0.f));
    material->setEmissive(Raz::Vec3f(0.f, 1.f, 0.f));
    material->setMetallicFactor(0.25f);
    material->setRoughnessFactor(0.75f);
    meshRenderer.addMaterial(std::move(material));
  }

  {
    auto material = Raz::MaterialBlinnPhong::create();
    material->setDiffuse(Raz::Vec3f(1.f, 0.f, 0.f));
    material->setEmissive(Raz::Vec3f(0.f, 1.f, 0.f));
    material->setAmbient(Raz::Vec3f(0.f, 0.f, 1.f));
    material->setSpecular(Raz::Vec3f(1.f, 0.f, 1.f));
    material->setTransparency(0.5f);
    meshRenderer.addMaterial(std::move(material));
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

  auto checkWindingOrder = [&submesh] (std::size_t startIndex, const Raz::Vec3f& normal) {
    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 1]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 2]].position).isCounterClockwise(normal));

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 3]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 4]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 5]].position).isCounterClockwise(normal));
  };

  checkWindingOrder(0, Raz::Axis::Y);
  checkWindingOrder(6, -Raz::Axis::X);
  checkWindingOrder(12, Raz::Axis::X);
  checkWindingOrder(18, -Raz::Axis::Z);
  checkWindingOrder(24, Raz::Axis::Z);
  checkWindingOrder(30, -Raz::Axis::Y);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  REQUIRE(meshRenderer.getMaterials().size() == 1);
  REQUIRE(meshRenderer.getMaterials().front()->getType() == Raz::MaterialType::BLINN_PHONG);

  const auto& material = static_cast<const Raz::MaterialBlinnPhong&>(*meshRenderer.getMaterials().front());

  CHECK(material.getBaseColor() == Raz::Vec3f(0.99f));
  CHECK(material.getEmissive() == Raz::Vec3f(0.75f));
  CHECK(material.getAmbient() == Raz::Vec3f(0.5f));
  CHECK(material.getSpecular() == Raz::Vec3f(0.25f));

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

  // Emissive map
  {
    CHECK(material.getEmissiveMap()->getBindingIndex() == 1);

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

  // Ambient map
  {
    CHECK(material.getAmbientMap()->getBindingIndex() == 2);

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
    CHECK(material.getSpecularMap()->getBindingIndex() == 3);

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
  const auto [mesh, meshRenderer] = Raz::ObjFormat::load(RAZ_TESTS_ROOT "assets/meshes/çûbè_CT.obj");

  CHECK(mesh.getSubmeshes().size() == 1);
  CHECK(mesh.recoverVertexCount() == 24);
  CHECK(mesh.recoverTriangleCount() == 12);

  const Raz::Submesh& submesh = mesh.getSubmeshes().front();

  auto checkWindingOrder = [&submesh] (std::size_t startIndex, const Raz::Vec3f& normal) {
    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 1]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 2]].position).isCounterClockwise(normal));

    CHECK(Raz::Triangle(submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 3]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 4]].position,
                        submesh.getVertices()[submesh.getTriangleIndices()[startIndex + 5]].position).isCounterClockwise(normal));
  };

  checkWindingOrder(0, Raz::Axis::Y);
  checkWindingOrder(6, -Raz::Axis::X);
  checkWindingOrder(12, Raz::Axis::X);
  checkWindingOrder(18, -Raz::Axis::Z);
  checkWindingOrder(24, Raz::Axis::Z);
  checkWindingOrder(30, -Raz::Axis::Y);

  CHECK(meshRenderer.getSubmeshRenderers().size() == 1);
  CHECK(meshRenderer.getSubmeshRenderers().front().getMaterialIndex() == 0);
  REQUIRE(meshRenderer.getMaterials().size() == 1);
  REQUIRE(meshRenderer.getMaterials().front()->getType() == Raz::MaterialType::COOK_TORRANCE);

  const auto& material = static_cast<const Raz::MaterialCookTorrance&>(*meshRenderer.getMaterials().front());

  CHECK(material.getBaseColor() == Raz::Vec3f(0.99f));
  CHECK(material.getEmissive() == Raz::Vec3f(0.75f));
  CHECK(material.getMetallicFactor() == 0.5f);
  CHECK(material.getRoughnessFactor() == 0.25f);

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

  // Emissive map
  {
    CHECK(material.getEmissiveMap()->getBindingIndex() == 1);

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

  // Normal map
  {
    CHECK(material.getNormalMap()->getBindingIndex() == 2);

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
    CHECK(material.getMetallicMap()->getBindingIndex() == 3);

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
    CHECK(material.getRoughnessMap()->getBindingIndex() == 4);

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
    CHECK(material.getAmbientOcclusionMap()->getBindingIndex() == 5);

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
    CHECK(meshRendererData.getMaterials().front()->getType() == Raz::MaterialType::COOK_TORRANCE);

    auto& material = static_cast<Raz::MaterialCookTorrance&>(*meshRendererData.getMaterials().front());
    CHECK(material.getBaseColor().strictlyEquals(Raz::Vec3f(1.f, 1.f, 1.f)));
    CHECK(material.getEmissive().strictlyEquals(Raz::Vec3f(0.f, 0.f, 0.f)));
    CHECK(material.getMetallicFactor() == 1.f);
    CHECK(material.getRoughnessFactor() == 1.f);
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
      CHECK(meshRendererData.getMaterials()[0]->getType() == Raz::MaterialType::COOK_TORRANCE);

      auto& material = static_cast<Raz::MaterialCookTorrance&>(*meshRendererData.getMaterials()[0]);
      CHECK(material.getBaseColor().strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(material.getEmissive().strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(material.getMetallicFactor() == 0.25f);
      CHECK(material.getRoughnessFactor() == 0.75f);
    }

    {
      CHECK(meshRendererData.getMaterials()[1]->getType() == Raz::MaterialType::BLINN_PHONG);

      auto& material = static_cast<Raz::MaterialBlinnPhong&>(*meshRendererData.getMaterials()[1]);
      CHECK(material.getBaseColor().strictlyEquals(Raz::Vec3f(1.f, 0.f, 0.f)));
      CHECK(material.getEmissive().strictlyEquals(Raz::Vec3f(0.f, 1.f, 0.f)));
      CHECK(material.getAmbient().strictlyEquals(Raz::Vec3f(0.f, 0.f, 1.f)));
      CHECK(material.getSpecular().strictlyEquals(Raz::Vec3f(1.f, 0.f, 1.f)));
      CHECK(material.getTransparency() == 0.5f);
    }
  }
}
